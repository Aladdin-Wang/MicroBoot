# AI 调试工作流

嵌入式调试应先选择能直接证明问题的数据，再决定使用哪种观测方式。单次变量值、连续曲线、日志和 RTOS 事件解决的问题不同。

| 问题 | 首选证据 | 教程 |
|---|---|---|
| 启动流程、状态切换、错误日志 | RTT 原始文本 | [采集和分析 RTT](rtt-workflow.md) |
| 某一时刻的变量、外设和 Fault 位 | 符号、RAM、寄存器 | [读取变量、内存和寄存器](memory-workflow.md) |
| PID、FOC、周期性抖动 | 带时间戳的连续采样 | [调试 PID / FOC](pid-workflow.md) |
| Cortex-M 崩溃 | Fault 寄存器、异常栈、AXF | [定位 HardFault](hardfault-workflow.md) |
| 任务饥饿、ISR 过长 | SystemView 事件 | [分析 RTOS Trace](systemview-workflow.md) |

常用提示词不需要包含工具参数，例如：

> 抓 10 秒 RTT，看看告警是从什么时候开始的。

> 连续读目标、反馈和输出，帮我算一下超调和稳定时间。

> 程序进 HardFault 了，先保存现场，再定位到源码。

一次完整处理应包含基线、原始数据、修改、重新构建、下载和相同条件复验。无法复现或采集质量不足时，应明确保留未验证项，不能把推测写成结论。
