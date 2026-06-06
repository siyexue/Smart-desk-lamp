/*
 * @Author: 思夜雪
 * @Date: 2026-04-21 17:32:22
 * @LastEditors: Do not edit
 * @LastEditTime: 2026-04-26 23:38:49
 * @Description: 
 * @FilePath: \MDK-ARMd:\STM32\Smart desk lamp\Board\Src\Posture.c
 */
#include "Posture.h"
#include "Buzzer.h"
#include "OLED.h"          // 替换 OLED_Ext.h

// 坐姿状态枚举
typedef enum {
    POSTURE_NORMAL = 0,
    POSTURE_WARN_L1,
    POSTURE_WARN_L2,
    POSTURE_WARN_L3
} Posture_State_t;

static Posture_State_t posture_state = POSTURE_NORMAL;
static uint32_t posture_bad_start_tick = 0;
static uint8_t posture_is_bad = 0;
static uint32_t beep_toggle_tick = 0;

void Posture_Init(void)
{
    posture_state = POSTURE_NORMAL;
    posture_is_bad = 0;
}

void Posture_Update(uint16_t current_distance)
{
    uint8_t current_is_bad = (current_distance < BAD_POSTURE_DISTANCE_CM) ? 1 : 0;

    if(current_is_bad != posture_is_bad)
    {
        posture_is_bad = current_is_bad;
        if(current_is_bad)
        {
            posture_bad_start_tick = HAL_GetTick();
            posture_state = POSTURE_NORMAL;
        }
        else
        {
            posture_state = POSTURE_NORMAL;
            Buzzer_Off();
            // 清除坐姿提示行（第3行）
            OLED_ShowString(3, 1, "                ");
        }
    }

    if(posture_is_bad)
    {
        uint32_t bad_duration = (HAL_GetTick() - posture_bad_start_tick) / 1000;

        switch(posture_state)
        {
            case POSTURE_NORMAL:
                if(bad_duration >= POSTURE_WARN_TIME_1)
                {
                    posture_state = POSTURE_WARN_L1;
                    // 显示 “Sit Straight!”
                    OLED_ShowString(3, 1, "                ");
                    OLED_ShowString(3, 1, "Sit Straight!");
                }
                break;

            case POSTURE_WARN_L1:
                if(bad_duration >= POSTURE_WARN_TIME_2)
                {
                    posture_state = POSTURE_WARN_L2;
                    beep_toggle_tick = HAL_GetTick();
                }
                break;

            case POSTURE_WARN_L2:
                if(HAL_GetTick() - beep_toggle_tick >= 500)
                {
                    beep_toggle_tick = HAL_GetTick();
                    Buzzer_Toggle();
                }
                // 显示 “WARNING!”
                OLED_ShowString(3, 1, "                ");
                OLED_ShowString(3, 1, "WARNING!");
                
                if(bad_duration >= POSTURE_WARN_TIME_3)
                {
                    posture_state = POSTURE_WARN_L3;
                    Buzzer_On();
                }
                break;

            case POSTURE_WARN_L3:
                Buzzer_On();
                // 显示 “CRITICAL!!!”
                OLED_ShowString(3, 1, "                ");
                OLED_ShowString(3, 1, "CRITICAL!!!");
                break;
        }
    }
}