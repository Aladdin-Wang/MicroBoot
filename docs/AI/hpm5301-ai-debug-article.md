# HPM5301：AI 全程调试实录

本文以 HPM5301 EVK Lite 和 HPM SDK 的 `segger_sysview/freertos` 示例为例，记录一次完整的 AI 调试过程：读取工程、修改代码、编译、下载、观察控制量、定位 RISC-V Trap，最后恢复并复核固件。数据来自 2026-08-13 连接在 COM220 的实机，命令和输出可以直接作为自己的操作模板。

页面配置、曲线和人工操作见 [HPM5301 + FreeRTOS 全功能实战](../tools/microlink/hpm5301-freertos-case.md)。ARM 工程的 RTT、变量、Memory、SuperWatch 和 SystemView 操作与本文一致，差别只有下载后端和异常寄存器：HPM 使用 ROM API，故障现场读取 RISC-V CSR。

## 工程和目标

```text
目标：HPM5301xEGx
开发板：hpm5301evklite
工程：hpm_sdk/samples/segger_sysview/freertos
IDE：SEGGER Embedded Studio 8.24
RTOS：FreeRTOS
下载基址：0x80000400
探针：COM220，IDCODE 0x1000563D
```

给 AI 的请求保持简短即可：

> 把这个 HPM5301 FreeRTOS 工程编译、下载，先看 RTT 和 PID，再定位一次 Trap，最后恢复并给出证据。

## 关键代码

### PID 和运行日志

示例工程增加了可读的控制变量。AI 通过 ELF 符号或 MAP 地址读取这些变量，不需要在业务代码中增加串口协议。

```c
volatile float motor_target_rpm = 800.0f;
volatile float motor_speed_rpm;
volatile float motor_command_percent;
volatile float pid_kp = 0.080f;
volatile float pid_ki = 0.120f;
volatile float pid_kd = 0.001f;
volatile uint32_t alarm_code;

motor_target_rpm = ((uptime_ms / 6000U) & 1U) == 0U ? 800.0f : 1600.0f;
motor_command_percent = clampf(
    pid_kp * error + pid_ki * pid_integral + pid_kd * derivative,
    0.0f, 100.0f);
motor_speed_rpm = clampf(
    speed + (requested_speed - speed) * 0.035f,
    0.0f, 2400.0f);
```

RTT 每 100 ms 输出一次摘要，适合确认程序路径和告警状态：

```c
SEGGER_RTT_printf(
    0,
    "HPM5301 t=%u target=%d speed=%d output=%d temp=%d state=%u alarm=%u\\n",
    uptime_ms,
    (int) motor_target_rpm,
    (int) motor_speed_rpm,
    (int) motor_command_percent,
    (int) motor_temperature_c,
    operating_state,
    alarm_code);
```

### Trap 现场保存

故障处理函数把硬件现场保存在 RAM 中。用户排查时只需读取这些字段，不需要了解故障触发保护机制。

```c
__attribute__((noinline)) static void trigger_illegal_instruction(void)
{
    __asm volatile(".word 0xffffffff");
}

void freertos_risc_v_application_exception_handler(uint32_t cause, uint32_t epc)
{
    trap_mcause = cause;
    trap_mepc = epc;
    trap_mtval = read_csr(CSR_MTVAL);
    trap_mstatus = read_csr(CSR_MSTATUS);
    trap_state = 2U;

    for (;;) {
        __asm volatile("nop");
    }
}
```

## 编译和下载

使用 SES 的 `emBuild.exe` 构建 Debug 配置，构建退出码为 0。本文所用证据组必须来自同一时间的 BIN、ELF 和 MAP：

```text
demo.bin  55,412 bytes
demo.elf  617,621 bytes
demo.map  220,783 bytes
BIN SHA-256: CD472E4D019A1AB013431504838441C172198508F1BB40E040C6E9F5654514CA
ELF SHA-256: FD4695CAD18487BC5EFD90DEFE39079E73AF4480E075B551AD3516DE0B8D698A
```

HPM 不使用 FLM。AI 调用 MKLink MCP `flash` 时必须给出精确型号、BIN 基址和板型：

```text
flash(
  firmware=".../demo.bin",
  target_part="HPM5301xEGx",
  base_address=0x80000400,
  board="hpm5301evklite",
  verify=true,
  reset_after=true
)
```

实机下载输出：

```text
open fileName: demo.bin success,file size: 55412 byte
Download: 100% ,used 1792 ms
demo.bin loaded successfully.
algorithm_source: hpm-rom-api
verified: true
```

## RTT 和 PID 证据

