#ifndef __KEY_H
#define __KEY_H
#include "main.h"

void Key_Init(void);
uint8_t Key_IsPressed(void); // 返回1表示按键被按下（消抖后）

#endif