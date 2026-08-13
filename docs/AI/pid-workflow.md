# AI 调试 PID：用数据判断参数是否有效

PID 调试不能只看曲线“是否顺眼”。参数修改前应保存基线，修改后在相同阶跃和负载下比较响应时间、超调、稳态误差和输出限幅，结果没有改善就回滚。

本例在 STM32F103RET6 内运行一阶电机模型，不驱动真实电机，用于演示变量采样和受约束调参。

## 示例代码

```c
volatile float g_pid_target_rpm = 800.0f;
volatile float g_pid_feedback_rpm;
volatile float g_pid_error_rpm;
volatile float g_pid_output_permille;
volatile float g_pid_kp = 0.60f;
volatile float g_pid_ki = 0.80f;
volatile float g_pid_kd = 0.005f;
volatile float g_pid_output_min = 0.0f;
volatile float g_pid_output_max = 1000.0f;
volatile rt_uint8_t g_pid_fault_flags;
```

控制线程每 10 ms 执行一次，目标转速每 6 秒在 800 rpm 和 1200 rpm 之间切换。真实 FOC 工程可以换成 `Iq_ref/Iq`、`Id_ref/Id`、电压指令和保护标志。

## 开始测试

给AI 的提示词：

> 采 15 秒目标、反馈、误差和输出，算响应时间和超调。再试一下 Kp=0.70，没变好就恢复。

AI 先读取参数和保护状态：

```text
Kp=0.60  Ki=0.80  Kd=0.005
output_min=0  output_max=1000
fault_flags=0
```

## 基线结果

```text
samples=650
duration=13.102 s
actual_rate=49.5 Hz
step=1200 -> 800 -> 1200 rpm
1200 -> 800: 进入 10% 误差带 2.604 s，超调 0%
```

第二个升阶跃在窗口结束时尚未稳定，因此没有计算不完整的响应时间。

## 修改 Kp 后复测

```text
write g_pid_kp @ 0x200003D8: 0.60 -> 0.70
Ki=0.80, Kd=0.005, output limits unchanged

800 -> 1200 rpm：进入 10% 误差带 2.665 s，超调 0%
1200 -> 800 rpm：进入 10% 误差带 2.665 s，超调 0%
```

2.665 秒没有优于基线的 2.604 秒，因此恢复原值：

```text
rollback g_pid_kp: 0.70 -> 0.60
readback: Kp=0.60, Ki=0.80, Kd=0.005
fault_flags=0
```

## 采样限制

本轮 `superwatch --dump-mem` 将 `g_pid_target_rpm@0x200003EC` 误判为不可写区域，请求 10 ms 时实际采样率也只有约 7 Hz。随后改用 MKLink 多区域 `dump-memory`，获得稳定的 49.5 Hz 数据。教程中的采样率均为实际值，不是配置值。

真实电机和 FOC 调试还要先定义过流、母线电压、温度、失速和急停边界。人工曲线操作见[SuperWatch 与 PID 调试](../tools/microlink/superwatch.md)。
