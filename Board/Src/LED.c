/*
 * @Author: 思夜雪
 * @Date: 2026-04-22 17:27:58
 * @LastEditors: Do not edit
 * @LastEditTime: 2026-04-26 23:56:32
 * @Description
 * @FilePath: \MDK-ARMd:\STM32\Smart desk lamp\Board\Src\LED.c
 */
#include "LED.h"

// 定义LED引脚 (PB12)
#define LED_PIN     GPIO_PIN_12
#define LED_PORT    GPIOB

/**
  * @brief  LED初始化（CubeMX已配置好，这里是空函数）
  * @retval 无
  */
void LED_Init(void)
{
    // 确保初始状态是灭的
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
}

void LED_On(void)  { HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET); }
void LED_Off(void) { HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET); }
void LED_Toggle(void) { HAL_GPIO_TogglePin(LED_PORT, LED_PIN); }