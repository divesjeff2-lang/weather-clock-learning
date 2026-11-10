# STM32F407 学习笔记：从点灯到 FreeRTOS 天气时钟

> 这是我学习 STM32 与 FreeRTOS 的个人学习/研究仓库。从 GPIO、定时器、串口、I2C、SPI、LCD 这些基础外设开始，一步步做到裸机天气时钟，再改造成 FreeRTOS 多任务版本，最后加上 Bootloader 串口升级。每个工程都在板子上编译、下载、调试过。
>
> 代码是边学边写的练习代码，驱动和工程结构参考了学习资料中的示例，在此基础上自己动手实现、调试和整理。原硬件已于 2026 年中损坏，暂无实物运行图。

---

## 1. 学习路线

| 章节 | 目录 | 内容 | 关键知识点 |
|---|---|---|---|
| 01 | `01_LED` | 点灯、面向对象封装 LED、软件 PWM 呼吸灯 | GPIO、RCC 时钟、驱动描述表 |
| 02 | `02_SysTick_Delay` | 用 SysTick 实现微秒/毫秒延时 | SysTick 寄存器、COUNTFLAG |
| 03 | `03_USART` | 串口轮询发送 | USART 配置、引脚复用 |
| 04 | `04_Key_Interrupt` | 按键扫描、EXTI 外部中断、SysTick 中断、串口接收中断控制 LED | NVIC 优先级分组、EXTI、中断服务函数 |
| 05 | `05_Timer` | TIM6 定时中断、PWM 输出、输入捕获测频率/占空比 | 时基、预分频、输出比较、输入捕获 |
| 06 | `06_I2C` | BL24C512 EEPROM 读写（存 SN、开机计数）、AHT20 温湿度 | 硬件 I2C 事件检查、超时处理 |
| 07 | `07_SPI` | SPI2 主机配置 | SPI 模式（CPOL/CPHA）、分频 |
| 08 | `08_LCD` | ST7789 屏幕：纯色、ASCII、中文、图片 | 屏幕初始化序列、窗口设置、字模/图片取模 |
| 09 | `09_ESP_AT` | ESP32-C3 AT 指令联网，HTTP 获取天气 | AT 指令收发与解析、JSON 字段提取 |
| 10 | `10_WeatherClock_BareMetal` | **裸机版天气时钟**（主循环 + 时间片轮询） | 页面拆分、多外设整合 |
| 11 | `11_Bootloader_XBIN` | **FreeRTOS 版天气时钟** + Bootloader + 上位机串口升级 | 多任务、队列、信号量、DMA、IAP 升级 |
| 12 | `12_Bootloader_Endian` | Bootloader 协议编解码改用小端读写宏 | 字节序、`bitops.h` |
| 13 | `13_Bootloader_Log_Optimize` | Bootloader 接入 EasyLogger 日志，对比编译优化等级 | 日志分级、`-O0` ~ `-O3` 代码体积与性能 |

<details>
<summary>各章节子工程说明（点开）</summary>

| 子工程 | 说明 |
|---|---|
| `01_LED/LED` | 库函数直接操作 PE9 |
| `01_LED/LED_Object` | 用描述表 + `led_init/led_on/led_off` 封装三颗 LED |
| `01_LED/LED_Breath` | 单颗 LED 软件 PWM 呼吸灯 |
| `01_LED/LED_Wave` | 三颗 LED 不同占空比渐变 |
| `04_Key_Interrupt/KEY` | 轮询按键，串口打印 |
| `04_Key_Interrupt/interrupt` | PC4/PC5 EXTI 下降沿中断翻转 LED |
| `04_Key_Interrupt/Interrupt-SysTick` | SysTick 1 ms 中断 |
| `04_Key_Interrupt/Interrupt_All` | 串口接收中断：收到 `1/2/3` 翻转对应 LED |
| `04_Key_Interrupt/USART_int` | 空模板（未完成） |
| `05_Timer/STM32F4_TIM6` | TIM6 1 kHz 更新中断，500 ms 翻转 LED |
| `05_Timer/STM32F4_PWM` | TIM1/TIM3 PWM 驱动 LED 亮度 |
| `05_Timer/STM32F4_Inputc` | TIM3 输入捕获测量 PWM 频率与占空比 |
| `06_I2C/STM32F4_EEPROM`、`STM32F4_EEPROM_v1` | BL24C512：写默认 SN、记录开机次数（v1 为第一次写的版本） |
| `06_I2C/STM32F4_AHT20` | AHT20 温湿度读取（完整版） |
| `06_I2C/STM32F4_aht20_2` | AHT20 驱动重写练习（未完成） |
| `08_LCD/STM32f4_conclusion_LCD` | 不分文件、把 ST7789 驱动全写在 `main.c` 里的小结练习 |
| `08_LCD/STM32F4_ColorBlock` | 全屏红绿蓝填充 |
| `11_Bootloader_XBIN/STM32F4_Bootloader` | Bootloader 工程 |
| `11_Bootloader_XBIN/WeatherClock_FreeRTOS` | FreeRTOS 天气时钟（作为 APP 运行在 Bootloader 之后） |
| `11_Bootloader_XBIN/SerialUpgrader` | C# WinForms 串口升级上位机（.NET 8） |

