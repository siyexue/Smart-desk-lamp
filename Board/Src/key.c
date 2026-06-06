/*
 * @Author: 思夜雪
 * @Date: 2026-04-21 17:29:58
 * @LastEditors: Do not edit
 * @LastEditTime: 2026-05-13 17:47:51
 * @Description: 
 * @FilePath: \MDK-ARMd:\STM32\Smart desk lamp\Board\Src\key.c
 */
#include "Key.h"

// 请根据你的CubeMX配置修改这里的引脚定义
#define KEY_PIN       GPIO_PIN_15
#define KEY_PORT      GPIOB

static uint32_t key_last_tick = 0;
static uint8_t key_last_state = 1; // 上拉输入，默认高电平（未按下）

/**
  * @brief  按键初始化（空函数，初始化已在CubeMX中完成）
  * @retval 无
  */
void Key_Init(void)
{
    // 预留接口
}

/**
  * @brief  读取按键状态（带消抖）
  * @retval 1: 按下; 0: 未按下
  */
uint8_t Key_IsPressed(void)
{
    uint8_t current_state = HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN);
    
    // 状态变化时记录时间
    if(current_state != key_last_state)
    {
        key_last_tick = HAL_GetTick();
        key_last_state = current_state;
    }
    
    // 消抖时间20ms，且检测到低电平（按下）
    if((HAL_GetTick() - key_last_tick) > 20)
    {
        if(current_state == GPIO_PIN_RESET)
        {
            return 1;
        }
    }
    return 0;
}