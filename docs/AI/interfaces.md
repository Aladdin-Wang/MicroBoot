# Skill、MCP、CLI 与 GUI

四个入口共享同一套 MKLink 能力，但服务对象不同。

| 入口 | 谁使用 | 适合任务 |
|---|---|---|
| Skill | AI | 决定何时调用什么能力、如何验证和处理风险 |
| MCP | AI | 结构化连接、内存、变量、RTT、HardFault、SystemView 等原子操作 |
| CLI | AI 或工程师 | 无 MCP 环境、项目初始化、报告生成和脚本化工作流 |
| Web GUI | 工程师 | 配置、可视化、在线/脱机烧录和人工复核 |

## 推荐选择

有 MCP 的 AI 环境优先使用 MCP；无 MCP 时使用 `python -m mklink`。Web GUI 适合人工观察和确认，不是 AI 获取结构化硬件数据的唯一入口。

固件下载单独遵循：IDE 原生下载优先，其次在线烧录/pyOCD，最后才是脱机部署。不要因为 MCP 可用就绕过工程既有 IDE 配置。

## 同一工程如何协作

Web GUI 和 AI 应使用同一个工程根目录、同一份 AXF/ELF 和 MAP。设备资源有互斥保护：RTT、SystemView、SuperWatch 或烧录占用探针时，另一入口会提示当前 owner。先正常停止当前会话，再交接设备。

## MCP 的典型顺序

```text
ping -> discover_probes -> connect(axf=...)
     -> read_variable / rtt_start / check_hardfault
     -> disconnect
```

## CLI 的典型顺序

```powershell
python -m mklink project-init
python -m mklink project-info
python -m mklink rtt --duration 10
python -m mklink systemview-analyze --duration 6
```

命令只是入口。完成任务的标准始终是拿到可验证的硬件结果，而不是命令返回零。
