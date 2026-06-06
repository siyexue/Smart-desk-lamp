/*
 * @Author: 思夜雪
 * @Date: 2026-04-21 17:28:32
 * @LastEditors: Do not edit
 * @LastEditTime: 2026-04-22 17:47:44
 * @Description: 
 * @FilePath: \MDK-ARMd:\STM32\Smart desk lamp\Board\Src\buzzer.c
 */
#include "Buzzer.h"

// 请根据你的CubeMX配置修改这里的引脚定义
#define BUZZER_PIN    GPIO_PIN_13
#define BUZZER_PORT   GPIOB

/**
  * @brief  蜂鸣器初始化（空函数，初始化已在CubeMX的MX_GPIO_Init中完成）
  * @retval 无
  */
void Buzzer_Init(void)
{
    // 预留接口，方便后续如果需要PWM驱动蜂鸣器时扩展
}

/**
  * @brief  蜂鸣器开
  * @retval 无
  */
void Buzzer_On(void)
{
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  蜂鸣器关
  * @retval 无
  */
void Buzzer_Off(void)
{
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

/**
  * @brief  蜂鸣器状态翻转
  * @retval 无
  */
void Buzzer_Toggle(void)
{
    HAL_GPIO_TogglePin(BUZZER_PORT, BUZZER_PIN);
}