# RTOS Trace / SystemView

SystemView 用事件时间线显示任务切换、ISR 进出和 CPU 占用，适合定位优先级反转、任务饥饿、ISR 过长和调度抖动。MKLink Web GUI 内置采集、解码和分析，不依赖 J-Link 或 SEGGER PC 软件。

本页使用“RTOS Trace”作为用户入口，“SystemView”是目标端事件协议和集成组件的名称。

## 数据链路

```text
RT-Thread hooks -> SEGGER_SYSVIEW -> RTT 通道 1
                -> MKLink -> Web GUI 时间轴与统计
```

目标工程必须先集成 RTT。SystemView 使用 RTT 上行通道 1，普通日志通常使用通道 0。

## RT-Thread 集成

### Env 包工程

在 `menuconfig` 中：

1. `RT-Thread online packages → tools packages → SystemView`；
2. 选择最新兼容版本；
3. 在内核配置中启用 hook list；
4. 将 SystemView RTT 上行缓冲调大，例如 16 KiB；
5. 重新生成工程、编译并烧录。

### Keil 手工工程

完整安装 MKLink AI Probe 后，可以让 AI 或 CLI 集成：

```powershell
python -m mklink rtt-integrate --project-root .
python -m mklink systemview-integrate --project-root .
```

执行前应提交或备份工程。集成会增加 SEGGER 源文件、IncludePath 和条件宏；发布固件可通过关闭对应宏移除 Trace。

## Web GUI 使用

1. 加载匹配的 AXF/ELF；
2. 连接目标并确认程序正在运行；
3. 打开“仪表盘 → RTOS Trace”；
4. 点击“自动搜索”；
5. 开始采集，运行需要观察的业务场景；
6. 停止后检查任务 CPU 占用、切换率、ISR 和异常提示。

![RTOS Trace 实测时间线和任务统计](../../images/microlink/gui/systemview.png)

## STM32F103 RT-Thread 示例

示例工程已启用 hook list、SystemView 和 16 KiB RTT Up Buffer。采集前仍需确认当前 Keil/IAR Target 和实际芯片一致，并加载与已烧录固件匹配的 ELF。

### 有效采集的硬性条件

SystemView 页面出现时间线并不等于数据有效。至少同时满足以下条件才可以分析任务占用：

- 任务数大于 0，且任务名能由当前 ELF 解析；
- 时间单位和观测时长为正常量级，而不是微秒级异常值；
- 页面没有 `Trace overflow` 或持续丢包；
- 事件数量与采样时长、任务周期相符；
- 重新复位后能重复得到相近结果。

若任务数为 0、时间基准异常或缓冲溢出，报告中只能记录“采集无效”，不能据此给出 CPU 占用、切换率或 ISR 结论。先检查 `SEGGER_SYSVIEW_RTT_BUFFER_SIZE` 是否至少为 16 KiB，再降低任务事件频率并从复位后的早期阶段重新采集。

!!! note "本案例的有效采集"
    STM32F103RET6 演示工程本次采集约 6.03 秒，识别 9 个任务、72 MHz 时钟，目标端无 `Trace overflow`。GUI 顶部的 `Runtime Drop` 是前端显示队列丢弃计数；它不等于目标缓冲溢出。需要审计时导出 JSONL 或摘要，并检查目标端 overflow 字段。

## 如何读结果

| 指标 | 关注点 |
|---|---|
| 每任务 CPU 占用 | 单任务长期占满、空闲任务异常降低 |
| 上下文切换率 | 过高可能意味着时间片或同步设计不合理 |
| ISR 时长与占用 | 过长会增加响应延迟和丢中断风险 |
| 时间轴 | 优先级反转、长临界区、任务饥饿 |
| 目标 overflow | 缓冲过小、事件率过高或读取不及时；必须停止并重采 |
| GUI Runtime Drop | 前端显示队列已丢弃旧事件；不据此推断目标端丢包，导出完整数据复核 |

阈值只能用于筛查，不能脱离业务实时性要求直接判定故障。

## 常见问题

| 现象 | 排查 |
|---|---|
| 未找到 RTT 控制块 | 检查 ELF/MAP、RTT 初始化、RAM 边界和自动搜索范围 |
| 启动后无事件 | 检查 hook list、SystemView 宏、通道 1和目标运行状态 |
| 任务名只有地址 | 加载 ELF，并确认任务控制块地址和 RAM base 正确 |
| 丢包持续增加 | 增大 RTT 缓冲、缩短采集链路延迟或减少事件 |

## 使用 AI 协助

> 采集 6 秒 RTOS Trace，先报告任务数、时间基准和目标 overflow，再分析 CPU/ISR。

无效采集只能用于排障，不能让 AI 根据异常时间或零任务数据推断调度结论。
