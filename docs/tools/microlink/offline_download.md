# 开源一个芯片自由的脱机下载器

## 一、脱机下载，也可以是自由的

💡 **什么是脱机下载器？**
简单来说，脱机下载器就是在**不连接电脑、不用专业软件**的情况下，也能帮你把程序烧录进芯片的工具。只要插上电源、按个按钮，固件就自动下载进 MCU，非常适合量产、售后、维修等场景。

🧩 **芯片自由**：适配你想用的芯片

通过加载标准的 **FLM 下载算法文件**（由 Keil Pack 或芯片厂商提供），MicroLink 可以支持几乎所有 Cortex-M 架构的芯片（如 STM32、GD32 等绝大多数国产芯片），包括内置 Flash 和外部 Flash 的烧写。

你无需等待更新固件适配芯片，无需绑定品牌，只要有 FLM 文件，就能烧。

📡 **编程自由**：流程由你定义

MicroLink 支持使用 Python 脚本完整编排脱机烧写过程：

- 烧哪个文件，烧到哪？
- 先烧 Boot，再烧 RTOS，再烧资源包？
- 烧完如何提示烧写成功？

全都不是问题。只需写几行 Python脚本，就能完整描述你的烧录流程。

> 🔧 **MicroLink 内部运行的是 Python 解释器（基于 PikaPython）**，脱机下载脚本会在脚本引擎中逐行执行，就像运行一个嵌入式程序一样灵活。

## 二、芯片自由的背后：如何让下载器不再“认芯片”？

在传统下载器中，“支持某颗芯片”往往意味着厂商写死支持逻辑，用户只能等待官方升级。但 MicroLink 打破了这种封闭方式，**通过动态加载芯片算法 + 脚本控制烧录流程**，实现了真正意义上的“芯片自由”。

我们将这个能力拆解为两个关键部分：

### 📦 1. FLM 算法加载机制：让下载器“学会”支持芯片

MicroLink 不内置任何芯片写死的支持代码，而是通过加载 Keil MDK 或芯片原厂提供的 **FLM 下载算法（Flash Loader）** 文件来完成烧录。

FLM 是一种标准化的芯片 Flash 编程算法格式，通常来自官方 Pack 包，包含以下核心内容：

- **Init**：芯片初始化代码（打开电源、时钟等）
- **Erase/Program**：擦除/写入 Flash 的函数
- **Sector Info**：每个 Flash 页的大小、布局等元信息

**🔍 IDE 是如何用 FLM 文件下载固件的？**

在 Keil、IAR 等 IDE 中，固件下载流程大致如下：

1. **选择芯片或目标板**：IDE 会根据芯片型号自动关联对应的 FLM 文件。
2. **加载 FLM 到 RAM**：IDE 把 FLM 算法文件中的代码段加载进目标 MCU 的 RAM。
3. **跳转执行**：通过调试接口（如 SWD），IDE 控制 MCU 跳转执行 FLM 中的 Init、Erase、Program 等函数。
4. **写入用户固件**：IDE 将用户编译好的 bin 文件，通过 FLM 算法完成 Flash 编程。

简单来说，IDE 并不是“自带”所有芯片的烧录代码，而是 **动态加载** FLM 算法，把烧录这件事交给芯片本身的 RAM 里执行。MicroLink 就复用了这套机制，使你不需要写适配代码，也能支持任意芯片。

📁 **FLM 文件获取方式：**

- 从 Keil MDK 安装路径中提取（通常在 `Keil\ARM\Flash`）
- 从芯片厂商提供的 Pack 包中提取

> ⚠️ 注意：不同芯片型号可能对应不同的 FLM 文件，请确认地址和容量匹配。

**MicroLink 如何利用 FLM 实现烧录？**

1. **解析 FLM 文件**（XXX.FLM）：提取必要代码段

2. **将代码加载进 MCU RAM**：作为“动态烧录器”运行
3. **通过调用烧录函数**：实现对目标芯片 Flash 的操作
4. **一颗芯片支持多个区域（如主 Flash + 外部 SPI Flash）**：可以加载多个 FLM 文件分别处理

这个过程中，MicroLink 自己并不关心芯片型号，只需要：

- 有合适的 FLM 算法
- 知道 Flash 起始地址
- 有合适的下载数据（bin）

> 这就像是“把芯片支持交给算法提供者”，MicroLink 只负责执行和协调。

### 💡 2. Python 脚本控制机制：让烧录流程更聪明

