# 第一次让AI 接手工程

第一次接入工程时，先确认芯片、IDE Target、链接地址、下载算法和构建产物。目录名和历史配置只能作为线索，最终应以芯片型号、IDE 工程和链接文件相互验证。

## 提供工程目录

给AI 的提示词：

> 帮我检查这个工程，确认芯片、Keil Target、下载算法和输出文件。

## AI 检查的工程信息

Keil 工程中需要重点核对以下配置：

```xml
<Device>STM32F103RE</Device>
<TargetName>rt-thread</TargetName>
```

链接后的向量表地址为：

```text
SCB.VTOR = 0x08005000
```

这表示应用位于 `0x08005000`，烧录或分析内存时必须保留前面的 BootLoader 区域。

## 本次识别输出

```text
工程目录：E:\PHDZ\PROJECT\liu\STM32F103_test\STM32F103RC
目标芯片：STM32F103RET6
Keil Device：STM32F103RE
Target：rt-thread
编译器：Arm Compiler 6.24
Flash：512 KiB
SRAM：64 KiB
应用向量表：0x08005000
下载算法：STM32F10x_512.FLM
下载器：MKLink V4，固件 V4.3.5，CDC COM228
```

AI 的判断：

```text
目录名与实际型号不一致，但芯片、Keil Device、Flash 容量和下载算法一致，
本次按 STM32F103RET6 / STM32F103RE 处理。
构建产物使用 build/keil 下同一次生成的 AXF、MAP 和 HEX。
应用从 0x08005000 启动，下载时不能覆盖前置 BootLoader。
```

完成工程识别后，再进入[AI 编译、在线烧录与运行验证](flash-workflow.md)。工程师手动配置见[连接硬件与配置工程](../tools/microlink/project-config.md)。
