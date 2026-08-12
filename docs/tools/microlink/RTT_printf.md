# RTT 终端与曲线

SEGGER RTT 通过调试接口读写目标 RAM 中的环形缓冲区，不占用 MCU UART，适合日志、命令行交互和实时数据观察。

## 嵌入式端准备

工程需要包含 `SEGGER_RTT.c/.h`，并在启动阶段初始化 RTT。最小输出示例：

```c
#include "SEGGER_RTT.h"

void app_log_init(void)
{
    SEGGER_RTT_Init();
    SEGGER_RTT_WriteString(0, "application started\r\n");
}
```

RT-Thread 工程可以将 console backend 重定向到 RTT，使 `rt_kprintf` 和 FinSH 复用通道 0。高频日志建议增大 Up Buffer，并使用非阻塞模式，避免日志反压业务任务。

## Web GUI 使用

1. 在“配置”中加载匹配当前固件的 AXF/ELF 或 MAP；
2. 连接 MKLink，打开“仪表盘 → RTT”；
3. 点击“自动搜索”定位 `_SEGGER_RTT`；
4. 选择通道 0 并开始采集；
5. 需要交互时在输入框发送命令；
6. 停止后保存原始日志。

![RTT 终端](../../images/microlink/gui/rtt-terminal.png)

RTT 与 RTOS Trace 共享控制块地址。任一页面找到地址后，另一页面会复用该设置。

## 找不到控制块

按以下顺序处理：

1. 确认当前固件实际执行了 `SEGGER_RTT_Init()`；
2. 确认 AXF/ELF 与已烧录固件来自同一次构建；
3. 在 MAP 中搜索 `_SEGGER_RTT`；
4. 增大搜索范围，或输入确认过的地址；
5. 检查控制块是否被堆、栈或 DMA 缓冲覆盖。

对于启动很早、低功耗或控制块可能被重定位的工程，可将 RTT 控制块固定到一段保留 RAM。链接脚本和堆边界必须同步为该区域留空间。

### RET6 实测输出

本例 `STM32F103RET6` 当前构建的 MAP 将 `_SEGGER_RTT` 解析为 `0x20000B18`。该地址只对本次 AXF/MAP 有效，重新编译后必须重新点击“自动搜索”。目标运行时可看到如下真实日志：

```text
MKLink demo | uptime=68501 ms | temp=29.0 C | speed=1000 rpm | state=1 | alarm=0
MKLink demo | uptime=69509 ms | temp=30.0 C | speed=1050 rpm | state=1 | alarm=1
MKLink demo | uptime=71525 ms | temp=32.0 C | speed=1150 rpm | state=2 | alarm=1
MKLink demo | uptime=74549 ms | temp=29.0 C | speed=1000 rpm | state=2 | alarm=0
```

温度达到 30.0°C 时 `alarm=1`，回落到 29.0°C 后恢复为 0。日志截图中的地址、来源 AXF 和状态栏应与当前会话一致。

## 成功判据

- 页面报告找到 RTT 控制块和有效通道；
- 日志连续且编码正确；
- 向下通道输入能得到预期响应；
- 停止后设备资源被释放；
- 分析结论保留原始日志和采集时长。

## 使用 AI

> 加载当前 ELF，自动定位 RTT，采集通道 0 十秒并保留原始日志。
