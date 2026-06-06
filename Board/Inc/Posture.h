/*
 * @Author: 思夜雪
 * @Date: 2026-04-21 17:31:42
 * @LastEditors: Do not edit
 * @LastEditTime: 2026-05-27 13:41:12
 * @Description: 
 * @FilePath: \MDK-ARMd:\STM32\Smart desk lamp\Board\Inc\Posture.h
 */
#ifndef __POSTURE_H
#define __POSTURE_H
#include "main.h"
#include <stdint.h>

// 参数配置
#define BAD_POSTURE_DISTANCE_CM    25  // 不良坐姿距离阈值
#define POSTURE_WARN_TIME_1         5   // 一级提醒时间（秒）：屏幕提示
#define POSTURE_WARN_TIME_2         10  // 二级提醒时间（秒）：蜂鸣器短鸣
#define POSTURE_WARN_TIME_3         15  // 三级提醒时间（秒）：持续蜂鸣

void Posture_Init(void);
void Posture_Update(uint16_t current_distance); // 传入当前距离，主循环调用

#endif