# MKLink 产品与功能总览

MKLink（MicroKeen）把在线调试、固件烧录、串口、RTT、变量采样和脱机生产集中到一台设备中。研发阶段可以在 Keil、IAR 或 Web GUI 中使用；进入小批量生产后，同一台下载器可以保存固件和算法，脱离电脑完成烧录；出现运行故障时，还可以读取内存、Fault 寄存器和 RTOS Trace。

![MKLink V4](../images/mklink/products/mklink-lcd-angle.jpg)

关于 AI 接入、变量采集和脱机使用的差异，见 [MKLink 与 J-Link：功能与使用成本](mklink-vs-jlink.md)。

## 支持的目标芯片

MKLink 下载器基于 DAPLink / CMSIS-DAP 体系，面向 Arm Cortex-M 和先楫 HPM 系列单片机。Arm Cortex-M 在线调试使用标准 CMSIS-DAP 接口，在线和脱机烧录使用与目标 Flash 匹配的 CMSIS-Pack / FLM 下载算法；HPM 烧录使用下载器设备端 HPM ROM API，不加载 Cortex-M FLM。更换芯片时，应核对准确器件、板卡与 Flash 配置，不能仅凭系列名称判断当前路径已验证。

| 目标 | 支持范围 | 下载方式 |
|---|---|---|
| Arm Cortex-M | Cortex-M0、M0+、M3、M4、M7、M23、M33、M55、M85 等 | CMSIS-DAP 在线调试；使用匹配的 Pack / FLM 完成在线和脱机烧录 |
| 先楫 HPMicro | HPM 系列，具体以芯片、板卡与 Flash 配置为准 | 使用下载器设备端 HPM ROM API，不加载 FLM |

!!! note "芯片内核支持与 Flash 算法是两件事"
    CMSIS-DAP 负责连接和调试 Arm Cortex-M 内核；烧录片上或外置 Flash 还需要匹配目标器件和存储器的算法。没有内置板型的 HPM 项目需要提供板型。具体配置见[连接硬件与配置工程](getting-started/project-config.md)和[先楫 HPM 生态](hpm/overview.md)。

## 先按任务选择功能

| 要完成的工作 | 使用功能 | 需要准备 | 主要入口 | 详细教程 |
|---|---|---|---|---|
| 编译、烧录并确认新程序运行 | 在线烧录 | 工程或 HEX/BIN | IDE、在线烧录 | [在线编译与烧录](flashing/online-flash.md) |
| 不接电脑进行单台或连续烧录 | 脱机下载 | 固件、量产参数；Arm 需匹配 FLM，HPM 需板卡与 Flash 配置 | 脱机烧录 | [脱机下载与量产](flashing/offline-flash.md) |
| 不占用 MCU 串口查看日志 | RTT View | RTT 控制块 | 仪表盘 / RTT View | [RTT View 日志与终端](observation/rtt.md) |
| 连续观察 PID、FOC 和状态变量 | SuperWatch | 匹配固件的 AXF/ELF | 仪表盘 / SuperWatch | [SuperWatch 与 PID 调试](observation/superwatch.md) |
| 在第三方上位机中显示变量曲线 | VOFA+ | 变量地址、类型、MKLink 虚拟串口 | VOFA+ / JustFloat | [VOFA+ 第三方上位机](observation/vofa.md) |
| 查看 RAM、外设和 Fault 寄存器 | Memory | 地址或 AXF 符号 | 仪表盘 / Memory | [Memory 内存与寄存器](debugging/memory.md) |
| 保存异常现场并定位源码 | HardFault / RISC-V Trap | Fault/Trap 现场、匹配的 ELF | 仪表盘 / Memory / HardFault | [故障现场分析](debugging/hardfault.md) |
| 分析任务、ISR 和 CPU 占用 | RTOS Trace | RTT、SystemView 适配 | 仪表盘 / RTOS Trace | [RTOS Trace / SystemView](observation/systemview.md) |
| 调试 UART、RS485 或 Modbus RTU | 串口与 Modbus | 通信参数或点表 | 仪表盘 | [串口与 Modbus](communication/serial-modbus.md) |
| 更新下载器自身功能 | 固件升级 | 对应型号升级包 | U 盘 / UF2 | [固件升级](flashing/firmware-upgrade.md) |
| 获取软件、固件、FLM 和源码 | 资料下载 | 下载器型号、目标器件 | 官方资料页 | [资料下载](support/downloads.md) |