有了 FLM 算法，接下来是：**如何控制整个烧录流程**？例如，先烧 boot，再烧 app，再烧外挂 Flash，再提示成功。

传统脱机下载器往往只支持烧一个文件，流程固定。而 MicroLink 提供了完整的 Python 脚本接口，允许用户自己用代码定义“烧录流程”。

MKLINK V2示例脚本：

```python
import load
import PikaStdLib
import PikaStdDevice
import time

time = PikaStdDevice.Time()
buzzer = PikaStdDevice.GPIO()
buzzer.setPin('PA4')
buzzer.setMode('out')

ok = True
# 加载 FLM 文件
load.flm("FLM/STM32F10x_1024.FLM", 0x08000000, 0x20000000)

# 设置频率
cmd.set_swd_clock(5000000)

#下载bin文件
if load.bin("boot.bin", 0x08000000) != 0:
    ok = False
    
 #下载hex文件
if load.hex("rt-thread.hex") != 0:
    ok = False   
    
#蜂鸣器提示下载成功
if ok:
    buzzer.enable()
    buzzer.high()
    time.sleep_ms(500)
    buzzer.low()
    time.sleep_ms(500) 
    
```

MKLINK V3示例脚本：

```python
import PikaStdLib
import time
import cmd
import load

# 自动下载循环次数
AUTO_DOWNLOAD_COUNT = 1
# 等待读取目标IDCODE有效的超时时间（ms）
WAIT_IDCODE_TIMEOUT = 10000
# FLM 文件路径
FLM_FILE_PATH = "FLM/STM32F10x_1024.FLM"
# 目标 Flash 基地址
FLM_FLASH_BASE = 0x08000000
# 目标 RAM 基地址
FLM_RAM_BASE = 0x20000000
# HEX 文件路径（支持通配符）
HEX_FILE_PATH = "rt-thread.hex"
# BIN 文件路径（支持通配符）
BIN_FILE_PATH = "bootloader.bin"
# BIN 文件下载地址
BIN_FILE_ADD = 0x08000000
# SWD 时钟频率（Hz）
SWD_CLOCK_HZ = 10000000
 # 设置下载速度
cmd.set_swd_clock(SWD_CLOCK_HZ)

 # 自动循环下载 
abort = False
for i in range(AUTO_DOWNLOAD_COUNT):
    if abort:
        break
    print("=== Auto Download Round:", i + 1, "===")
    elapsed = 0
 # 等待连接目标板   
    while True:
        idcode = cmd.get_idcode()
        if idcode not in (0, 0xFFFFFFFF):
            break
        if elapsed >= WAIT_IDCODE_TIMEOUT:
            print("wait idcode online timeout")
            abort = True
            break
        print("=== waited_ms :", elapsed, "===")
        time.sleep_ms(500)
        elapsed += 500
    if abort:
        break
    print("IDCODE: 0x%08X" % idcode)
 # 加载下载算法 
    if load.flm(FLM_FILE_PATH, FLM_FLASH_BASE, FLM_RAM_BASE) != 0:
        print("load flm failed")
        abort = True
        break
 # 下载bin文件     
    if load.bin(BIN_FILE_PATH,BIN_FILE_ADD) != 0:
        print("load bin failed")
        abort = True
        break        
 # 下载hex文件     
    if load.hex(HEX_FILE_PATH) != 0:
        print("load hex failed")
        abort = True
        break
 # 循环次数为1，只下载一次        
    if  AUTO_DOWNLOAD_COUNT == 1:
        break    
    elapsed = 0
 # 等待断开连接目标板     
    while True:
        idcode = cmd.get_idcode()
        if idcode in (0, 0xFFFFFFFF):
            break

        if elapsed >= WAIT_IDCODE_TIMEOUT:
            print("wait idcode offline timeout")
            abort = True
            break
        time.sleep_ms(500)
        elapsed += 500
if not abort:
    print("auto download finished")
else:
    print("auto download aborted")

```

该脚本通过加载FLM算法文件，将多个二进制文件（如boot.bin、rt-thread.hex）分别烧录到STM32内部Flash中。

> **注意：**请根据您的实际项目需求，修改以下内容：
>
> - **下载算法文件名称**（如 `"STM32/STM32F10x_1024.FLM"` ）：应替换为对应芯片和Flash型号的 FLM 文件。
> - **BIN 文件名称及地址**（如 `"boot.bin"`、`"rt-thread.hex"`及其对应的地址）：请确保文件名和烧录地址与您的程序结构一致。
>
> 若文件名或地址设置不当，可能导致程序无法正常运行或烧录失败。

