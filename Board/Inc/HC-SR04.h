#ifndef __HC_SR04_H
#define __HC_SR04_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include <stdint.h>

// 引脚定义（TRIG=PA4, ECHO=PA0/TIM2_CH1）
#define TRIG_Pin  GPIO_PIN_4
#define TRIG_Port GPIOA
#define ECHO_Pin  GPIO_PIN_0
#define ECHO_Port GPIOA

// 全局测距结果
extern volatile uint16_t g_distance_cm;
extern volatile uint8_t  g_measurement_ok;

// === API ===
void     HC_SR04_Init(void);
uint16_t HC_SR04_GetDistance_cm(void);   // 触发测量 + 返回距离
void     HC_SR04_Update(void);           // 主循环调用（超时处理）

// 微秒延迟
void     Delay_us(uint16_t us);

#ifdef __cplusplus
}
#endif

#endif /* __HC_SR04_H */
