# AI 定位 HardFault：从异常现场回到源码

HardFault 发生后，先不要复位。复位会清除 Fault 状态，也可能覆盖异常栈。正确流程是：保存寄存器和栈帧，使用同一次构建的 AXF 将 PC/LR 映射到源码，再恢复固件并复验。

## 1. 准备可控的演示入口

`applications/main.c` 中使用一个受保护的故障入口。正常运行不会触发，只有写入指定魔数才执行非法调用：

```c
#define MKLINK_HARDFAULT_ARM_VALUE 0x48464C54UL

volatile rt_uint32_t g_hardfault_demo_arm;
volatile rt_uint32_t g_hardfault_demo_count;

static void mklink_hardfault_demo_entry(void *parameter)
{
    RT_UNUSED(parameter);
    while (1) {
        if (g_hardfault_demo_arm == MKLINK_HARDFAULT_ARM_VALUE) {
            void (*invalid_function)(void) = (void (*)(void))0xFFFFFFF1UL;
            g_hardfault_demo_arm = 0U;
            g_hardfault_demo_count++;
            invalid_function();
        }
        rt_thread_mdelay(20);
    }
}
```

`0xFFFFFFF1` 不是有效的 Thumb 函数地址，CPU 取指时会产生 `IACCVIOL`。这是文档演示代码，量产固件不应保留故障注入入口。

## 2. 故障前基线

给AI 的提示词：

> 这块板子要复现一次 HardFault。先读 build、Fault 寄存器和心跳，保存现场后再触发。

AI 的基线输出：

```text
build_id=20260813
g_hardfault_demo_arm=0
g_hardfault_demo_count=0
SCB.CFSR=0x00000000
SCB.HFSR=0x00000000
g_mklink_heartbeat=不断增长
```

## 3. 触发并读取 Fault

AI 按 AXF 符号写入魔数：

```text
write g_hardfault_demo_arm @ 0x20007DA0 <- 0x48464C54
write verified: 0x48464C54
```

触发后读取到：

```text
SCB.CFSR = 0x00000001  (IACCVIOL)
SCB.HFSR = 0x40000000  (FORCED)
g_hardfault_demo_count = 1
g_mklink_heartbeat      = 停止增长
```

`IACCVIOL` 是指令访问违规；`FORCED` 表示可配置 Fault 升级成 HardFault。MMFAR/BFAR 的有效位没有置位，不能使用其中残留的地址。

## 4. 计算真正的异常栈地址

第一次自动解码将 MSP 当作栈指针，错误定位到 `rt_hw_context_switch_to`。RT-Thread 线程态使用 PSP，`context_rvds.S` 入口还会压入 `EXC_RETURN` 和 R4-R11，共 9 个字，即 36 字节。

```text
PSP = 0x2000C534
硬件异常帧 = PSP + 9 * 4 = 0x2000C558
```

硬件异常帧顺序是 `R0,R1,R2,R3,R12,LR,PC,xPSR`。AI 从 `0x2000C558` 解码并用本次 AXF 映射：

```text
LR   = 0x080128E3 -> applications/main.c:128
PC   = 0xFFFFFFF0
xPSR = 0x01000000
```

PC 是无效执行地址，LR 指向非法调用路径，与 `IACCVIOL` 相互印证。这里不能只复制自动解码结果，必须结合 RTOS 异常入口判断 MSP/PSP 和额外压栈。

## 5. AI 最终输出和恢复

```text
结论：确认是受控的非法函数地址调用。
证据：CFSR.IACCVIOL=1，HFSR.FORCED=1，PC=0xFFFFFFF0，
      LR=0x080128E3 对应 applications/main.c:128。
限制：MMFAR/BFAR 无有效标志，不能作为故障地址。
处理：已重新下载安全固件并验证恢复。
```

恢复输出：

```text
Erase Done.
Programming Done.
Verify OK.
Application running ...
CFSR=0x00000000
HFSR=0x00000000
g_hardfault_demo_count=0
g_mklink_heartbeat=继续增长
```

人工操作见[HardFault / RISC-V Trap 现场分析](../tools/microlink/hardfault.md)。
