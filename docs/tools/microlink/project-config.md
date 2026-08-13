# 配置第一个工程

> 本页介绍 Web GUI 手动配置。工程自动识别和检查见[第一次让嵌入式 AI 接手工程](../../AI/first-project.md)。

## 本页目标

让 Web GUI 知道工程目录、MKLink 设备以及 AXF/ELF、MAP 文件的位置，为 RTT、符号变量、内存和 SystemView 共用同一份配置。

## 连接硬件

连接`VCC`、 `SWDIO`、`SWCLK`、`GND`，需要复位控制时再连接 `RST`。目标板已有供电时不要重复从下载器供电；使用 VCC（默认输出3.3V） 输出前先确认目标板允许的电压，可以通过配置文件`MICROKEEN(u盘)/Python/default_config.py`设置VCC的电压。

## 选择工程

打开“配置”页面：

1. 在“本地设备”中保持“自动搜索”，或选择确认过的 MKLink 端口；

2. 点击“连接设备”。

   ![HPM5301 本地设备与串口配置](../../images/microlink/hpm5301/web-config-device.png)

3. 选择 AXF/ELF 符号文件和 MAP 文件；

![工程与本地设备配置](../../images/microlink/gui/project-config.png)

配置修改后会自动保存。AXF/ELF 用于变量类型、源码行和 HardFault 解析；MAP 可用于 RTT 控制块地址发现。二者不是固件镜像，不能替代 BIN/HEX 烧录文件。

下一步：[在线编译与烧录](online-flash.md)。
