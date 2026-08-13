# AI 读取变量、RAM 和寄存器

变量读取适合看类型化的运行状态，原始内存适合核对底层字节，寄存器读取用于确认 Cortex-M 的核心状态。三种结果结合起来，才能判断“源码中的值”和“芯片中的值”是否一致。

## 示例变量

```c
volatile rt_uint32_t g_mklink_demo_build_id = 20260813U;
volatile rt_uint32_t g_demo_uptime_ms;
volatile rt_int16_t  g_demo_temperature_x10 = 250;
volatile rt_uint8_t  g_demo_alarm_active;
volatile float       g_pid_target_rpm = 800.0f;
volatile float       g_pid_feedback_rpm;
```

`volatile` 保留运行时读写，AXF 中的 DWARF 符号提供变量地址和类型。

## 开始读取

给AI 的提示词：

> 读 build、uptime、温度、告警和 PID 变量，再读 build 附近 16 字节以及 VTOR、CFSR。

## 类型化变量输出

```text
g_mklink_demo_build_id  @ 0x200003C8  uint32_t = 20260813
g_demo_uptime_ms        @ 0x20007D9C  uint32_t = 35228
g_demo_temperature_x10  @ 0x200003B6  int16_t  = 283
g_demo_alarm_active     @ 0x20007D94  uint8_t  = 0
g_pid_target_rpm        @ 0x200003EC  float    = 1200.0
g_pid_feedback_rpm      @ 0x20007DB4  float    = 1195.23
```

`g_demo_temperature_x10=283` 表示 28.3 °C；目标和反馈相差约 4.8 rpm，当前接近稳态。这里使用 AXF 类型信息，不需要手工猜测变量类型。

## 原始 RAM 输出

```text
0x200003C8: CD 27 35 01 00 00 96 44 0A D7 A3 3B CD CC 4C 3F
```

STM32F1 为小端序，前四个字节 `CD 27 35 01` 组成 `0x013527CD`，十进制为 `20260813`。原始字节用于交叉验证，变量类型仍以 AXF 为准。

## 寄存器和内存占用

```text
SCB.VTOR = 0x08005000
SCB.CFSR = 0x00000000
SCB.HFSR = 0x00000000

Flash: 115156 / 524288 B = 21.96%
RAM:    35960 / 65536 B = 54.87%
```

`VTOR=0x08005000` 与工程应用起始地址一致，CFSR/HFSR 为 0 表示当前没有活动 Fault。地址随链接布局变化，不能直接复制到其他工程。人工操作见[Memory 内存与寄存器](../tools/microlink/memory.md)。
