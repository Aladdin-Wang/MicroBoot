# 常见问题

## Web GUI

### 页面没有显示“后端正常”

确认打开的是后端健康后跳转的真实 Web GUI，而不是 `MKLink Web GUI.html` 启动页。重新安装 `.[gui,mcp]`，检查 `gui/dist`，再执行：

```powershell
python -m mklink web-entry install --quick-launch
```

端口以页面底部为准，不要假设总是 8765。

## 设备连接

### Keil 无法识别下载器

1. 在设备管理器确认 CMSIS-DAP 设备正常；
2. 关闭可能占用探针的 Web GUI、AI、pyOCD 和其他 IDE；
3. 重新插拔 USB 数据线；
4. 更新 Keil，建议不低于 5.29；
5. 仍异常时卸载设备驱动并重新枚举。

![Keil 未找到下载器](../../images/microlink/no_link.png)

### Web GUI 找不到 MKLink

检查 USB 数据线是否支持数据、目标板是否供电，以及虚拟串口是否被其他程序占用。不要公开完整探针 ID 或 COM 号。

## 烧录

### 工程里出现多个芯片型号

停止烧录，核对板上芯片丝印、当前 IDE Target、编译宏、Flash Algorithm 和链接布局。目录名不能作为器件型号证据。

### BIN 应填写什么地址

填写链接脚本定义的镜像起始地址。普通 STM32 应用常见为 `0x08000000`，带 BootLoader 的 App 往往不是这个地址。无法从 MAP、链接脚本或发布说明确认时不要烧录。

### 提示资源被占用

先停止 RTT、SystemView、SuperWatch、在线烧录或其他持有探针的任务。页面显示“停止中”时等待底层操作返回并释放资源。

### 脱机任务部署成功，但按键没有烧录

部署成功只表示脚本、固件和 FLM 已写入 `MICROKEEN`。先在 Web GUI 使用“触发测试”，确认日志包含 `loaded success` 和 `auto download finished`；再检查下载器型号、脚本名、目标供电、IDCODE、按键或机台触发接线。完整流程见[脱机下载与量产](offline_download.md)。

### 脱机烧录显示 100%，程序仍未运行

核对 HEX 内部地址或 BIN 基址、Flash Algorithm、复位方式、BootLoader 跳转和选项字节。随后用 RTT、版本变量或通信响应验证目标运行；百分比到 100% 不能替代运行验收。

## RTT 和符号

### RTT 自动搜索失败

确认固件初始化 RTT、ELF/MAP 匹配当前固件，并检查 `_SEGGER_RTT` 是否被堆栈覆盖。必要时从 MAP 获取地址。

### 变量不存在或值异常

确认变量没有被优化掉、ELF 包含 DWARF、地址在 SRAM，并确保 ELF 和固件来自同一次构建。

## SystemView

### 能找到 RTT，但没有 RTOS 事件

检查 SystemView 宏、RT-Thread hook list、通道 1和目标是否运行。普通 RTT 日志正常不代表 SystemView 已完成集成。

### 有时间线，但任务数为 0 或出现 overflow

这次采集无效，不要解读 CPU 占用。检查当前 ELF、`task_start_exec/task_stop_exec` 适配、72 MHz 等时间基准和 SystemView 缓冲；目标端 `Trace overflow` 必须消除后重采。GUI 的 `Runtime Drop` 是前端显示队列统计，应与目标 overflow 分开判断。

## 固件升级与资料

### 下载器固件和目标 MCU 固件有什么区别

下载器固件用于更新 MKLink 自身，V2 使用 `.rbl`，V3/V4 使用 `CHERRYUF2` 下的 `.uf2`。目标 MCU 固件是应用的 HEX/BIN，通过在线或脱机烧录写入芯片。两类文件不能混用，详见[固件升级](firmware-upgrade.md)。

### 从哪里下载软件、固件和 FLM

统一入口见[资料下载](downloads.md)。生产使用前记录下载来源、版本、文件大小和 SHA-256；升级包必须与下载器型号匹配，FLM 必须覆盖目标器件和固件地址范围。

## 功能与文档不一致

读取下载器固件版本和 Skill 版本。Skill 默认每 24 小时检查一次公开版本，GitHub 失败时使用 Gitee；固件仍需按对应型号的升级说明更新。