本次 ELF 的 `_SEGGER_RTT` 地址为 `0x00087100`。启动 RTT 后读取 2 秒，得到：

```text
HPM5301 t=2100 target=800 speed=770 output=37 temp=32 state=1 alarm=0
HPM5301 t=2500 target=800 speed=781 output=38 temp=33 state=1 alarm=0
HPM5301 t=3000 target=800 speed=789 output=38 temp=33 state=1 alarm=0
HPM5301 t=3500 target=800 speed=793 output=38 temp=33 state=1 alarm=0
HPM5301 t=4000 target=800 speed=796 output=38 temp=33 state=1 alarm=0
```

判断依据很明确：速度向目标值收敛，`state=1` 表示正常运行，`alarm=0` 表示没有告警。需要观察控制环细节时，再用 SuperWatch 连续采样 `motor_target_rpm`、`motor_speed_rpm`、`motor_command_percent` 和 PID 参数；该操作和 STM32 完全相同。

## RISC-V Trap：从现象定位到源码

### 1. 先确认故障现象

触发后 RTT 停止输出，读取保留变量得到：

```text
trap_state  = 0x00000002
trap_mcause = 0x00000002
trap_mepc   = 0x8000B6E2
trap_mtval  = 0xFFFFFFFF
trap_mstatus= 0x00001880
```

### 2. 按故障代码判断原因

RISC-V `mcause=2` 的含义是 **Illegal instruction（非法指令）**。`mtval=0xFFFFFFFF` 是处理器报告的指令字，与代码中的 `.word 0xffffffff` 一致，因此故障类型不是看门狗、总线访问或栈溢出，而是执行到了非法指令。

### 3. 用同一 ELF 映射 `mepc`

使用构建时配套的 ELF 查询 `mepc`，不要拿旧版本 ELF 交叉解析：

```text
riscv32-unknown-elf-addr2line.exe -e demo.elf -f -C 0x8000B6E2
trigger_illegal_instruction
.../src/main.c:68
```

源码第 68 行正是 `.word 0xffffffff`。至此，CSR、指令字、函数名和源码行相互印证，定位完成。

### 4. 恢复并验证

重新用同一份 `demo.bin` 走 HPM ROM API 下载并复位。恢复后的现场为：

```text
trap_state/mcause/mepc/mtval/mstatus = 0
alarm_code = 0
```

RTT 随后恢复输出，速度从 770 rpm 逐步回到 796 rpm，说明任务调度、控制环和 RTT 均已重新运行。若实际项目停在 Trap，应先保存 CSR、栈帧和 ELF 版本，再修复源码，不能只复位后继续使用。

## 其他能力如何迁移

HPM5301 的变量读取、内存读写、SuperWatch、RTT View、VOFA+ 和 SystemView 的调用方式与 ARM 目标一致：先连接并加载 ELF，再按变量名或地址采样。只有两点不同：

1. 下载使用 HPM ROM API，不寻找 FLM；
2. 异常定位读取 `mcause/mepc/mtval/mstatus`，而 Cortex-M 使用 CFSR/HFSR 和异常栈帧。

## 脱机下载验收

将已验证的 `demo.bin` 部署到下载器后，触发一次脱机任务。部署和触发都必须核对文件大小与 SHA-256，避免同名旧固件混入：

```text
demo.bin  55412 bytes
SHA-256   CD472E4D019A1AB013431504838441C172198508F1BB40E040C6E9F5654514CA
load.offline("Python/offline_download.py")
IDCODE: 0x1000563D
open fileName: demo.bin success,file size: 55412 byte
Download: 100% ,used 1840 ms
demo.bin loaded successfully.
auto download finished
```

触发完成后重新读取 `demo_build_id`、`alarm_code` 和 RTT 启动段，三项都匹配才算验收通过。本次部署的文件清单为 `G:\demo.bin` 和 `G:\python\offline_download.py`；HPM 脱机脚本没有复制或加载 FLM。

## 验收清单

- [x] SES 构建退出码为 0，BIN/ELF/MAP 同一证据组
- [x] HPM ROM API 下载，基址 `0x80000400`，校验通过
- [x] RTT 输出正常，PID 速度收敛，`alarm=0`
- [x] `mcause=2`、`mtval=0xffffffff` 与源码第 68 行吻合
- [x] 重烧后 Trap 现场清零，RTT 和控制任务恢复
- [x] 脱机部署使用相同摘要，并在目标上复核运行结果

完整 GUI 页面和曲线请看 [HPM5301 + FreeRTOS 全功能实战](../tools/microlink/hpm5301-freertos-case.md)。
