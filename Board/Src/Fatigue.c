#include "Fatigue.h"
#include "Buzzer.h"
#include "LED.h"
#include "Key.h"
#include "OLED.h"
#include "tim.h"

// ƣ��״̬ö�� (ʵ��Ӧ���滻Ϊ��Ĺ��ܺ���?)
typedef enum {
    FATIGUE_NORMAL = 0,
    FATIGUE_WARN_L1,
    FATIGUE_WARN_L2,
    FATIGUE_WARN_L3,
    FATIGUE_RESTING
} Fatigue_State_t;

static Fatigue_State_t fatigue_state = FATIGUE_NORMAL;
static uint32_t learning_start_tick = 0;
static uint32_t learning_pause_tick = 0;
static uint32_t total_learning_sec = 0;
static uint8_t is_learning = 0;
static uint8_t brightness_backup = 0;
static uint32_t alert_toggle_tick = 0;

// �ڲ���������ȡ��ǰ���Ȱٷֱ� (ʵ��Ӧ���滻Ϊ��Ĺ��ܺ���?)
static uint8_t Get_Brightness_Percent(void)
{
    extern TIM_HandleTypeDef htim3;
    return (uint8_t)((__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) * 100) / __HAL_TIM_GET_AUTORELOAD(&htim3));
}

// �ڲ���������������
static void Set_Brightness_Percent(uint8_t percent)
{
    if(percent > 100) percent = 100;
    uint32_t ccr = (percent * __HAL_TIM_GET_AUTORELOAD(&htim3)) / 100;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccr);
}

// ֹͣ�������ѣ�LED+��������
static void Stop_All_Alerts(void)
{
    LED_Off();
    Buzzer_Off();
}

void Fatigue_Init(void)
{
    fatigue_state = FATIGUE_NORMAL;
    total_learning_sec = 0;
    is_learning = 0;
    Stop_All_Alerts();
}

void Fatigue_Update(uint16_t distance, uint8_t brightness)
{
    uint8_t dist_valid = (distance >= LEARNING_DIST_MIN_CM) && (distance <= LEARNING_DIST_MAX_CM);
    uint8_t lamp_on = (brightness > 0);
    uint8_t current_learning = (dist_valid && lamp_on) ? 1 : 0;

    if(current_learning != is_learning)
    {
        is_learning = current_learning;
        if(current_learning)
        {
            if(learning_pause_tick != 0)
            {
                uint32_t pause_sec = (HAL_GetTick() - learning_pause_tick) / 1000;
                if(pause_sec > REST_TIMEOUT_SEC)
                {
                    total_learning_sec = 0;
                    learning_start_tick = HAL_GetTick();
                }
                else
                {
                    learning_start_tick += (HAL_GetTick() - learning_pause_tick);
                }
                learning_pause_tick = 0;
            }
            else
            {
                learning_start_tick = HAL_GetTick();
                total_learning_sec = 0;
            }
        }
        else
        {
            learning_pause_tick = HAL_GetTick();
            Stop_All_Alerts();
            if(fatigue_state != FATIGUE_WARN_L3 && fatigue_state != FATIGUE_RESTING)
            {
                fatigue_state = FATIGUE_NORMAL;
                OLED_ShowString(4, 1, "                ");
            }
        }
    }

    if(is_learning)
    {
        total_learning_sec = (HAL_GetTick() - learning_start_tick) / 1000;
    }

    switch(fatigue_state)
    {
        case FATIGUE_NORMAL:
            Stop_All_Alerts();

            if(total_learning_sec >= FATIGUE_TIME_3_MIN * 60)
            {
                fatigue_state = FATIGUE_WARN_L3;
                OLED_ShowString(4, 1, "                ");
                OLED_ShowString(4, 1, "Press Key to Rest");
            }
            else if(total_learning_sec >= FATIGUE_TIME_2_MIN * 60)
            {
                fatigue_state = FATIGUE_WARN_L2;
                alert_toggle_tick = HAL_GetTick();
                OLED_ShowString(4, 1, "                ");
                OLED_ShowString(4, 1, "45 Mins Break!");
            }
            else if(total_learning_sec >= FATIGUE_TIME_1_MIN * 60)
            {
                fatigue_state = FATIGUE_WARN_L1;
                alert_toggle_tick = HAL_GetTick();
                OLED_ShowString(4, 1, "                ");
                OLED_ShowString(4, 1, "20 Mins Look Far");
            }
            break;

        case FATIGUE_WARN_L1:
            if(HAL_GetTick() - alert_toggle_tick >= 1000)
            {
                alert_toggle_tick = HAL_GetTick();
                LED_Toggle();
            }

            // key press: stop alert, reset timer
            if(Key_IsPressed())
            {
                Stop_All_Alerts();
                fatigue_state = FATIGUE_NORMAL;
                total_learning_sec = 0;
                learning_start_tick = HAL_GetTick();
                learning_pause_tick = 0;
                OLED_ShowString(4, 1, "                ");
                break;
            }

            if(total_learning_sec >= FATIGUE_TIME_2_MIN * 60)
            {
                fatigue_state = FATIGUE_WARN_L2;
                alert_toggle_tick = HAL_GetTick();
                OLED_ShowString(4, 1, "                ");
                OLED_ShowString(4, 1, "45 Mins Break!");
            }
            break;

        case FATIGUE_WARN_L2:
            if(HAL_GetTick() - alert_toggle_tick >= 500)
            {
                alert_toggle_tick = HAL_GetTick();
                LED_Toggle();
            }

            // key press: stop alert, reset timer
            if(Key_IsPressed())
            {
                Stop_All_Alerts();
                fatigue_state = FATIGUE_NORMAL;
                total_learning_sec = 0;
                learning_start_tick = HAL_GetTick();
                learning_pause_tick = 0;
                OLED_ShowString(4, 1, "                ");
                break;
            }

            if(total_learning_sec >= FATIGUE_TIME_3_MIN * 60)
            {
                fatigue_state = FATIGUE_WARN_L3;
                brightness_backup = Get_Brightness_Percent();
                Set_Brightness_Percent(10);
                OLED_ShowString(4, 1, "                ");
                OLED_ShowString(4, 1, "Press Key to Rest");
            }
            break;

        case FATIGUE_WARN_L3:
            LED_On();
            Buzzer_On();

            if(Key_IsPressed())
            {
                fatigue_state = FATIGUE_RESTING;
                Set_Brightness_Percent(brightness_backup);
                Stop_All_Alerts();
                OLED_ShowString(4, 1, "                ");
                OLED_ShowString(4, 1, "Resting...");
                total_learning_sec = 0;
                learning_start_tick = HAL_GetTick();
                learning_pause_tick = HAL_GetTick();
            }
            break;

        case FATIGUE_RESTING:
            Stop_All_Alerts();

            if(HAL_GetTick() - learning_pause_tick >= REST_TIMEOUT_SEC)
            {
                fatigue_state = FATIGUE_NORMAL;
                OLED_ShowString(4, 1, "                ");
                learning_pause_tick = 0;
            }
            break;
    }
}

uint32_t Fatigue_GetLearningTime(void)
{
    return total_learning_sec;
}