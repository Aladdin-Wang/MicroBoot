# RTT的使用原理

**调试口不只用来下载程序，也能收日志、发命令。** RTT 将通信缓冲区放在单片机 RAM 中，下载器通过调试接口读写，不需要额外占用 UART。

## 数据怎样到电脑上

SEGGER RTT 使用一个控制块，记录缓冲区地址、大小和读写位置。程序把输出写进上行缓冲区，下载器读取后交给电脑；电脑输入则写进下行缓冲区，由程序取走处理。[SEGGER RTT 原理说明](https://kb.segger.com/RTT)

![RTT 上下行缓冲区与数据流](../../images/mklink/rtt/rtt-data-flow.jpg)

| 方向 | 程序做什么 | MKLink 做什么 |
| --- | --- | --- |
| 板子 → 电脑 | 把日志或回显写入上行缓冲区 | 读取并显示到 RTT View |
| 电脑 → 板子 | 读取下行缓冲区，处理收到的命令 | 将输入送到目标缓冲区 |

## 为什么能当命令行

RTT 负责传输字节，命令的含义由目标程序决定。工程只接了日志输出，就只能看日志；同时接好命令行输入，才可以发送 `help`、查状态和修改参数。

> 把工程的命令行接到 RTT，发个 help，看看有哪些命令。

![RTT View 中的板上命令行](../../images/mklink/cases/stm32f103-rt-thread/stm32-rtt-console-version.png)

打开 **仪表盘 → RTT View**，定位控制块后开始，底部输入命令并发送。截图中的 STM32 工程使用 RT-Thread 5.1.0 的命令行，换成其他系统或裸机工程，也可连接自己的命令处理代码。

## 程序需要准备什么

将 RTT 组件加入工程，初始化缓冲区，并连接日志与输入接口。可以交给 AI 根据现有工程集成。MKLink 使用当前 AXF/ELF 查找控制块，不必手工固定地址。

重新编译后，控制块可能换位置。更新匹配的符号文件后再连接；确实需要固定地址时，要在链接布局中预留 RAM，避免与堆栈重叠。

## 会不会影响程序运行

RTT 通常无需暂停 CPU 来传输，但不是零开销：格式化日志、拷贝数据和调试总线访问都需要资源。缓冲区满时，阻塞模式会等待，非阻塞模式则可能舍弃数据。[SEGGER RTT 缓冲模式](https://www.segger.com/products/debug-probes/j-link/technology/about-real-time-transfer/)

因此，高频控制数据优先用 [SuperWatch](../observation/superwatch.md) 观察；日志保留关键状态和错误即可。命令行与日志的具体操作见 [RTT View 使用说明](../observation/rtt.md)。