</details>

## 2. 项目：FreeRTOS 天气时钟

基于 **STM32F407 + ESP32-C3** 的桌面天气时钟：STM32 运行 FreeRTOS 负责界面与调度，ESP32-C3 运行官方 ESP-AT 固件负责联网。代码在 `11_Bootloader_XBIN/WeatherClock_FreeRTOS`，裸机版在 `10_WeatherClock_BareMetal`。

**功能**

- 联网后通过 SNTP 自动校时，RTC（LSE 32.768 kHz）走时
- 通过 HTTPS 获取实时天气（心知天气 API）
- AHT20 采集室内温湿度
- 240×320 ST7789 彩屏，四个页面：欢迎页、Wi-Fi 连接页、主界面、错误页
- Wi-Fi 断线检测与界面提示

### 2.1 规格参数

| 项目 | 参数 |
|---|---|
| 主控 | STM32F407（Cortex-M4F，168 MHz，HSE 8 MHz） |
| 操作系统 | FreeRTOS V10.4.3 LTS，1 kHz 节拍，heap_4，堆 92 KB |
| 网络模块 | ESP32-C3（ESP-AT 固件），UART 115200 |
| 屏幕 | ST7789，240×320，RGB565，SPI2 10.5 MHz + DMA |
| 温湿度传感器 | AHT20，硬件 I2C 100 kHz |
| 开发环境 | Keil MDK5 + **ARM Compiler 5**，STM32F4xx 标准外设库 V1.9.0 |

### 2.2 硬件连接

| 外设 | STM32 外设 | 引脚 |
|---|---|---|
| ESP32-C3（AT） | USART2，发送用 DMA1_Stream6，接收用 RXNE 中断 | PA2 (TX) / PA3 (RX) |
| 调试串口 | USART1 115200，发送用 DMA2_Stream7 | PA9 (TX) / PA10 (RX) |
| ST7789 屏幕 | SPI2 + DMA1_Stream4 | PB13 (SCK) / PC3 (MOSI)；PE2 (CS) / PE3 (RST) / PE4 (DC) / PE5 (BL) |
| AHT20 | I2C2 | PB10 (SCL) / PB11 (SDA) |
| Bootloader 升级口 | USART3 115200（仅 Bootloader 使用） | PB10 (TX) / PB11 (RX) |
| 烧录 | SWD | SWDIO / SWCLK / GND / 3V3 |

### 2.3 软件架构

```
 init 任务 (优先级 9)  ── 初始化外设 → 欢迎页 → 连 Wi-Fi → 主界面 → 启动软件定时器 → 删除自己

 FreeRTOS 软件定时器（定时器服务任务，优先级 9）
   ├─ time update   1 s  ──────────── 直接在定时器回调里读 RTC
   └─ time sync / wifi / inner / outdoor ─┐
                                          │ workqueue_run()（队列 16 条）
                                          ▼
                    workqueue 任务（优先级 5）── SNTP、HTTPS、读 AHT20 等慢操作
                                          │
                     ui_fill_color / ui_write_string / ui_draw_image
                                          │ ui_queue（16 条消息）
                                          ▼
                    ui 任务（优先级 8）── 唯一操作屏幕的任务
```

| 设计点 | 做法 | 解决的问题 |
|---|---|---|
| UI 任务独占屏幕 | 其他任务只往 `ui_queue` 发绘制消息；字符串先 `pvPortMalloc` 复制一份，由 ui 任务画完后释放 | 多任务同时操作 SPI 屏幕导致花屏、资源竞争；避免栈上字符串失效 |
| 前后台分离 | 软件定时器只负责"到点"，耗时操作投递给 workqueue 任务执行 | 联网等慢操作不阻塞定时器服务任务和界面刷新 |
| AT 通信不阻塞 CPU | 发送走 DMA；接收在中断里收集，收到 `OK` / `ERROR` / `busy` / `ready` 等应答后用二值信号量通知，等待带超时 | CPU 不用忙等串口 |
| 屏幕批量传输 | 大块像素用 SPI DMA 发送，二值信号量等待传输完成 | 减少 CPU 占用 |

**定时任务**（`app/app.c`）

| 定时器 | 周期 | 执行位置 | 作用 |
|---|---|---|---|
| time update | 1 s | 定时器回调 | 读 RTC，时间变化时刷新显示 |
| inner update | 3 s | workqueue | 读取 AHT20 温湿度 |
| wifi update | 5 s | workqueue | 检查 Wi-Fi 连接状态 |
| outdoor update | 1 min | workqueue | 获取网络天气 |
| time sync | 启动后立即执行，之后每 1 h；失败 1 s 后重试 | workqueue | SNTP 校时写入 RTC |

**刷新率估算**：SPI 时钟 = APB1 42 MHz / 4 = 10.5 MHz，一整屏 240×320×16 bit ≈ 1.23 Mbit，理论上全屏刷新最多约 8 帧/秒。所以界面只做局部刷新：时间、温度等数字变化时才重画对应区域。