🧭 **脚本的作用是：**

- **按需加载多个 FLM 算法**（多片 Flash）
- **灵活控制烧写顺序与文件分布**
- **添加控制逻辑：判断、日志输出、状态指示**
- **控制外设：比如烧写成功后蜂鸣器响一下**
- **自动扫描单片机，根据循环次数自动下载**

> MKLink 的烧录逻辑不是固定的，而是**被你编写出来的**。

这相当于赋予了创客一个高度自由的平台，让你可以自己定义烧录策略，而不是被工具的“支持列表”所限制。

### 🎯 3. 如何使用脱机下载功能？

了解原理之后，我们来看 MicroLink 的脱机下载功能如何实际使用。**整个流程非常简单**，不需要专业软件、不依赖电脑，就可以完成整套烧录动作。

**✅ 第一步：准备 FLM 算法文件与烧录脚本**

你只需将以下文件拷贝到 MicroLink 的脱机下载目录中（类似 U 盘的文件夹）：

| 文件名                | 作用                               |
| --------------------- | ---------------------------------- |
| `*.FLM`               | FLM 下载算法文件（支持多个）       |
| `*.bin`               | 需要烧录的固件文件                 |
| `offline_download.py` | 控制烧录流程的 Python 脚本（必须） |

示例目录结构：

```
/MICROLINK/
│
├── offline_download.py
├── STM32F1x_1024.FLM
├── boot.bin
├── rt-thread.hex
```

------

**✅ 第二步：触发脱机下载（两种方式）**

MicroLink 支持两种脱机烧录触发方式：

**🔘 方式一：按键触发**

MKLink V3，板上有下载按钮。

- **按下按钮** → MKLink V3 自动执行 `offline_download.py` 脚本
- **烧录成功** → LED亮绿灯
- **烧录失败**→  LED亮红灯
- **可用于量产、售后维修、断网环境**

> 纯离线运行，不依赖上位机。

------

![](../../images/microlink/key.png)

MKLink V2，可用于机台烧录。

- **触发下载**→ TDI引脚与GND短接触发执行 `offline_download.py` 脚本
- **烧录成功** → TDO引脚可接蜂鸣器或者LED灯提示烧录成功或者失败
- **可用于量产、售后维修、断网环境**

**💻 方式二：串口命令触发**

如果 MicroLink 已连接电脑，你可以通过 USB CDC 虚拟串口发送`load.offline()`加回车，效果如下：

![](../../images/microlink/load_offline.jpg)

该命令会立即触发一次离线脚本的执行，适合：

- 测试脚本是否正确
- 在工控系统中集成自动烧录流程
- 将离线流程作为可控子模块远程触发。



## 三、未来展望：不断扩展功能，打造工程师手边真正好用的工具

### 🛒 1. 淘宝购买链接

如果你想支持这个项目，也欢迎购买 MicroLink 实体设备：

🔗 淘宝购买链接：
 👉 https://item.taobao.com/item.htm?ft=t&id=895964393739

包括主板、扩展板、TypeC数据线、说明文档等内容。

### ❓ 2 .常见问题答疑（FAQ）

为了帮助用户更快上手和排查问题，我们整理了使用 MicroLink 脱机下载功能时的一些常见问题与解答：

| 问题                                 | 解答                                                         |
| ------------------------------------ | ------------------------------------------------------------ |
| 💡 FLM 文件从哪里获取？               | 可从 Keil MDK 安装目录（通常在 `Keil\ARM\Flash`）或芯片厂商提供的 Pack 包中提取 |
| 🔄 脱机脚本可以烧录多个文件吗？       | 可以，Python 脚本中你可以按需加载多个 bin 文件、写入不同地址，还可操作多个 Flash 区域。 |
| 🧪 烧录失败了怎么办？可以调试脚本吗？ | 可以。你可以通过串口连接 MicroLink，使用 `load.offline()` 命令手动触发脚本执行，并通过输出调试信息来定位问题。 |
| 📦 脱机脚本能控制哪些外设？           | 可以控制 GPIO、电平翻转、蜂鸣器响声、串口交互、延时、打印日志等，用于丰富脱机过程的交互与状态反馈。 |
| 🚦 如何判断烧录成功？                 | 烧录完成后 MicroLink 会自动鸣响蜂鸣器作为提示，也可以在脚本中自定义 LED 灯亮灭或串口发送完成信号。 |