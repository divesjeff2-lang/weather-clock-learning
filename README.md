<div align="center">

# 🌦️ STM32F407 Smart Weather Clock

### 基于 FreeRTOS 的 STM32F407 智能天气时钟系统

<br>

<img src="./Doc/banner.png" width="100%"/>

<br>

![MCU](https://img.shields.io/badge/MCU-STM32F407-1679A7?style=for-the-badge)
![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-00C853?style=for-the-badge)
![Language](https://img.shields.io/badge/C-Programming-FF6D00?style=for-the-badge)
![IDE](https://img.shields.io/badge/IDE-Keil5-FF1744?style=for-the-badge)
![Version](https://img.shields.io/badge/Version-v1.0-success?style=for-the-badge)

<br>

### 🚀 一个工程化、多任务、可扩展的嵌入式智能天气时钟项目

支持：

🕒 实时时钟 | 🌡️ 温湿度采集 | ☁️ 网络天气 | 📶 WiFi联网  
🎨 图形化UI | 🔔 多任务调度 | 📦 日志系统 | ⚡ 工程化设计  

</div>

---

# 📸 项目展示

## 主界面

<div align="center">
<img src="./Doc/main_ui.gif" width="900"/>
</div>

---

## 菜单动画

<div align="center">
<img src="./Doc/menu.gif" width="900"/>
</div>

---

## 硬件实物

<div align="center">
<img src="./Doc/hardware.jpg" width="900"/>
</div>

---

# ✨ 项目亮点

<table>
<tr>
<td width="50%">

## 🎯 多任务实时系统

- FreeRTOS任务调度
- Queue消息通信
- Mutex互斥锁
- EventGroup事件同步

</td>

<td width="50%">

## 🎨 图形化界面

- 图标显示
- 局部刷新
- 动画切换
- 菜单交互

</td>
</tr>

<tr>
<td>

## 📡 网络通信

- ESP8266联网
- HTTP请求
- JSON解析
- 天气API

</td>

<td>

## 🛠️ 工程化开发

- 日志系统
- 错误检测
- 模块解耦
- 易于扩展

</td>
</tr>
</table>

---

# 🏗️ 系统架构

<div align="center">
<img src="./Doc/architecture.png" width="1000"/>
</div>

```text
Application Layer
│
├── UI Task
├── Sensor Task
├── Weather Task
├── Key Task
├── UART Task
└── Log Task

──────────────────────────

FreeRTOS Kernel

──────────────────────────

Driver Layer
│
├── LCD
├── AHT20
├── ESP8266
├── RTC
├── USART
├── SPI
└── IIC

──────────────────────────

Hardware Layer
│
└── STM32F407
```

---

# ⚙️ 功能模块

## 🕒 时钟系统

- RTC时间同步
- 日期星期显示
- 自动校准

---

## 🌡️ 环境监测

- AHT20采集
- 温度显示
- 湿度显示

---

## ☁️ 天气系统

- 城市天气
- 风力等级
- 空气湿度
- 天气图标

---

## 📶 WiFi系统

- 自动联网
- 断线重连
- 网络状态显示

---

## 📋 日志系统

- 系统启动日志
- 错误日志
- 串口调试输出

---

# 📂 项目结构

```bash
STM32F407_WeatherClock
│
├── Core
│   ├── main.c
│   ├── stm32f4xx_it.c
│
├── Hardware
│   ├── LED
│   ├── KEY
│   ├── USART
│   ├── LCD
│   ├── SPI
│   ├── IIC
│   ├── TIM
│   ├── ESP8266
│   └── AHT20
│
├── FreeRTOS
│   ├── Task
│   ├── Queue
│   ├── Semaphore
│
├── UI
│
├── Doc
│
└── README.md
```

---

# 🧠 FreeRTOS任务设计

| Task | Priority | Stack | Description |
|--|--|--|--|
| StartTask | 5 | 256 | 系统启动 |
| DisplayTask | 4 | 512 | UI刷新 |
| WeatherTask | 4 | 512 | 网络天气 |
| SensorTask | 3 | 256 | 温湿度采集 |
| KeyTask | 2 | 128 | 按键扫描 |
| UartTask | 2 | 256 | 串口通信 |
| LogTask | 1 | 256 | 日志记录 |

---

# 📈 系统性能

| 项目 | 数据 |
|--|--|
| MCU主频 | 168MHz |
| UI刷新率 | 30FPS |
| 天气更新时间 | 10min |
| 温湿度采样 | 1s |
| CPU占用 | < 35% |
| RAM占用 | < 40% |

---

# 🔌 硬件连接

## AHT20

| STM32 | Sensor |
|--|--|
| PB6 | SCL |
| PB7 | SDA |

## ESP8266

| STM32 | Module |
|--|--|
| PA9 | TX |
| PA10 | RX |

## LCD

SPI接口

---

# 🚀 快速开始

## 1. 克隆项目

```bash
git clone your_repo_url
```

## 2. 打开工程

```bash
Keil5
```

## 3. 编译下载

```bash
Build → Download
```

## 4. 运行

```bash
Power On
```

---

# 🛣️ 开发路线

- [x] 底层驱动
- [x] FreeRTOS移植
- [x] UI开发
- [x] 网络通信
- [x] 日志系统
- [ ] OTA升级
- [ ] MQTT接入
- [ ] LVGL重构
- [ ] APP互联

---

# 🎥 演示视频

（预留视频二维码或 Bilibili 链接）

---

# ⭐ 如果项目对你有帮助

<div align="center">

## 给个 Star 支持一下 ⭐

</div>

---

# 👨‍💻 Author

### Jeff Dives

Embedded Developer
