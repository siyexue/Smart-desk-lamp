/*
 * @Author: 思夜雪
 * @Date: 2026-05-26 13:36:57
 * @LastEditors: Do not edit
 * @LastEditTime: 2026-06-03 13:54:49
 * @Description: 
 * @FilePath: \MDK-ARMd:\STM32\Smart desk lamp\Board\Inc\Fatigue.h
 */
/**
 * @Author: 
 * @Date: 2026-04-21 17:33:06
 * @LastEditors: Do not edit
 * @LastEditTime: 2026-05-13 17:49:58
 * @Description: 
 * @FilePath: \MDK-ARMd:\STM32\Smart desk lamp\Board\Inc\Fatigue.h
 */

#ifndef __FATIGUE_H
#define __FATIGUE_H

#include "main.h"
#include <stdint.h>

// 距离阈值定义
#define LEARNING_DIST_MIN_CM   25   // 学习状态最小距离
#define LEARNING_DIST_MAX_CM   50  // 学习状态最大距离
#define FATIGUE_TIME_1_MIN     20   // 一级疲劳：20分钟
#define FATIGUE_TIME_2_MIN     45   // 二级疲劳：45分钟
#define FATIGUE_TIME_3_MIN     60   // 三级疲劳：60分钟
#define REST_TIMEOUT_SEC        300000  // 休息超时：5分钟（自动清零）

void Fatigue_Init(void);
void Fatigue_Update(uint16_t distance, uint8_t brightness); // 传入距离与亮度
uint32_t Fatigue_GetLearningTime(void); // 获取当前学习时长

#endif