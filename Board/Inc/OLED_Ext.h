/*
 * @Author: 思夜雪
 * @Date: 2026-04-21 18:13:30
 * @LastEditors: Do not edit
 * @LastEditTime: 2026-04-21 18:15:40
 * @Description:
 * @FilePath: \MDK-ARMd:\STM32\Smart desk lamp\Board\Inc\OLED_Ext.h
 */
#ifndef __OLED_EXT_H
#define __OLED_EXT_H
#include "OLED.h"
#include "stdint.h"

// 清除指定行（Line: 1-4）
void OLED_ClearLine(uint8_t Line);

// 显示坐姿提示
void OLED_ShowPostureWarning(char *str);

// 显示疲劳提示
void OLED_ShowFatigueWarning(char *str);

#endif