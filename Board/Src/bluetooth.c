#include "bluetooth.h"
#include "OLED.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim3;
extern uint16_t distance;
extern uint16_t adc_value;

// Lamp mode: 0=Auto, 1=Manual
uint8_t lamp_mode = MODE_AUTO;

#define RX_BUF_SIZE  64
#define RX_TIMEOUT_MS  500   // RX frame timeout: 500ms

uint8_t rx_buf[RX_BUF_SIZE];
uint8_t rx_data;
uint8_t rx_cnt = 0;
uint8_t rx_flag = 0;
uint32_t rx_start_tick = 0;  // Timestamp when first byte of current frame arrived

void Bluetooth_Init(void)
{
    rx_cnt = 0;
    rx_flag = 0;
    rx_start_tick = 0;
    memset(rx_buf, 0, RX_BUF_SIZE);
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
}

/**
  * @brief  Check for RX frame timeout.
  *         If no '\n' is received within RX_TIMEOUT_MS after the first byte,
  *         discard the incomplete frame and reset the buffer.
  *         Call this periodically from the main loop.
  */
void Bluetooth_RxTimeoutCheck(void)
{
    if(rx_cnt > 0 && (HAL_GetTick() - rx_start_tick) > RX_TIMEOUT_MS)
    {
        rx_cnt = 0;
        rx_flag = 0;
        memset(rx_buf, 0, RX_BUF_SIZE);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        if(rx_data == '\n')
        {
            rx_buf[rx_cnt] = '\0';
            rx_flag = 1;
            rx_cnt = 0;
        }
        else if(rx_data != '\r')
        {
            // Record timestamp on first byte of a new frame
            if(rx_cnt == 0)
            {
                rx_start_tick = HAL_GetTick();
            }

            if(rx_cnt < RX_BUF_SIZE - 1)
            {
                rx_buf[rx_cnt++] = rx_data;
            }
        }
        HAL_UART_Receive_IT(&huart1, &rx_data, 1);
    }
}

void Bluetooth_SendString(char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
}

// Set PWM duty cycle for lamp brightness
void Set_Brightness(uint8_t percent)
{
    if(percent > 100) percent = 100;
    uint32_t ccr = (percent * __HAL_TIM_GET_AUTORELOAD(&htim3)) / 100;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccr);
}

void Bluetooth_Process(void)
{
    // Check for incomplete frame timeout before processing
    Bluetooth_RxTimeoutCheck();

    if(rx_flag)
    {
        // Echo received data
        Bluetooth_SendString("Received: ");
        Bluetooth_SendString((char*)rx_buf);
        Bluetooth_SendString("\r\n");

        rx_flag = 0;

        if(strstr((char*)rx_buf, "AUTO") != NULL)
        {
            lamp_mode = MODE_AUTO;
            Bluetooth_SendString("OK: Switched to AUTO Mode\r\n");
        }
        else if(strstr((char*)rx_buf, "MANUAL") != NULL)
        {
            lamp_mode = MODE_MANUAL;
            Bluetooth_SendString("OK: Switched to MANUAL Mode\r\n");
        }
        else if(strstr((char*)rx_buf, "ON") != NULL)
        {
            if(lamp_mode == MODE_MANUAL) {
                Set_Brightness(100);
                OLED_ShowNum(1, 5, 100, 3);
                Bluetooth_SendString("OK: Lamp ON\r\n");
            } else {
                Bluetooth_SendString("Error: In AUTO Mode\r\n");
            }
        }
        else if(strstr((char*)rx_buf, "OFF") != NULL)
        {
            if(lamp_mode == MODE_MANUAL) {
                Set_Brightness(0);
                OLED_ShowNum(1, 5, 0, 3);
                Bluetooth_SendString("OK: Lamp OFF\r\n");
            } else {
                Bluetooth_SendString("Error: In AUTO Mode\r\n");
            }
        }
        else if(strstr((char*)rx_buf, "DIM:") != NULL)
        {
            if(lamp_mode == MODE_MANUAL) {
                uint8_t percent = atoi((char*)rx_buf + 4);
                Set_Brightness(percent);
                OLED_ShowNum(1, 5, percent, 3);
                char reply[32];
                sprintf(reply, "OK: Brightness %d%%\r\n", percent);
                Bluetooth_SendString(reply);
            } else {
                Bluetooth_SendString("Error: In AUTO Mode\r\n");
            }
        }
        else if(strstr((char*)rx_buf, "GET") != NULL)
        {
            char status[64];
            uint8_t brightness = (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) * 100) / __HAL_TIM_GET_AUTORELOAD(&htim3);
            char *mode_str = (lamp_mode == MODE_AUTO) ? "AUTO" : "MANUAL";
            sprintf(status, "Status: Mode=%s, Brightness=%d%%, Dist=%dcm, ADC=%d\r\n", mode_str, brightness, distance, adc_value);
            Bluetooth_SendString(status);
        }
        else
        {
            Bluetooth_SendString("Error: Unknown Command\r\n");
        }

        memset(rx_buf, 0, RX_BUF_SIZE);
    }
}