# AI 分析 RTOS Trace：任务、ISR 与 CPU 占用

SystemView 用事件时间线记录任务切换和 ISR 进出，适合定位任务饥饿、ISR 过长和调度抖动。分析百分比之前，必须先确认任务数、时间基准、overflow 和传输丢包正常。

## 工程准备

本例启用了 RT-Thread hook、SEGGER SystemView 和 RTT Channel 1，并创建 `pidloop`、`svfast`、`rttrx` 等任务。普通 RTT 日志使用 Channel 0，SystemView 使用 Channel 1。

其中 `svfast` 是一个每 20 ms 运行一次的演示任务：

```c
static void sysview_fast_entry(void *parameter)
{
    RT_UNUSED(parameter);
    while (1) {
        sysview_spin(400, 0);
        rt_thread_mdelay(20);
    }
}
```

工程还包含 75 ms、250 ms 和 1000 ms 周期的任务，用来形成可区分的调度事件。

## 开始采集

给AI 的提示词：

> 采 6 秒 RTOS Trace，检查有没有丢数据，再列任务占用和最长 ISR。

AI 的采集输出：

```text
duration=6.026 s
events=20957
tasks=9
context_switches=3440 (570.9/s)
ISR=6026 (1.61%)
longest_ISR=225.1 us
```

任务统计：

```text
tidle0  96.72%
main     0.87%
pidloop  0.80%
svfast   0.56%
rttrx    0.37%
```

## 复核自动告警

分析器同时输出：

```text
[cpu_starvation] tidle0 occupies 96.72% CPU
```

`tidle0` 是 RT-Thread 空闲任务。它占用 96.72% 表示 CPU 大部分时间没有业务负载，不代表业务任务饥饿，因此该告警属于语义误报。最长 ISR 225.1 us 是否可接受，需要与产品控制周期和实时性预算比较。

## 分析结论

```text
采集有效：识别到 9 个任务，时间基准正常，目标无 overflow。
CPU 主要处于空闲态，没有业务任务饥饿的证据。
tidle0 的 starvation 告警为误报。
最长 ISR=225.1 us，需要结合系统实时性要求继续评估。
```

人工时间线操作见[RTOS Trace / SystemView](../tools/microlink/SystemView.md)。
