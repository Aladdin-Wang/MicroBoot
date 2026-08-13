# STM32F103RET6 + RT-Thread 全功能实战

本章记录AI 对真实 STM32F103RET6 工程完成识别、修改、构建、在线与脱机下载、RTT、变量、PID、HardFault 和 SystemView 的全过程。所有地址、摘要和测量结果都来自 2026-08-13 的同一次真机演示；重新构建后应以新产物为准。

工程师希望手动复现某项功能时，可从 [MKLink 产品与功能总览](../tools/microlink/microlink.md) 进入对应 GUI 教程。

## 1. 任务与硬件

给 AI 的起始提示词：

> 用这个 STM32F103RET6 工程做一次完整调试，编译、烧录并把各项运行数据验证一遍。

识别结果：

| 项目 | 实测值 |
|---|---|
| 芯片 | STM32F103RET6，Cortex-M3 |
| Flash / SRAM | 512 KiB / 64 KiB |
| Keil Device / Target | `STM32F103RE` / `rt-thread` |
| RTOS | RT-Thread 5.1.0 |
| 编译器 | Arm Compiler 6.24 |
| 下载算法 | `STM32F10x_512.FLM` |
| 应用 VTOR | `0x08005000` |
| MKLink | V4，固件 V4.3.5，CDC `COM228` |
| RTT 控制块 | `0x20000B18` |

目录名中的 `STM32F103RC` 是历史名称。AI 以芯片、Keil Device、链接布局和算法容量为依据，没有按目录名选择错误型号。

## 2. 演示固件与构建标识

程序包含周期变化的温度、转速和告警变量，一个不驱动真实电机的 PID 一阶模型，RTT 日志、SystemView 任务，以及受魔数保护的 HardFault 演示入口。为了区分旧固件，AI 增加：

```c
volatile rt_uint32_t g_mklink_demo_build_id = 20260813U;

rt_kprintf("MKLink AI demo | build=%lu | target=STM32F103RET6 | rtos=RT-Thread 5.1.0\r\n",
           (unsigned long)g_mklink_demo_build_id);
```

## 3. 构建和在线下载

提示词：

> 用 Keil 编译并烧录，完成后读 build 和 RTT 验证。

构建输出：

```text
0 Error(s), 0 Warning(s)
Code=94694  RO-data=20462  RW-data=2504  ZI-data=35392
```

产物与摘要：

```text
build/keil/Obj/rt-thread.axf
build/keil/Obj/rt-thread.hex
build/keil/List/rt-thread.map
HEX  FD86D173977506E46C7CDEC2BD9C648940562B6FD86059433BEBC6FFECEF051E
AXF  ADCB3D772269C5B0D2CCD4777CBA6672E08328290539B9587529E540B96BEA98
```

下载和运行验证：

```text
Erase Done.
Programming Done.
Verify OK.
Application running ...
g_mklink_demo_build_id @ 0x200003C8 = 20260813
```

AI 的关键输出：

```text
构建：0 error / 0 warning。
下载：erase、program、verify、run 全部完成。
验证：RAM 中 build_id=20260813，确认目标运行本次固件。
```

详细规则见[AI 编译、在线烧录与运行验证](flash-workflow.md)，人工烧录见[在线编译与烧录](../tools/microlink/online-flash.md)。

## 4. RTT 验证运行逻辑

提示词：

> 抓 10 秒 RTT，检查版本、uptime 和温度告警。

脱机下载完成后的完整启动片段：

```text
RT-Thread 5.1.0 build Aug 13 2026 16:56:20
RTT Control Block Detection Address is 0x20000b18
MKLink AI demo | build=20260813 | target=STM32F103RET6 | rtos=RT-Thread 5.1.0
MKLink demo | uptime=5002 ms | temp=30.0 C | speed=1050 rpm | state=0 | alarm=1
MKLink demo | uptime=10038 ms | temp=29.0 C | speed=1000 rpm | state=2 | alarm=0
```

AI 的输出：

```text
运行的是 build 20260813，uptime 连续递增。
30.0 C 时 alarm=1，回落到 29.0 C 时 alarm=0，阈值逻辑符合代码。
```

完整方法见[AI 采集 RTT](rtt-workflow.md)，手动终端见[RTT 终端与曲线](../tools/microlink/RTT_printf.md)。

## 5. 变量、RAM 和寄存器

提示词：

> 读 build、温度、告警和 PID 变量，再核对附近 RAM 以及 VTOR、Fault 寄存器。

关键符号地址：

```text
g_mklink_demo_build_id  @ 0x200003C8 = 20260813
g_demo_uptime_ms        @ 0x20007D9C
g_demo_temperature_x10  @ 0x200003B6
g_demo_alarm_active     @ 0x20007D94
g_pid_target_rpm        @ 0x200003EC
g_pid_feedback_rpm      @ 0x20007DB4
g_pid_error_rpm         @ 0x20007DAC
g_pid_output_permille   @ 0x20007DC0
```

原始内存和核心状态：

```text
0x200003C8: CD 27 35 01 00 00 96 44 0A D7 A3 3B CD CC 4C 3F
SCB.VTOR = 0x08005000
SCB.CFSR = 0x00000000
SCB.HFSR = 0x00000000
```

