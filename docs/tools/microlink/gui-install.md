# 安装上位机与快速启动

## 本页目标

在 Windows 上安装完整的 MKLink AI Probe 运行环境，并通过快速启动网页进入真实 Web GUI。Web GUI 同时服务人工操作和 AI Skill，不是一个静态演示页面。

## 准备清单

- Windows 10 或 Windows 11；
- Python 3.9 或更高版本；
- MKLink USB 数据线；
- 可访问 [GitHub 仓库](https://github.com/Aladdin-Wang/Mklink-AI-Probe) 或 [Gitee 镜像](https://gitee.com/Aladdin-Wang/Mklink-AI-Probe)。

## 推荐安装方式

可以直接这样告诉 AI：

"请从仓库 https://github.com/Aladdin-Wang/Mklink-AI-Probe或者https://gitee.com/Aladdin-Wang/Mklink-AI-Probe安装或更新 MKLink AI Probe。读取仓库内的 Mklink-AI-Probe/SKILL.md 和完整安装说明，将完整 Skill 安装到当前用户的用户级 Skill 目录，不能只复制 SKILL.md。安装 Web GUI、MCP 以及运行 MKLink 所需的全部依赖，并运行依赖导入检查、Skill 自检和 MKLink 自检。然后执行 python -m mklink web-entry install --quick-launch 生成统一的快速启动网页：优先保存到卷标为 MICROKEEN 的下载器 U 盘，没有检测到下载器时保存到用户桌面。最后启动 Web GUI，等待后端健康后打开真实 Web GUI 页面，确认页面显示“后端正常”和当前后端端口，并向我报告 Skill 版本、源码提交、自检结果、快速启动网页位置和 Web GUI 地址。若启动失败，请继续检查并补齐依赖，不要在只生成网页或只打开启动页时结束。"

AI 会根据自己的产品和操作系统选择正确的 Skill 目录，完成下载、依赖安装和验证。



## 打开 Web GUI

双击 `MKLink Web GUI.html`。文件优先生成到卷标为 `MICROKEEN` 的下载器 U 盘；未检测到 U 盘时生成到当前用户桌面。

启动页会调用当前用户安装的 `mklink-ai-probe://` 协议，等待本地后端健康后再打开真正的 Web GUI。进入 GUI 后，底部状态栏应显示“后端正常”和本次启动的端口。

![Web GUI 后端状态](../../images/microlink/gui/backend-status.png)

!!! note "端口不是固定配置"
    常见地址是 `http://127.0.0.1:8765`。端口被占用时可能选择其他端口，请以页面底部显示的实际端口为准。

## 成功判据

- 页面不是倒计时启动页；
- 顶部可见“配置、仪表盘、脱机烧录、在线烧录”；
- 页面底部显示“后端正常 · 端口号”；
- “配置”页面能刷新本地串口列表。

## 失败处理

| 现象 | 处理 |
|---|---|
| 点击网页没有反应 | 重新执行 `python -m mklink web-entry install`，并允许浏览器打开外部协议 |
| 页面提示启动超时 | 补装 `.[gui,mcp]` 依赖并确认 `gui/dist/index.html` 存在 |
| 只看到启动页 | 等待后端健康；仍未跳转时不要把它视为安装成功 |
| 地址打开但没有 Web 页面 | 关闭不包含 Web assets 的旧 API 进程，再重新启动 |

下一步：需要为产品增加专用页面时，阅读[用 AI 定制企业专属上位机](custom-web-gui.md)；直接使用官方界面时，继续[配置第一个工程](project-config.md)。
