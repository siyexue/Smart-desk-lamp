/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "HC-SR04.h"
#include "bluetooth.h"
#include "Buzzer.h"
#include "LED.h"
#include "Key.h"
#include "Posture.h"
#include "Fatigue.h"
//#include "OLED_Ext.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t distance = 0;     // 超声波测距值
uint16_t adc_value = 0;    // ADC值

/* 自动调光参数：实测标定值
 * 0 lux   → ADC 3554 → 占空比 93%（最暗环境，灯最亮）
 * 500 lux → ADC 1200 → 占空比 12%（最亮环境，灯最暗）
 * PA6 PWM (1kHz) → 100Ω → AO3400 MOSFET → 5V LED 灯带
 */
#define ADC_DARK        3499
#define ADC_BRIGHT      264
#define DUTY_MAX        100
#define DUTY_MIN        12
#define DEAD_ZONE       2       // 死区：变化小于此值不响应
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  自动调光：根据 ADC 值计算目标 PWM 占空比
  *         0 lux → 93%, 500 lux → 12%，线性映射 + 死区 + 平滑
  * @param  adc: 当前 ADC 原始值
  * @retval 滤波后的目标占空比 (0~100)
  */
uint8_t Auto_Brightness(uint16_t adc)
{
    static uint8_t last_duty = 50;
    uint8_t target;

    /* ADC 限幅 */
    if (adc > ADC_DARK)  adc = ADC_DARK;
    if (adc < ADC_BRIGHT) adc = ADC_BRIGHT;

    /* 分段线性映射：ADC 大（暗）→ duty 大，ADC 小（亮）→ duty 小 */
    /* 共12段，覆盖实际 ADC 范围 3499~264 */
    if (adc >= 3200) {
        /* 3200~3499 → 88~100 */
        target = 88 + (uint8_t)((uint32_t)(adc - 3200) * 12 / 299);
    } else if (adc >= 2900) {
        /* 2900~3200 → 76~88 */
        target = 76 + (uint8_t)((uint32_t)(adc - 2900) * 12 / 300);
    } else if (adc >= 2600) {
        /* 2600~2900 → 65~76 */
        target = 65 + (uint8_t)((uint32_t)(adc - 2600) * 11 / 300);
    } else if (adc >= 2300) {
        /* 2300~2600 → 54~65 */
        target = 54 + (uint8_t)((uint32_t)(adc - 2300) * 11 / 300);
    } else if (adc >= 2000) {
        /* 2000~2300 → 44~54 */
        target = 44 + (uint8_t)((uint32_t)(adc - 2000) * 10 / 300);
    } else if (adc >= 1700) {
        /* 1700~2000 → 34~44 */
        target = 34 + (uint8_t)((uint32_t)(adc - 1700) * 10 / 300);
    } else if (adc >= 1400) {
        /* 1400~1700 → 25~34 */
        target = 25 + (uint8_t)((uint32_t)(adc - 1400) *  9 / 300);
    } else if (adc >= 1100) {
        /* 1100~1400 → 18~25 */
        target = 18 + (uint8_t)((uint32_t)(adc - 1100) *  7 / 300);
    } else if (adc >= 800) {
        /* 800~1100 → 14~18 */
        target = 14 + (uint8_t)((uint32_t)(adc - 800) *  4 / 300);
    } else if (adc >= 500) {
        /* 500~800 → 12~14 */
        target = 12 + (uint8_t)((uint32_t)(adc - 500) *  2 / 300);
    } else {
        /* 264~500 → 12 */
        target = 12;
    }

    /* 死区：变化太小不响应，避免亮度抖动 */
    if (abs((int)target - (int)last_duty) <= DEAD_ZONE) {
        return last_duty;
    }

    /* 自适应步长：大变化快追，小变化慢调，消除突变同时保证响应速度 */
    int16_t diff = (int16_t)target - (int16_t)last_duty;
    uint8_t step;

    if (abs(diff) > 20)
        step = 5;       /* 大变化时快追 */
    else if (abs(diff) > 5)
        step = 2;       /* 中变化时中速 */
    else
        step = 1;       /* 小变化时慢调 */

    if (diff > 0) {
        last_duty += step;
        if (last_duty > target) last_duty = target;
    } else {
        last_duty -= step;
        if (last_duty < target) last_duty = target;
    }

    return last_duty;
}

/**
  * @brief  ADC 滑动平均滤波
  * @retval 滤波后的 ADC 值
  */
#define ADC_FILTER_SIZE  8

uint16_t ADC_GetFiltered(void)
{
    static uint16_t buf[ADC_FILTER_SIZE] = {0};
    static uint8_t  idx = 0;
    static uint32_t sum = 0;
    static uint8_t  filled = 0;

    sum -= buf[idx];
    buf[idx] = HAL_ADC_GetValue(&hadc1);
    sum += buf[idx];
    idx = (idx + 1) % ADC_FILTER_SIZE;

    if (!filled) {
        if (idx == 0) filled = 1;
    }

    uint8_t count = filled ? ADC_FILTER_SIZE : idx;
    if (count == 0) count = 1;  // 安全保护，防止除零
    return (uint16_t)(sum / count);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  OLED_Init();
  HC_SR04_Init();
  Bluetooth_Init();
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_ADC_Start(&hadc1);
  Buzzer_Init();
  Key_Init();
  LED_Init();
  Posture_Init();
  Fatigue_Init();
  OLED_Clear();
  //OLED_ShowString(1, 1, "ADC:");
  OLED_ShowString(1, 1, "brt:");
  OLED_ShowString(2, 1, "D:");
  OLED_ShowString(2, 6, "cm");
  //OLED_ShowString(3, 1, "Mode:"); 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    // 0. 超声波超时维护（非阻塞，每次循环都检查）
    HC_SR04_Update();

    // 1. 蓝牙命令处理
    Bluetooth_Process();

    // 2. 100ms 定时任务
    static uint32_t last_tick = 0;
    if(HAL_GetTick() - last_tick >= 100)
    {
        last_tick = HAL_GetTick();
        
        // 使用滑动平均滤波后的 ADC 值
        adc_value = ADC_GetFiltered();
        distance = HC_SR04_GetDistance_cm();

        // 自动调光：使用新的标定映射 + 平滑算法
        if(lamp_mode == MODE_AUTO)
        {
            uint8_t duty = Auto_Brightness(adc_value);
            Set_Brightness(duty);
            OLED_ShowNum(1, 5, duty, 3);
        }

        // OLED 显示
        OLED_ShowString(1,8,"%");
        OLED_ShowNum(2, 3, distance, 3);
        if(lamp_mode == MODE_AUTO) {
            OLED_ShowString(1, 10, "AUTO");
        } else {
            OLED_ShowString(1, 10 , "MANU");
        }
        
        // 显示学习时间 (MM:SS)
        uint32_t learn_sec = Fatigue_GetLearningTime();
        uint16_t mins = learn_sec / 60;
        uint8_t secs = learn_sec % 60;
        OLED_ShowNum(2, 9, mins, 2);
        OLED_ShowChar(2, 11, ':');
        OLED_ShowNum(2, 12, secs,2);
    }

    // ================= 姿态与疲劳检测 =================
    // 3. 姿态检测（传入距离）
    Posture_Update(distance);
    
    // 4. 疲劳检测（传入距离和当前亮度）
    uint8_t current_brightness = (__HAL_TIM_GET_COMPARE(&htim3, TIM_CHANNEL_1) * 100) / __HAL_TIM_GET_AUTORELOAD(&htim3);
    Fatigue_Update(distance, current_brightness);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */