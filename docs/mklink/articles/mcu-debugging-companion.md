# RTT的使用原理

## 一、下载口也可以同时当串口用会有多爽

- 不需要USB 转串口工具 + 杜邦线接线
- printf直接通过下载口输出到USB虚拟串口
- 边仿真边打印，互不影响

请看在rtthread系统中使用的演示视频：

V4下载器演示视频：



V3下载器演示视频：



## 二、SEGGER RTT+ MKLink，让串口调试真正自由

在嵌入式开发中，我们总是离不开**“串口打印”**来调试。

但传统串口调试存在很多明显的痛点：

- ⚡ **阻塞式打印占用 CPU 时间**：大量输出容易拖慢程序，不宜放在对耗时敏感的中断中；
- 🔌 **硬件占用**：需要额外占用 MCU 的 UART 接口资源；
- 🔄 **接线麻烦**：需要 USB 转串口工具 + 杜邦线接线，步骤繁琐；
- 🚪 **资源受限**：串口数量有限，调试与功能常常冲突。

### 如何打破这些痛点？

**SEGGER  RTT + MKLink**完美结合了双方的优势：

- **SEGGER RTT**：提供高速、非侵入式的数据传输；
- **MKLink**：将 RTT 通道虚拟为标准 USB CDC 串口，不再占用 MCU 的串口！

**📢 让调试既拥有 RTT 的性能，又能使用任意串口助手，真正做到“即插即用，自由畅快”！**

## 三、一分钟了解 SEGGER RTT 是什么、怎么用

### 1、SEGGER RTT是什么？

RTT，全称**Real Time Transfer（实时传输）**，是一种**无需中断 MCU 程序执行**，就能实现数据与主机交互的调试技术。

它使用一种内存共享机制，将 MCU 内部的数据实时“搬运”到 PC 上。

**📦 形象理解：：**

> 就像你在 MCU 的 RAM 里放了个“邮箱”，PC 随时来收信，MCU 照常干活，互不打扰。

### 2、SEGGER RTT 的基本工作原理

🔵 **在 MCU RAM 中，有一个非常重要的结构体：**

> **_SEGGER_RTT 控制块**

🔵 **它的作用是：**

- 保存多个 **UpBuffer（MCU ➡ PC）** 和 **DownBuffer（PC ➡ MCU）** 的信息；
- 包括每个缓冲区的起始地址、大小、写指针、读指针等。

🔵 **收发数据过程：**

- **MCU发送数据** ➔ 把数据 `memcpy` 拷贝到 **UpBuffer** 的空闲区域；
- **PC接收数据** ➔ 通过MKLink 读取 UpBuffer 的数据；
- **PC发送指令** ➔ 通过MKLink 把数据写入 DownBuffer；
- **MCU读取指令** ➔ 从 DownBuffer 中 `memcpy` 出来。

程序将数据写入 RAM 缓冲区，下载器再通过调试接口搬运，因此通常无需暂停 MCU 就能收发。完整传输时间取决于数据量和链路速度；缓冲区满时，阻塞模式会等待，非阻塞模式可能丢弃数据。[SEGGER RTT 缓冲模式说明](https://kb.segger.com/RTT)

![RTT.drawio](../../images/mklink/rtt/rtt-data-flow.jpg)

### 3、SEGGER RTT怎么用？

只需简单三步：

✅ **步骤一**：集成 RTT 源码

从 SEGGER J-Link 安装目录 `Samples/RTT` 复制以下文件到工程中，并添加头文件路径。

如我电脑上的路径：

C:\Program Files (x86)\SEGGER\JLink_V632f\Samples\RTT

![rtt_code](../../images/mklink/rtt/rtt-code.jpg)

✅ **步骤二**：输出日志到 RTT

```c
#include "SEGGER_RTT.h"
int main(void)
{
    SEGGER_RTT_Init();
    SEGGER_RTT_printf(0, "hello RTT\n");
    while(1){

    }
}
```

✅ **步骤三**：连接调试工具

- 如果使用 J-Link，可以用 RTT Viewer、RTT Client 等工具；
- 如果用 MKLink ，可以用**任意串口助手**直接访问 RTT 数据！

## 四、MKLink ：释放 SEGGER RTT的真正威力

**MKLink突破传统**，打通了 RTT 和通用串口调试工具之间的隔阂，直接把 RTT 数据转发到 **USB CDC 虚拟串口**，让 MCU 仿佛接了一个超级快的“软串口”！

🔵 MCU端：

- 继续使用 RTT 库发送日志，不需要改变一行代码。

🔵 MKLink端：

- 通过 USB CDC 映射成标准串口；
- 自动扫描 MCU 内存中 **_SEGGER_RTT** 控制块地址（如 0x20000000）；
- 直接读写 UpBuffer / DownBuffer；
- 完美支持双向通信！

🔵 PC端：

- 用你最喜欢的串口助手直接连 MKLink串口，爽快收发！

📷 **MKLink 数据流示意图：**

![RTT_printf](../../images/mklink/rtt/rtt-printf.jpg)



✅ **效果总结：**

- 不再局限于官方 Viewer；
- 不再受限于波特率；
- 不再需要额外串口硬件和线缆；
- 打开任意串口助手即用，极致灵活！

## 五、多种使用 SEGGER RTT 功能的方法

### 1、如何开启MKLink的SEGGER RTT功能

✅ **步骤一：找到MKLink 的 USB CDC 虚拟串口**

使用USB TypeC数据线与MKLink连接以后，电脑设备端会弹出三个设备：

![](../../images/mklink/communication/usb-devices.png)

- V2和V3会弹出两个USB串行设备端口号，分别是USB转串口和虚拟串口

- V4会弹出三个USB串行设备端口号，分别是USB转串口、USB转485端口和虚拟串口

打开虚拟串口后，下载器会自动打印如下信息：

![](../../images/mklink/communication/usb-cdc.png)

V4版本可以通过屏幕界面，来告诉你打开的是什么端口，分别打开三个串口号，效果如下:



✅ **步骤二：使用串口助手类工具访问 MKLink 的 USB CDC 虚拟串口**

比如使用SSCOM，连接MicroLink的串口，输入以下指令：

```
RTTView.start(0x20000000,1024,0)
```

- 0x20000000:搜索RTT控制块的起始地址；
- 1024：搜寻范围大小；
- 0：启动RTT的通道。

![RTT_MAP](../../images/mklink/rtt/rtt.jpg)

**_SEGGER_RTT** 控制块地址可以通过查看MDK编译生成的.map文件来查找，如下：

![RTT_MAP](../../images/mklink/rtt/rtt-map.jpg)

可知，\_SEGGER_RTT在地址0x20000040处，可以通过设置搜寻的地址和大小来重新启动MicroLink的RTT功能。

### 2、固定_SEGGER_RTT的地址的方法

✅ **步骤一：**打开SEGGER_RTT.c，添加红框中的代码，宏 SEGGER_RTT_OPS_ADDR 指定控制块地址；下面代码以 0x20000200 为例

![RTT_MAP](../../images/mklink/rtt/rtt-address.png)

代码如下，方便直接复制：

```c
#define SEGGER_RTT_OPS_ADDR 0X20000200
#define __ARM_AT(x) ".ARM.__at_"#x
#define ARM_AT(x) __ARM_AT(x)
#if defined(__CC_ARM)  // ARM Compiler 5 (AC5)
    #define SEGGER_RTT_SECTION __attribute__((at(SEGGER_RTT_OPS_ADDR)))
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6000000)  // ARM Compiler 6 (AC6)
    #define SEGGER_RTT_SECTION __attribute__((section(ARM_AT(SEGGER_RTT_OPS_ADDR))))
#elif defined(__GNUC__)  // GCC
    #define SEGGER_RTT_SECTION __attribute__((section(".segger_rtt_ops"), used, aligned(4)))
#else
    #define SEGGER_RTT_SECTION
#endif
SEGGER_RTT_SECTION
SEGGER_RTT_PUT_CB_SECTION(SEGGER_RTT_CB_ALIGN(SEGGER_RTT_CB _SEGGER_RTT));
```

固定地址需要在链接布局中预留对应 RAM，避免与堆栈和其他数据重叠。GCC 分支中的段名只指定放置区段，还需要在链接脚本中将该区段安排到预留地址。

### 3、上电自动开启RTT功能的方法

✅ **步骤一：**打开U盘中python文件夹下的default_config.py,添加红框中的代码，下载器上电会自动执行default_config.py脚本。

![RTT_MAP](../../images/mklink/rtt/rtt-auto-detection.png)



### 4、MDK中将printf重定向到RTT通道的方法

✅ **步骤一：RTE配置**

  1.打开 **RTE 配置**窗口（菜单：`Project -> Manage -> Run-Time Environment`）。

![RTT_MAP](../../images/mklink/rtt/cmsis-compiler.png)

   2.勾选以下选项：

- 在 **CMSIS-Compiler** 下勾选 **CORE**；
- 在 **STDOUT(API)** 下勾选 **Custom**；

如果你在**RTE**中找不到 **CMSIS-Compiler** ，说明你的**MDK**版本较低——如果不想升级**MDK**，则可以通过下面的链接从官方直接下载对应的**cmsis-pack**：

**https://www.keil.arm.com/packs/cmsis-compiler-arm/**

或者老版本的**cmsis-pack**中，找到**Compiler**：

![RTT_MAP](../../images/mklink/rtt/compiler.png)

✅ **步骤二：添加stdout_putchar()**

在代码中实现 **stdout_putchar()** 函数——用它来把printf重定向到RTT通道：

```c
int stdout_putchar(int ch)
{
    SEGGER_RTT_PutChar(0, ch);
    return ch;
}
```

### 5、将rtthread系统命令行重定向到RTT通道的方法

 **方法一：安装SEGGER_RTT软件包**

SEGGER_RTT软件包是将rtthread的msh重定向到SEGGER RTT

![RTT_MAP](../../images/mklink/rtt/rt-thread-rtt.png)

**方法二：安装agile_console软件包**

agile_console软件包可以将rtthread的msh重定向到多个端口，比如可以不影响原先uart打印的基础上，再增加一路RTT端口，比较适合两种方式需要同时使用的场景。

✅ **步骤一：安装软件包**

![RTT_MAP](../../images/mklink/rtt/agile-rtt.png)

✅ **步骤二：单片机添加RTT 源码**

从 SEGGER J-Link 安装目录 `Samples/RTT` 复制以下文件到工程中，并添加头文件路径。

如我电脑上的路径：

C:\Program Files (x86)\SEGGER\JLink_V632f\Samples\RTT

![rtt_code](../../images/mklink/rtt/rtt-code.jpg)

✅ **步骤三：单片机添加agile软件包的适配代码**

添加一个`agile_console_rtt_be.c`文件，代码如下：

```c
#include <rtthread.h>
#include <agile_console.h>
#include "SEGGER_RTT.h"
static struct agile_console_backend _console_backend = {0};

static void rtt_backend_output(rt_device_t dev, const uint8_t *buf, int len)
{
    SEGGER_RTT_Write(0,buf,len);
}

static int rtt_backend_read(rt_device_t dev, uint8_t *buf, int len)
{
    return  SEGGER_RTT_Read(0,buf, len);
}

static void segger_rtt_check(void)
{
    while (SEGGER_RTT_HasKey())
    {
        agile_console_wakeup();
    }
}

static int agile_console_rtt_init(void)
{
    SEGGER_RTT_Init();
    rt_thread_idle_sethook(segger_rtt_check);

    _console_backend.output = rtt_backend_output;
    _console_backend.read = rtt_backend_read;

    agile_console_backend_register(&_console_backend);
    return 0;
}
INIT_BOARD_EXPORT(agile_console_rtt_init);

```

## 六、用户真实评价

![rtt_code](../../images/mklink/support/wechat-article-contact.png)

## 七、新版 Web GUI：直接在 RTT View 收日志、发命令

前面的缓冲区原理和工程适配方法仍然适用。使用新版 MKLink 时，可以直接在网页操作，无需打开串口助手输入启动指令。

### 1、加载工程并开始接收

1. 打开 MKLink Web GUI，在 **配置** 中连接设备，加载与板上固件匹配的 AXF/ELF。
2. 进入 **仪表盘 → RTT View**，使用当前符号定位控制块，或点击“自动搜索”。
3. 使用工程对应的通道，通常为通道 0，点击“开始”。
4. 在接收区查看日志，在底部输入框发送命令。

重新编译后，控制块地址可能变化。更新当前符号文件再连接，不要照抄前文示例或旧截图中的地址。

### 2、不接 UART，照样使用板上命令行

RTT 传输的是字节，命令由目标程序解释。前面介绍的 RT-Thread 命令行重定向完成后，可以输入 `help` 查看可用命令，再输入 `version` 等工程支持的指令。

![新版 RTT View 中的 help 与 version 命令回显](../../images/mklink/cases/stm32f103-rt-thread/stm32-rtt-console-version.png)

*STM32 示例使用 RT-Thread 5.1.0，输入选择 CRLF 换行。自己的工程按命令行要求设置。*

如果工程只接了日志输出，还需要连接下行输入与命令处理，才能执行指令。也可以让 AI 完成适配：

> 把工程的命令行接到 RTT，发个 help，看看有哪些命令能用。

### 3、让 AI 帮忙查询与分析

> 查一下当前运行状态，抓一段日志，看看初始化有没有报错。

AI 可以通过 MKLink 发送命令、读取结果，再结合工程分析。你在 RTT View 查看同样的日志与回显；需要观察变量随时间的变化，再使用 SuperWatch。

切换到下载或其他独立采集前，先点击“停止”结束当前会话。仅暂停显示或切换页面不会释放设备。

[安装 MKLink](../getting-started/gui-install.md) · [RTT View 操作说明](../observation/rtt.md) · [用 AI 进行 RTT 调试](../../embedded-ai/workflows/rtt-workflow.md)
