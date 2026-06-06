/*
 * @Author: your name
 * @Date: 2026-04-12 17:22:03
 * @LastEditors: Do not edit
 * @LastEditTime: 2026-04-12 17:38:58
 * @Description: Bluetooth module header (HC-05/HC-06 UART)
 * @FilePath: \MDK-ARMd:\STM32\Smart desk lamp\Board\Inc\bluetooth.h
 */
#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"

// Lamp mode definition: 0=Auto, 1=Manual
#define MODE_AUTO   0
#define MODE_MANUAL 1

// Global lamp mode variable
extern uint8_t lamp_mode;

// Function prototypes
void Bluetooth_Init(void);
void Bluetooth_Process(void);
void Bluetooth_RxTimeoutCheck(void);
void Bluetooth_SendString(char *str);
void Set_Brightness(uint8_t percent);

#ifdef __cplusplus
}
#endif

#endif