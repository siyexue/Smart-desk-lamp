/*
 * HC-SR04 超声波测距 — TIM2 硬件输入捕获版
 *
 * 原理：
 *   PA4(TRIG) 发 20µs 脉冲 → 模块发出 8 个 40kHz 超声波
 *   PA0(ECHO/TIM2_CH1) → PWM 输入模式捕获双边沿
 *     - CH1 上升沿 → 锁存 CCR1
 *     - CH2 下降沿 → 锁存 CCR2（脉宽 = CCR2 - CCR1）
 *   distance(cm) = pulse_width(µs) / 58
 *
 * 优点：
 *   硬件自动锁存，零 CPU 参与测量，理论零误差
 *   无需 EXTI，无需软件状态机，一个中断搞定
 */

#include "HC-SR04.h"
#include "tim.h"

volatile uint16_t g_distance_cm = 0;
volatile uint8_t  g_measurement_ok = 0;

static volatile uint8_t  sr04_triggered = 0;      // 已发脉冲，等待 ECHO
static volatile uint32_t sr04_timeout   = 0;       // 超时计数值

/* ====================== 初始化 ====================== */
void HC_SR04_Init(void)
{
    /* 1. 拉低 TRIG */
    HAL_GPIO_WritePin(TRIG_Port, TRIG_Pin, GPIO_PIN_RESET);

    /* 2. 启动 TIM2 时基（1MHz 自由运行计数器） */
    HAL_TIM_Base_Start(&htim2);

    /* 3. 启动输入捕获中断（CH1=上升沿, CH2=下降沿） */
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
}

/* ====================== 微秒延迟（保留兼容） ====================== */
void Delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);
}

/* ====================== 发送 20µs 触发脉冲 ====================== */
static void HC_SR04_Trigger(void)
{
    /* 清零计数器 */
    __HAL_TIM_SET_COUNTER(&htim2, 0);

    /* 拉高 20µs */
    TRIG_Port->BSRR = TRIG_Pin;
    while (__HAL_TIM_GET_COUNTER(&htim2) < 20);
    TRIG_Port->BRR  = TRIG_Pin;

    /* 标记：等待输入捕获完成 */
    sr04_triggered = 1;
    sr04_timeout   = __HAL_TIM_GET_COUNTER(&htim2);  // ≈20
}

/* ====================== 非阻塞读距离 ====================== */
uint16_t HC_SR04_GetDistance_cm(void)
{
    /* 空闲时才触发新一轮测量 */
    if (!sr04_triggered) {
        HC_SR04_Trigger();
    }
    return g_distance_cm;
}

/* ====================== 超时处理（主循环中调用） ====================== */
void HC_SR04_Update(void)
{
    if (sr04_triggered) {
        uint32_t elapsed = __HAL_TIM_GET_COUNTER(&htim2) - sr04_timeout;

        /* 40ms 无应答 → 超时复位 */
        if (elapsed > 40000) {
            sr04_triggered = 0;
            g_measurement_ok = 0;
        }
    }
}

/* ====================== TIM2 输入捕获中断回调 ====================== */
/*
 * PWM 输入模式下：
 *   CH1 中断 → 上升沿，CCR1 锁存计数器值（时间戳）
 *   CH2 中断 → 下降沿，CCR2 锁存计数器值（时间戳）
 *   脉宽 = CCR2 - CCR1（单位：µs）
 *
 *   我们只在 CH2（下降沿）中断时计算结果，
 *   因为此时 CCR1 和 CCR2 都已更新完毕。
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM2)
        return;

    /* 只在下降沿（CH2）完成一次完整的脉宽测量 */
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 && sr04_triggered)
    {
        uint32_t falling = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
        uint32_t rising  = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        uint32_t pulse_us = falling - rising;  // 脉宽（µs）

        /* 有效范围：100µs ~ 38ms（≈2cm ~ 6.5m） */
        if (pulse_us >= 100 && pulse_us < 38000)
        {
            uint16_t dist = (uint16_t)(pulse_us / 58);
            if (dist > 400) dist = 400;       // 上限 4m
            if (dist < 2)   dist = 2;         // 下限 2cm
            g_distance_cm   = dist;
            g_measurement_ok = 1;
        }
        else
        {
            g_measurement_ok = 0;
        }

        sr04_triggered = 0;  // 本轮测量结束
    }
}