前四字节按小端序等于 `20260813`。AXF 解析得到 Flash 使用 115156 / 524288 B（21.96%），RAM 使用 35960 / 65536 B（54.87%）。地址只对本次构建有效。详见[AI 读取变量、RAM 和寄存器](memory-workflow.md)与[Memory 手动教程](../tools/microlink/memory.md)。

## 6. PID / SuperWatch 数据分析

提示词：

> 采 15 秒 PID 变量，算响应时间和超调，再试一下 Kp=0.70，没变好就恢复。

AI 使用 MKLink 多区域内存采样得到 650 个样本、13.102 秒数据，实际 49.5 Hz。基线 `1200 -> 800 rpm` 进入 10% 误差带用时 2.604 秒，超调 0%。

初始参数和保护状态：

```text
Kp=0.60  Ki=0.80  Kd=0.005
output_min=0  output_max=1000  fault_flags=0
```

AI 临时将 Kp 改为 0.70，其他条件不变：

```text
800 -> 1200 rpm：2.665 s，超调 0%
1200 -> 800 rpm：2.665 s，超调 0%
```

结果没有改善，最终回滚并回读确认 `Kp/Ki/Kd=0.60/0.80/0.005`、`fault_flags=0`。本次 `superwatch --dump-mem` 还出现地址范围误判且请求 10 ms 时只有约 7 Hz，随后改用同一 MKLink 的 `dump-memory`。完整边界和 FOC 映射见[AI 调试 PID](pid-workflow.md)，人工画曲线见[SuperWatch 与 PID 调试](../tools/microlink/superwatch.md)。

## 7. HardFault 受控定位与恢复

提示词：

> 复现一次 HardFault，先保存现场并定位源码，完成后恢复安全固件。

触发前：

```text
g_hardfault_demo_arm=0
g_hardfault_demo_count=0
CFSR=0  HFSR=0
```

向 `g_hardfault_demo_arm@0x20007DA0` 写入 `0x48464C54` 后：

```text
CFSR=0x00000001  IACCVIOL
HFSR=0x40000000  FORCED
g_hardfault_demo_count=1
心跳冻结
```

自动解码最初误选 MSP。AI 检查 RT-Thread 异常入口后，按 `PSP=0x2000C534` 加上入口压入的 9 words，从 `0x2000C558` 解码硬件异常帧：

```text
LR = 0x080128E3 -> applications/main.c:128
PC = 0xFFFFFFF0
xPSR = 0x01000000
```

无效执行地址与 `IACCVIOL` 一致。随后 Keil 重烧安全固件，确认 `CFSR=0`、`HFSR=0`、计数清零、build_id 正确、心跳增长且 RTT 恢复。详见[AI 定位 HardFault](hardfault-workflow.md)与[HardFault 手动教程](../tools/microlink/hardfault.md)。

## 8. SystemView 分析任务和 ISR

提示词：

> 采 6 秒 RTOS Trace，检查丢包，再分析任务占用和最长 ISR。

有效采集结果：

```text
6.026 s，20957 events，9 tasks
3440 switches，570.9/s
tidle0 96.72%  main 0.87%  pidloop 0.80%  svfast 0.56%  rttrx 0.37%
ISR 6026 次，占 1.61%，最长 225.1 us
```

分析器把 `tidle0 96.72%` 报成 CPU starvation，这是语义误报：空闲任务高占用表示 CPU 大部分时间空闲。最长 ISR 225.1 µs 仍需结合产品实时性预算评估。详见[AI 分析 RTOS Trace](systemview-workflow.md)与[SystemView 手动教程](../tools/microlink/SystemView.md)。

## 9. 部署并触发脱机下载

提示词：

> 把刚验证过的 HEX 做成脱机任务，检查算法和 SHA，部署后触发一次。

本次使用 `STM32F10x_512.FLM`，没有采用容量错误的 1024 KiB 算法。任务文件：

```text
G:\python\stm32f103ret6_ai_demo.py
G:\stm32f103ret6_ai_demo.hex
G:\FLM\STM32F10x_512.FLM
```

触发结果：

```text
Download: 100% ,used 12791 ms
/stm32f103ret6_ai_demo.hex loaded success
auto download finished
```

U 盘 HEX 与 Keil HEX 的 SHA-256 一致，随后 RTT 再次输出 build 20260813 和完整启动段。详见[AI 部署脱机下载](offline-workflow.md)与[脱机下载手动教程](../tools/microlink/offline_download.md)。

## 10. 最终验收

- 构建为 0 错误、0 警告，AXF/MAP/HEX 来自同一次构建；
- 在线下载完成 erase、program、verify 和运行验证；
- 脱机任务完成部署、真实触发和 RTT 复验；
- RTT、变量、RAM 和寄存器数据互相一致；
- PID 参数实验无改善后已回滚；
- HardFault 现场已定位，目标已恢复安全固件；
- SystemView 采集有效，自动告警经过 RTOS 语义复核。

每项判断均可回到构建产物、原始硬件值和可重复的验收条件。
