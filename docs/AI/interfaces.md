# AI 的 Skill、MCP、CLI 与 GUI

MKLink 的四个入口共享底层能力，但使用者和用途不同。

| 入口 | 使用者 | 作用 |
|---|---|---|
| Skill | AI | 读取操作规则，选择能力，处理风险并定义验收证据 |
| MCP | AI | 用结构化参数连接设备、读取变量、RTT、Fault 和 SystemView |
| CLI | AI / 工程师 | 无 MCP 环境、项目初始化、批量采样和报告生成 |
| Web GUI | 工程师 | 手动配置、可视化曲线、烧录和人工复核 |

## AI 如何选择入口

有 MCP 时优先调用 MCP；当前环境没有 MCP 时，使用 `python -m mklink`。Web GUI 供工程师手动配置和观察曲线。

固件下载另有优先级：工程存在且 IDE 已安装时，先用 Keil/IAR 原生命令行；只有预编译镜像或 IDE 不适用时才使用在线下载工具；量产和无电脑场景使用脱机任务。已经开始的下载路径失败后，应先保留并解释错误，不能静默切换后端。

## MCP 典型流程

```text
ping -> discover_probes -> connect(axf=本次构建产物)
     -> read_variable / capture_rtt / check_hardfault
     -> disconnect
```

MCP 返回结构化字段，AI 应保留关键原始值，而不是只输出一句“正常”。

## CLI 典型流程

```powershell
python -m mklink project-init
python -m mklink project-info
python -m mklink rtt --duration 10
python -m mklink systemview-analyze --duration 6
```

具体参数由 AI 根据工程、设备和当前 Skill 版本生成，不建议把文档中的端口或地址原样复制到其他工程。

## AI 与 GUI 交接

两者可以使用同一工程和同一份 AXF/ELF、MAP，但不能同时占用探针。切换入口前，应正常结束 RTT、SystemView、SuperWatch 或烧录会话，再释放设备。若提示 owner 冲突，先查清占用者，不要强制启动第二个作业。

GUI 的完整人工操作从 [MKLink 产品与功能总览](../tools/microlink/microlink.md) 进入；AI 日常调试沿本章后续教程执行。
