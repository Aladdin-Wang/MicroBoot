# MKLink AI Probe Skill 安装与使用

MKLink AI Probe Skill 可以让 AI 直接调用 MKLink 下载器完成编译、烧录、RTT 日志、变量监控、内存和寄存器读取、HardFault 分析等工作。

普通开发者不需要先学习命令，也不需要自己查找 AI 的 Skill 安装目录。最简单的方式是把仓库链接交给 AI，让 AI 自动安装。

## 一、推荐安装方式：让 AI 自动安装

将下面任意一个仓库链接发给支持本地文件和终端操作的 AI 编码助手：

- GitHub：<https://github.com/Aladdin-Wang/Mklink-AI-Probe>
- Gitee：<https://gitee.com/Aladdin-Wang/Mklink-AI-Probe>

国内网络访问 GitHub 较慢时，可以使用 Gitee 地址。

然后直接对 AI 说：

> 请从这个仓库安装 MKLink AI Probe。读取仓库中的 `Mklink-AI-Probe/SKILL.md` 和安装说明，把完整 Skill 安装到你的用户级 Skill 目录，同时安装 Web GUI 和 MCP 所需的运行环境。安装完成后执行自检，并告诉我是否可以正常使用。

AI 会根据当前使用的 Codex、Claude Code、Cursor 或其他编码助手，自动选择对应的 Skill 目录。不同 AI 的安装目录并不相同，因此不要照抄其他电脑的用户名和绝对路径。

安装时必须保留完整的 `Mklink-AI-Probe` 目录，其中包含：

- `SKILL.md`：告诉 AI 什么时候调用什么能力；
- `mklink/`：实际操作 MKLink 的程序；
- `gui/`：Web GUI 和 Windows 上位机资源；
- `references/`：烧录、RTT、调试和故障排查说明。

只复制一个 `SKILL.md` 文件不能完成硬件操作。

## 二、手动安装（备用方式）

只有在 AI 无法自动安装时，才需要手动处理。

1. 从 GitHub 或 Gitee 下载完整仓库并解压。
2. 找到内层包含 `SKILL.md`、`pyproject.toml`、`mklink/` 和 `gui/` 的 `Mklink-AI-Probe` 目录。
3. 将这个完整目录放入当前 AI 产品的用户级 Skill 目录。
4. 在该目录中安装运行环境：

```bash
python -m pip install -e ".[gui,mcp]"
```

需要 Python 3.9 或更高版本。安装完成后重启或重新加载 AI 编码助手。

以 Codex 为例，用户级目录通常是：

```text
%USERPROFILE%\.codex\skills\mklink-ai-probe
```

其他 AI 产品的目录可能不同，请以对应产品的 Skill 文档为准。

## 三、第一次使用

将 MKLink 连接到电脑和目标板，然后把工程目录告诉 AI：

> 我的嵌入式工程在 `工程目录`。请使用 MKLink 检查工程、识别 Keil 或 IAR 配置和目标芯片，先告诉我准备执行哪些操作，确认后再编译和下载。

固件下载默认按以下顺序选择：

1. 优先使用工程已有的 IDE，例如调用 Keil 编译并直接下载；
2. 没有可用 IDE 时，使用 pyOCD 在线烧录；
3. 最后才使用 MKLink 脱机下载方式。

如果某一种方式已经开始执行但失败，AI 应停止并说明原因，不应静默换一种方式继续写入芯片。

## 四、常用说法

不需要记忆 MKLink 命令，直接告诉 AI 你想得到什么结果。

### 编译并下载程序

> 请使用工程里的 Keil 配置编译程序，确认零错误后通过 Keil 下载到目标板，并验证程序已经运行。

### 打开 Web GUI

> 请启动 MKLink AI Probe 的 Web GUI，并使用我的工程目录。

浏览器默认打开：

```text
http://127.0.0.1:8765
```

Web GUI 可以进行工程配置、在线/脱机烧录、RTT View、SystemView、SuperWatch、内存查看、串口和 Modbus 调试。

### 查看 RTT 日志

> 请连接目标板，查找 RTT 控制块并采集 10 秒日志，发现异常时结合源码分析原因。

### 连续观察变量

> 请从 AXF 文件中找到这些变量，使用 SuperWatch 连续观察 10 秒，并保存带时间戳的原始数据。

### 分析 HardFault

> 请读取 HardFault 寄存器和异常栈，结合 AXF 文件定位到函数和源码行，并给出可以验证的修复建议。

### 检查外设状态

> 请读取 DMA、ADC 和相关中断寄存器，判断 DMA 是否还在传输，并用实时变量或内存数据验证结论。

## 五、使用时需要注意

- AI 必须运行在能够访问本机文件和执行命令的环境中，普通网页聊天窗口不能直接操作本机下载器。
- 烧录、写内存、修改寄存器和复位目标板都会改变硬件状态。涉及这些操作时，应先让 AI 说明步骤。
- 下载过程中不要拔掉 MKLink、目标板电源或 USB 线。
- AXF/ELF 的符号和 HardFault 解析默认使用内置解析器，不要求额外安装 GNU Arm 工具链。
- Web GUI 默认只允许本机访问，不要直接暴露到公网。
- 如果更换了 Skill 安装目录，应让 AI 重新检查安装路径和运行环境。

## 六、常见问题

### AI 提示没有找到 Skill

确认安装的是完整目录，然后重启或重新加载 AI。也可以把仓库链接再次发给 AI，让它检查自己的用户级 Skill 目录。

### 提示找不到 `mklink` 模块

进入包含 `pyproject.toml` 的内层 `Mklink-AI-Probe` 目录，重新执行：

```bash
python -m pip install -e ".[gui,mcp]"
```

### Web GUI 无法打开

让 AI 检查 GUI 依赖、`gui/dist/index.html`、本机端口占用和后台进程。正常启动地址为 `http://127.0.0.1:8765`。

### 找不到 MKLink

检查 USB 数据线、目标板供电和设备管理器，卸载MKlink驱动，并重新拔插，关闭可能正在占用 MKLink 的其他上位机、串口工具或调试软件。

### 需要进一步了解功能

查看项目说明：<https://github.com/Aladdin-Wang/Mklink-AI-Probe#readme>
