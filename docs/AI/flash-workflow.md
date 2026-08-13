# AI 编译、在线烧录与运行验证

构建成功只说明编译器生成了固件，下载成功也不等于应用已经正常运行。完整流程应包含：工程识别、构建、产物核对、下载校验和运行态验证。

## 示例中的构建标识

为了区分旧固件，在 `applications/main.c` 中加入：

```c
volatile rt_uint32_t g_mklink_demo_build_id = 20260813U;

rt_kprintf("MKLink AI demo | build=%lu | target=STM32F103RET6 | rtos=RT-Thread 5.1.0\r\n",
           (unsigned long)g_mklink_demo_build_id);
```

构建标识同时存在于 RAM 变量和 RTT 日志中，可用于烧录后的双重验证。

## 构建并下载

给AI 的提示词：

> 用 Keil 编译这个工程并烧录，完成后读 build 和 RTT 验证。

AI 识别到：

```text
目标：STM32F103RET6
Keil Device：STM32F103RE
Target：rt-thread
编译器：Arm Compiler 6.24
算法：STM32F10x_512.FLM
```

构建输出：

```text
0 Error(s), 0 Warning(s)
Code=94694  RO-data=20462  RW-data=2504  ZI-data=35392
```

同一次构建的产物：

```text
build/keil/Obj/rt-thread.axf
build/keil/Obj/rt-thread.hex
build/keil/List/rt-thread.map
HEX SHA-256：FD86D173977506E46C7CDEC2BD9C648940562B6FD86059433BEBC6FFECEF051E
AXF SHA-256：ADCB3D772269C5B0D2CCD4777CBA6672E08328290539B9587529E540B96BEA98
```

Keil 原生下载输出：

```text
Erase Done.
Programming Done.
Verify OK.
Application running ...
```

## 运行验证

```text
g_mklink_demo_build_id @ 0x200003C8 = 20260813
MKLink AI demo | build=20260813 | target=STM32F103RET6 | rtos=RT-Thread 5.1.0
```

RAM 变量和 RTT 日志都包含 `20260813`，确认目标运行的是本次构建，而不是 Flash 中残留的旧版本。

工程已有 Keil/IAR 时优先使用 IDE 原生下载；只有预编译镜像时才选择其他在线下载方式；量产任务见[AI 部署脱机下载](offline-workflow.md)。人工操作见[在线编译与烧录](../tools/microlink/online-flash.md)。