Web GUI 适合人工配置、操作和查看曲线；AI Skill 适合读取工程、调用同一套硬件能力并整理验证证据。**配套 V4 固件与 0.3.0 上位机 / Skill**通过共享后台共用设备，GUI、CLI、MCP 可以接入同一台下载器；同一设备的工程、符号与采集设置也会共享。传统独占串口工具仍需先释放命令口。

### 配套 V4 开发版本的新工作流

截至 2026-10-06，以下功能已完成配套本地验收，0.3.0 上位机仍在草稿 PR 中，尚非公开正式版本。先核对[版本条件](development/v4-shared-cdc.md#version-check)，再按任务进入；仅安装当前公开版本或仅升级固件，不代表全部具备。

| 想完成的任务 | 操作入口 | 先确认 |
| --- | --- | --- |
| 一边看曲线，一边让 AI 读取同一份数据 | [GUI 与 AI 共享设备](development/v4-shared-cdc.md#shared-device) | 同一下载器共享设置；两台下载器分别选择 |
| 同时查看多个日志或命令终端 | [RTT 0～7 通道](development/v4-shared-cdc.md#rtt-channels) | 目标已初始化所选通道，发送需要对应输入缓冲区 |
| 检查更细的变量变化与采样间隔 | [SuperWatch 采样](development/v4-shared-cdc.md#sampling) | 1 μs 表示请求全速，实际速率以记录为准 |
| 下载、单步后继续看曲线 | [恢复采集](development/v4-shared-cdc.md#dap-restart) | 核对当前固件和符号，再显式开始 |
| 用 HPM5301 的 BIN 或 Intel HEX 烧录 | [HPM 在线与脱机路径](development/v4-shared-cdc.md#hpm-flashing) | 核对板卡、Flash 配置及地址范围 |
| 通过远程 GUI 查看已有工程 | [远程观察范围](development/v4-shared-cdc.md#remote-view) | 先在设备所在电脑加载符号；跨物理主机尚未验证 |

企业还可以在官方 Web GUI 和调试后端上增加自己的变量、RTT 命令、检测流程与报告页面，完整方法见[用 AI 定制企业专属上位机](development/custom-web-gui.md)。

使用嵌入式 AI 操作下载器时，从[嵌入式 AI + MKLink 使用概览](../embedded-ai/overview.md)开始。

## 产品型号

### MKLink V3

![MKLink V3](../images/mklink/products/mklink-v3-front.png)

在在线功能基础上增加独立脱机下载、板载存储、目标电压跟随和按键触发，适合研发与小批量生产共用。

购买链接：[https://item.taobao.com/item.htm?ft=t&id=1013104417098](https://item.taobao.com/item.htm?ft=t&id=1013104417098)

### MKLink V4

![MKLink V3](../images/mklink/products/mklink-lcd-front.png)

V4 增加显示、RS485、功率监测、更大的存储空间和可选择的 Python 脱机脚本，适合研发与小批量生产共用。

购买链接：[https://item.taobao.com/item.htm?ft=t&id=1020501356342](https://item.taobao.com/item.htm?ft=t&id=1020501356342)

| 能力 | V3 | V4 |
|---|:---:|:---:|
| CMSIS-DAP 在线下载和调试 | 支持 | 支持 |
| USB 转 UART | 支持 | 支持 |
| RTT、SystemView、VOFA+ | 支持 | 支持 |
| 下载器内部 Python API | 支持 | 支持 |
| 脱机 HEX/BIN + FLM | 支持 | 支持 |
| 按键触发脱机烧录 | 支持 | 支持 |
| 可选择多个脱机脚本 | - | 支持 |
| RS485、功率显示 | - | 支持 |

!!! note "以设备实际版本为准"
    不同批次固件可能增加能力。升级方法见[固件升级](flashing/firmware-upgrade.md)。

## 一套工具覆盖产品生命周期

### 研发

- 在 Keil、IAR 等 IDE 中使用标准 CMSIS-DAP 下载和单步调试；
- 用在线烧录页检查 HEX/BIN 地址范围并完成擦除、编程、校验和复位；
- 用 RTT 查看日志，用 SuperWatch 观察控制环，用 RTOS Trace 分析调度；
- 发生 HardFault 时先保存寄存器和异常栈，再恢复目标。

### 小批量和量产

- 将固件、下载脚本和对应配置部署到下载器；Arm 使用匹配 FLM，HPM 使用板卡与 Flash 配置；
- 通过按键或机台输入触发；
- 记录固件摘要、脚本版本、器件型号和烧录结果；
- 多镜像工程可以按顺序烧录 BootLoader、参数区和应用程序。

### 售后维护

- 使用 YModem 将升级文件发送给产品 BootLoader；
- 在现场通过 RTT、内存和变量获取运行证据；
- 需要无人值守或异地处理时，再使用受认证的远程 Site Agent。

## 推荐学习路线

第一次使用建议按以下顺序完成：

1. [安装上位机与快速启动](getting-started/gui-install.md)，确认页面显示“后端正常”；
2. [连接硬件与配置工程](getting-started/project-config.md)，加载与固件匹配的 AXF/ELF；
3. [在线编译与烧录](flashing/online-flash.md)，完成校验并检查系统节拍与任务计数；
4. [RTT View](observation/rtt.md) 和 [SuperWatch](observation/superwatch.md)，建立正常运行基线；
5. 根据任务进入 [Memory](debugging/memory.md)、[HardFault / RISC-V Trap](debugging/hardfault.md) 或 [RTOS Trace](observation/systemview.md)；
6. 需要生产工位时再配置[脱机下载](flashing/offline-flash.md)。

本手册的实测案例使用 `STM32F103RET6`、RT-Thread 5.1.0 和 Keil MDK。工程目录保留了历史名称 `STM32F103RC`，但芯片丝印、Keil Device 和下载算法均按 RET6 配置。

先楫案例从 HPM SDK 的 hello_world 开始，展示正弦波、在线下载、RTT 与脱机任务，并附 HPM5301 和 HPM6E80 的实测结果。完整使用说明见 [MKLink × 先楫 HPM](hpm/overview.md)。

## AI 与工程师如何配合

AI 可以读取工程、选择操作入口、采集日志和整理证据。写 Flash、写 RAM、复位或改变控制参数前，仍应由工程师确认目标、范围和停止条件。

简洁的任务描述即可：

> 检查当前工程和芯片，编译并使用MKlink烧录。

完整的安装和安全边界见[嵌入式 AI + MKLink 使用概览](../embedded-ai/overview.md)。

## 购买、支持与资料

- [固件升级](flashing/firmware-upgrade.md)
- [资料下载](support/downloads.md)
- [常见问题](support/faq.md)
- [先楫 HPM 生态](hpm/overview.md)
- [售后服务与嵌入式 AI 交流群](support/after-sales.md)
- [下载器内部 Python API](development/python-api.md)

官方店铺：

- [MKLink V3](https://item.taobao.com/item.htm?ft=t&id=1013104417098)
- [MKLink V4](https://item.taobao.com/item.htm?ft=t&id=1020501356342)

产品讨论和原理文章收录在导航末尾的“扩展阅读”中。先楫 HPM 的工程流程已单独整理为“先楫生态”，需要技术支持或加入交流群可进入“售后服务”。
