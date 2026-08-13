# AI 采集 RTT：用运行日志验证固件

RTT 通过调试接口读写目标 RAM 中的环形缓冲区，不占用产品 UART。RT-Thread 的 `rt_kprintf` 可以直接输出到 RTT Channel 0，适合验证启动版本、状态机和低速运行数据。

## 示例代码

本次 STM32F103RET6 示例在启动时输出构建标识，在主循环中输出运行状态：

```c
volatile rt_uint32_t g_mklink_demo_build_id = 20260813U;

rt_kprintf("MKLink AI demo | build=%lu | target=STM32F103RET6 | rtos=RT-Thread 5.1.0\r\n",
           (unsigned long)g_mklink_demo_build_id);

rt_kprintf("MKLink demo | uptime=%lu ms | temp=%.1f C | speed=%u rpm | state=%u | alarm=%u\r\n",
           (unsigned long)g_demo_uptime_ms,
           g_demo_temperature_x10 / 10.0f,
           g_demo_motor_speed_rpm,
           g_demo_operating_state,
           g_demo_alarm_active);
```

## 开始采集

给AI 的提示词：

> 读这次构建的 RTT，抓 10 秒，检查版本、uptime 和温度告警。

AI 使用当前 MAP 定位 RTT 控制块。本次地址为 `0x20000B18`，不能直接套用旧构建中的地址。

## 实际输出

```text
RT-Thread 5.1.0 build Aug 13 2026 16:56:20
RTT Control Block Detection Address is 0x20000b18
MKLink AI demo | build=20260813 | target=STM32F103RET6 | rtos=RT-Thread 5.1.0
MKLink demo | uptime=5002 ms | temp=30.0 C | speed=1050 rpm | state=0 | alarm=1
MKLink demo | uptime=10038 ms | temp=29.0 C | speed=1000 rpm | state=2 | alarm=0
```

## 结果说明

```text
build=20260813：确认运行的是本次构建。
uptime 由 5002 ms 增长到 10038 ms：主循环仍在运行。
30.0 C 时 alarm=1，29.0 C 时 alarm=0：阈值逻辑符合示例代码。
```

RTT 日志可以证明程序执行路径和低速状态，不能代替高速变量采样。控制环数据应继续使用[AI 使用 SuperWatch 调试 PID / FOC](pid-workflow.md)。人工操作见[RTT 终端与曲线](../tools/microlink/RTT_printf.md)。
