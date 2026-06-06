# Smart Desk Lamp 🪄💡

> 基于 STM32F103C8T6 的智能阅读台灯 — 坐姿检测 · 疲劳定时提醒 · 环境光自适应调光 · 蓝牙通信

![GitHub last commit](https://img.shields.io/github/last-commit/siyexue/Smart-desk-lamp)
![GitHub language count](https://img.shields.io/github/languages/count/siyexue/Smart-desk-lamp)
![GitHub top language](https://img.shields.io/github/languages/top/siyexue/Smart-desk-lamp)

---

## 📖 项目简介

本项目基于 STM32F103C8T6 微控制器（HAL 库），设计并实现一款多功能智能阅读台灯。以嘉立创 EDA 完成 2 层 PCB 原理图设计，在面包板上完成硬件焊接与系统联调。

核心功能包括：**超声波坐姿检测**（三级声光报警）、**学习疲劳定时提醒**（基于累计学习时间）、**环境光自适应调光**（PWM 控制 LED 灯带）、**蓝牙无线通信**（远程控制与状态查询）。

适用于学习、办公、阅读等场景，帮助用户养成良好的用眼习惯。

---

## ✨ 功能特性

| 功能 | 说明 |
|------|------|
| 🌞 **环境光自适应调光** | 光敏电阻采集环境光照，经 ADC 采样 + 8 点滑动平均滤波后，通过分段线性映射算法自动调节 PWM 占空比，实现 LED 灯带亮度平滑过渡 |
| 📐 **超声波坐姿检测** | HC-SR04 超声波模块，定时器输入捕获模式测量回波脉宽，计算距离。距离 < 25cm 持续 5s/10s/15s 分别触发三级报警（OLED 提示 → 蜂鸣器交替 → 蜂鸣器常响） |
| ⏱️ **学习疲劳定时提醒** | 学习状态自动计时（坐姿正常 + 灯光亮起），累计 20/45/60 分钟分别触发三级提醒；离开 > 5 分钟自动归零；按键可进入 5 分钟休息模式 |
| 📱 **蓝牙无线通信** | HC-05 蓝牙模块，支持远程切换自动/手动模式、开关灯、调节亮度、查询状态 |
| 🕹️ **多级手动调光** | 按键切换自动/手动模式，手动模式下支持多档亮度调节 |
| 📟 **OLED 实时显示** | 4 行信息显示：亮度百分比 + 模式 / 距离 + 学习计时 / 姿态提示 / 疲劳提醒 |

---

## 🔧 硬件平台

### 核心器件

| 组件 | 型号 / 说明 |
|------|-------------|
| 主控芯片 | STM32F103C8T6 |
| 超声波模块 | HC-SR04 |
| 环境光传感器 | 光敏电阻分压电路 |
| 蓝牙模块 | HC-05 |
| 显示屏 | 0.96 寸 OLED（SSD1306，软件模拟 I2C） |
| LED 灯带 | 5V 120 灯 2835 裸板灯带 |
| 驱动管 | AO3400 N-MOSFET（逻辑电平，Vgs=3.3V 可完全导通） |
| 蜂鸣器 | 有源蜂鸣器 |
| 状态指示灯 | LED（PB12） |
| 按键 | 轻触按键（PB15，低电平有效） |
| 晶振 | 8 MHz 外部晶振，PLL ×9 → 72 MHz |

### 电源

| 参数 | 值 |
|------|-----|
| LED 灯带 | DC 5V / 2A |
| STM32 供电 | STLINK供电 |

### 硬件设计

- 使用嘉立创 EDA 完成 2 层 PCB 原理图设计
- 在面包板上完成硬件焊接与调试
- 使用杜邦线连接各模块

---

## 💻 开发环境

| 工具 | 说明 |
|------|------|
| IDE | Keil MDK-ARM |
| 芯片配置 | STM32CubeMX |
| 编译器 | ARMCC |
| 调试器 | J-Link / ST-Link |
| 版本管理 | Git + GitHub |

---

## 📁 项目结构

```
Smart-desk-lamp/
├── Core/                          # 核心代码
│   ├── Inc/                       # 头文件
│   └── Src/                       # 源文件
│       ├── main.c                 # 主程序（自动调光算法、主循环）
│       ├── gpio.c / tim.c / adc.c / usart.c
│       ├── stm32f1xx_it.c         # 中断服务
│       ├── stm32f1xx_hal_msp.c    # MSP 初始化
│       └── system_stm32f1xx.c
├── Board/                         # 板级外设驱动
│   ├── Inc/
│   │   ├── HC-SR04.h              # 超声波驱动
│   │   ├── bluetooth.h            # 蓝牙指令处理
│   │   ├── Posture.h              # 姿态检测
│   │   ├── Fatigue.h              # 疲劳检测
│   │   ├── LED.h / buzzer.h / key.h / OLED.h
│   │   └── OLED_Font.h / OLED_Ext.h
│   └── Src/
│       ├── HC-SR04.c              # 超声波驱动实现
│       ├── bluetooth.c            # 蓝牙指令解析
│       ├── Posture.c              # 三级姿态报警
│       ├── Fatigue.c              # 三级疲劳提醒
│       ├── LED.c / buzzer.c / key.c / OLED.c
│       └── OLED_Ext.c
├── Drivers/                       # HAL 库
├── MDK-ARM/                       # Keil 工程文件
├── Smart desk lamp.ioc            # CubeMX 引脚配置
├── 工程配置速查手册.md              # 完整工程配置文档
└── README.md                      # 本文件
```

---

## 🔌 引脚分配

| 引脚 | 功能 | 说明 |
|------|------|------|
| PA0 | ECHO | TIM2_CH1 输入捕获（超声波回波） |
| PA1 | Light Sensor | ADC1_IN1 模拟输入（环境光） |
| PA4 | TRIG | GPIO 输出（超声波触发） |
| PA6 | PWM_LED | TIM3_CH1 → 100Ω → AO3400 Gate |
| PA9 | BT_TX | USART1_TX（蓝牙发送） |
| PA10 | BT_RX | USART1_RX（蓝牙接收） |
| PB8 | OLED_SCL | 软件模拟 I2C 时钟 |
| PB9 | OLED_SDA | 软件模拟 I2C 数据 |
| PB12 | LED1 | 状态指示灯（低电平亮） |
| PB13 | BUZZER | 有源蜂鸣器（低电平响） |
| PB15 | KEY | 按键输入（低电平有效） |

---

## ⏱️ 定时器配置

### TIM2 — 超声波输入捕获

| 参数 | 值 |
|------|-----|
| 时钟源 | APB1 (36 MHz)，不分频 |
| 预分频 | 71 → 1 MHz 计数 (1 µs/tick) |
| 自动重装载 | 65535 (最大 65.535 ms) |
| 模式 | PWM 输入模式，CH1 上升沿 → CH2 下降沿 |
| 测距公式 | `distance(cm) = pulse_width(µs) / 58` |
| 有效范围 | 2 cm ~ 400 cm |

### TIM3 — LED PWM 调光

| 参数 | 值 |
|------|-----|
| 时钟源 | APB1 (36 MHz)，不分频 |
| 预分频 | 719 → 100 kHz 计数 |
| 自动重装载 | 99 → PWM 频率 = 1 kHz |
| 模式 | PWM Mode 1，高电平有效 |
| 占空比范围 | 0% ~ 100%（CCR 0 ~ 99） |

---

## 📡 蓝牙指令集

通过 HC-05 蓝牙模块（9600 波特率）发送指令：

| 指令 | 功能 |
|------|------|
| `AUTO` | 切换自动调光模式 |
| `MANUAL` | 切换手动模式 |
| `ON` | 手动模式：灯全亮 (100%) |
| `OFF` | 手动模式：关灯 (0%) |
| `DIM:<n>` | 手动模式：设亮度 n% |
| `GET` | 查询状态（模式 / 亮度 / 距离 / ADC） |

> 串口中断接收以 `\n` 为帧结束符，忽略 `\r`。

---

## 🚀 快速开始

### 1. 克隆仓库

```bash
git clone https://github.com/siyexue/Smart-desk-lamp.git
```

### 2. 打开工程

- 使用 **STM32CubeMX** 打开 `Smart desk lamp.ioc` 重新生成代码（可选）
- 使用 **Keil MDK-ARM** 打开 `MDK-ARM/` 下的工程文件

### 3. 编译下载

编译工程 → 连接调试器 → 下载到目标板

---

## 📚 参考资料

- [工程配置速查手册](工程配置速查手册.md) — 完整的时钟树、寄存器参数、功能模块详解
- STM32F103 参考手册
- Keil MDK 文档

---

## 📜 许可证

本项目仅供学习参考。

---

*由 [siyexue](https://github.com/siyexue) 创建并维护*