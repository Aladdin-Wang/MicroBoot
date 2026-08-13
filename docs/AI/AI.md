# AI + MKLink：让 AI 读到真实硬件

写嵌入式程序时，AI 最容易遇到的障碍不是不会写 C，而是看不到板子上的真实状态。代码说 DMA 应该工作，AI 却不知道寄存器是否真的打开；日志说任务在运行，AI 却不知道任务是否已经饿死；HardFault 发生后，AI 甚至不知道 PC 停在了哪一行。

MKLink 把下载、运行观测和故障现场交给 AI 使用。工程师给出工程目录和目标，AI 读取 AXF/ELF、调用编译器和下载器，再把变量、RAM、寄存器、RTT 和 RTOS Trace 带回到代码分析中。

```text
工程源码 -> 构建产物 -> 下载器 -> MCU 运行现场
    ^                              |
    +-------- AI 根据真实数据修改代码
```

## 先看一篇完整实战

[STM32F103RET6 + RT-Thread 全功能实战](stm32f103-case.md)是一篇完整案例：从 `main.c` 增加构建标识开始，经过 Keil 构建、在线下载、脱机触发、RTT 日志、变量读取、PID 采样、HardFault 定位和 SystemView 分析，最后恢复安全固件。

下面的文章把其中每项能力单独展开。每篇都包含可以直接复制的关键代码、简短提示词和本次测试中 AI 的真实关键输出。

## 你需要给 AI 什么

一次正常的输入不需要写成命令手册，像和工程师同事说话即可：

> 看一下这个 STM32F103RET6 工程，先编译，再烧录，最后读 RTT 看新固件有没有跑起来。

AI 会从工程文件中寻找 Keil Target、芯片和输出目录。第一次使用时，建议先让它报告识别结果，再允许写入目标板。

## 按问题选择文章

| 你遇到的问题 | 文章 |
|---|---|
| AI 不知道工程和芯片是否匹配 | [第一次让AI 接手工程](first-project.md) |
| 修改后不知道是否真的烧进去了 | [AI 编译、在线烧录与运行验证](flash-workflow.md) |
| 想把验证过的固件放进下载器量产 | [AI 部署和触发脱机下载](offline-workflow.md) |
| 想看启动日志和状态变化 | [AI 采集和分析 RTT](rtt-workflow.md) |
| 想确认变量、地址和寄存器 | [AI 读取变量、内存和寄存器](memory-workflow.md) |
| 想调 PID 或 FOC 控制环 | [AI 使用 SuperWatch 调试 PID / FOC](pid-workflow.md) |
| 程序进入 HardFault | [AI 定位 HardFault](hardfault-workflow.md) |
| 怀疑任务调度或 ISR 占用过高 | [AI 分析 RTOS Trace / SystemView](systemview-workflow.md) |

AI 教程中的对应人工操作，可从 [MKLink 产品与功能总览](../tools/microlink/microlink.md) 进入。
