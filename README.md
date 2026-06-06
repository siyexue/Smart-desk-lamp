# Smart Desk Lamp 🪄💡

**智能台灯** — 基于 STM32 的智能照明控制系统

## 📖 项目简介

本项目基于 STM32 微控制器，实现一款具备 **智能调光、环境感知、多种照明模式** 的桌面台灯。适用于学习、办公、阅读等场景。

### 主要功能

| 功能 | 描述 |
|------|------|
| 🌞 环境光检测 | 自动感知周围亮度，智能调节灯光强度 |
| 🎚️ 多级调光 | 支持多档亮度手动调节 |
| 📐 PWM 控制 | 使用 PWM 技术实现无级调光 |
| 🕹️ 按键交互 | 物理按键控制开关及模式切换 |
| 📱 扩展接口 | 预留传感器及通信接口，便于功能扩展 |

## 🔧 硬件平台

| 组件 | 型号/说明 |
|------|-----------|
| 🧠 主控芯片 | STM32（具体型号见 `.ioc` 配置文件） |
| 💡 光源 | LED 灯珠 + PWM 驱动电路 |
| 🌡️ 传感器 | 环境光传感器 |
| 🔘 输入 | 物理按键 |
| ⚡ 电源 | USB / 直流供电 |
| 🛠️ 开发板 | 自研 PCB / 开发板 |

## 💻 开发环境

| 工具 | 版本 / 说明 |
|------|-------------|
| IDE | Keil MDK-ARM |
| 芯片配置 | STM32CubeMX (`.ioc` 文件) |
| 编译器 | ARMCC / GCC |
| 调试器 | J-Link / ST-Link |
| 版本管理 | Git + GitHub |

## 📁 项目结构

```
Smart-desk-lamp/
├── Core/                    # 核心代码
│   ├── Inc/                 # 头文件
│   └── Src/                 # 源文件
├── Drivers/                 # 外设驱动库 (HAL/LL)
├── Board/                   # 板级支持包
├── MDK-ARM/                 # Keil 工程文件
├── .mxproject               # CubeMX 配置文件
├── Smart desk lamp.ioc      # CubeMX 引脚配置
├── 工程配置速查手册.md       # 开发配置文档
├── .gitignore               # Git 忽略规则
└── README.md                # 本文件
```

## 🚀 快速开始

### 1️⃣ 克隆仓库

```bash
git clone https://github.com/siyexue/Smart-desk-lamp.git
```

### 2️⃣ 打开工程

- 使用 **STM32CubeMX** 打开 `Smart desk lamp.ioc` 重新生成代码（可选）
- 使用 **Keil MDK-ARM** 打开 `MDK-ARM/` 下的工程文件

### 3️⃣ 编译下载

- 编译工程 → 连接调试器 → 下载到目标板

## 📚 参考资料

- [工程配置速查手册](./工程配置速查手册.md)
- [STM32 参考手册](https://www.st.com/)
- [Keil MDK 文档](https://www.keil.com/)

## 📜 许可证

本项目仅供学习参考。

---

_✍️ 由 [siyexue](https://github.com/siyexue) 创建并维护_