### 2.4 Bootloader 与串口升级

| 区域 | 地址 | 说明 |
|---|---|---|
| Bootloader | `0x08000000` | 上电先运行；按住 KEY2 复位可停留在 Bootloader |
| Magic Header | `0x0800C000` | 记录 APP 地址、长度、CRC32、版本号 |
| APP（天气时钟） | `0x08010000` | `SCB->VTOR = 0x08010000` |

- 升级协议：`header(1) + opcode(1) + length(2) + payload + crc16(2)`，指令有查询、擦除、写入（单包最多 4 KB）、校验（CRC32）、复位、启动。
- 编译天气时钟后，Keil 的 After Build 会依次执行 `fromelf` 生成 `.bin`、`scripts/gen_magic_header.py` 生成带 4 KB 头的 `generated/*_upgrade.xbin`，再用 `SerialUpgrader` 通过串口下发。
- Bootloader 启动 APP 前会校验 CRC32，不通过就停在 Bootloader。

## 3. 快速开始

1. 用 USB 转串口给 ESP32-C3 烧录官方 **ESP-AT** 固件。
2. 修改配置（仓库里都是占位符）：
   - `11_Bootloader_XBIN/WeatherClock_FreeRTOS/app/wifi.h`：`WIFI_SSID`、`WIFI_PASSWD`
   - `11_Bootloader_XBIN/WeatherClock_FreeRTOS/app/app.c`：把天气 URL 里的 `YOUR_SENIVERSE_KEY` 换成自己的[心知天气](https://www.seniverse.com/) key
3. 安装 Keil MDK5 和 **ARM Compiler 5**（新版 MDK 默认只带 AC6，FreeRTOS 用的是 RVDS 移植层，需要 AC5）。
4. 先编译并下载 `11_Bootloader_XBIN/STM32F4_Bootloader`（地址 `0x08000000`）。
5. 再编译并下载 `11_Bootloader_XBIN/WeatherClock_FreeRTOS`（地址 `0x08010000`）。After Build 需要本机有 `python`。
6. 复位运行；调试串口（USART1，115200）可以看到运行日志。

> 不想用 Bootloader 的话，可以直接跑 `10_WeatherClock_BareMetal`（从 `0x08000000` 启动，配置在 `app/app.h`）。
> 其它练习工程都是独立的 Keil 工程，打开对应目录下的 `mdk/stm32f407.uvprojx` 即可。

## 4. 常见问题

| 现象 | 可能原因 | 处理方法 |
|---|---|---|
| 编译报 `__asm` / port.c 相关错误 | 用了 ARM Compiler 6 | 在 Options → Target 里切换到 ARM Compiler 5 |
| 下载天气时钟后不运行 | 没有先烧 Bootloader，或 Magic Header 未写入导致 CRC 校验失败 | 先烧 Bootloader，再通过 SerialUpgrader 下发 `.xbin` |
| 一直停在 Wi-Fi 连接页 | SSID/密码错误；手机热点是 5 GHz | 检查 `wifi.h`；ESP32-C3 只支持 2.4 GHz |
| 时间不对或不更新 | SNTP 未同步成功 | 看调试串口 `[SNTP]` 日志；确认已联网 |
| 天气显示为空 | API key 无效或请求超时 | 换自己的 key；看 `[WEATHER]` 日志 |
| AT 指令无响应 | ESP32-C3 不是 AT 固件，或 TX/RX 接反 | 重新烧录 ESP-AT；交换 PA2/PA3 接线 |
| 屏幕花屏 | 多处直接操作屏幕 | 所有绘制都通过 `ui_*` 接口交给 ui 任务 |

## 5. 我的收获

- 理解了为什么要让一个任务独占屏幕，以及用队列在任务之间传消息
- 会用 DMA + 中断 + 信号量写不阻塞 CPU 的串口和 SPI 驱动
- 能看懂 ESP-AT 指令流程，并通过调试串口日志定位联网问题
- 理解了 Bootloader + APP 的 Flash 分区、中断向量表重定位和固件校验
- 对比裸机主循环和 FreeRTOS 两个版本，体会到前后台分离带来的好处

## 6. 第三方组件

| 组件 | 用途 | 许可 |
|---|---|---|
| [STM32F4xx 标准外设库 V1.9.0](https://www.st.com/) | 外设驱动 | ST 许可（见各文件头） |
| [FreeRTOS](https://www.freertos.org/) V10.4.3 LTS | 实时操作系统 | MIT |
| [EasyLogger](https://github.com/armink/EasyLogger) | 日志（第 13 章） | MIT |
| CRC 计算（Wang Yaofu） | Bootloader 校验 | 见文件头 |
| [ESP-AT](https://github.com/espressif/esp-at) | ESP32-C3 联网固件 | Apache-2.0 |
| [Maple Mono](https://github.com/subframe7536/maple-font) | 屏幕字模 | OFL-1.1 |
| [心知天气 API](https://www.seniverse.com/) | 天气数据 | 需自行申请 key |
