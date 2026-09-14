# 安装 MKLink Skill，开始用 AI 调试

想让 AI 帮你编译、下载和读取板上数据，先把完整的 MKLink Skill 装进正在使用的 AI 开发环境。

## 直接把下面这段话交给 AI

> 请从仓库 [https://github.com/Aladdin-Wang/Mklink-AI-Probe](https://github.com/Aladdin-Wang/Mklink-AI-Probe) 或者 [https://gitee.com/Aladdin-Wang/Mklink-AI-Probe](https://gitee.com/Aladdin-Wang/Mklink-AI-Probe) 安装或更新 MKLink AI Probe。读取仓库内的 `Mklink-AI-Probe/SKILL.md` 和完整安装说明，将完整 Skill 安装到当前用户的用户级 Skill 目录，不能只复制 SKILL.md。安装 Web GUI、MCP 以及运行 MKLink 所需的全部依赖，并运行依赖导入检查、Skill 自检和 MKLink 自检。
>
> 然后执行 `python -m mklink web-entry install --quick-launch` 生成统一的快速启动网页：优先保存到卷标为 MICROKEEN 的下载器 U 盘，没有检测到下载器时保存到用户桌面。
>
> 最后启动 Web GUI，等待后端健康后打开真实 Web GUI 页面，确认页面显示“后端正常”和当前后端端口，并向我报告 Skill 版本、源码提交、自检结果、快速启动网页位置和 Web GUI 地址。若启动失败，请继续检查并补齐依赖，不要在只生成网页或只打开启动页时结束。

AI 会按当前客户端的要求完成安装和检查。使用官方完整发布包，不要只复制一份 SKILL.md；客户端需要的依赖和工具配置也要一起准备好。

安装后开启新会话，再说：

> 用 MKLink 看看接着什么板子，确认能连接。

安装包和完整 Skill 的下载入口见[官方发布页](https://github.com/Aladdin-Wang/Mklink-AI-Probe/releases)。只想先手动操作，可以安装桌面应用；它自带运行环境，不需要另外配置 Python。桌面应用与 AI Skill 分别安装，装好桌面应用不等于 AI 已经接入工具。

## 怎样算装好了？

- AI 能识别 MKLink 工具，并报告连接结果。
- 打开的是有“配置、仪表盘、在线烧录、脱机烧录”的实际界面。
- 页面显示“后端正常”；接好下载器与目标板后可以连接设备。

如果只打开了启动页，继续让 AI 检查本地服务。找不到下载器时，先检查 USB 数据线和目标供电。

## 以后怎么更新

> 检查 MKLink 有没有新版本，告诉我更新了什么。

每个会话首次加载 Skill 时会重新检查公开版本；断网不影响继续使用已安装的工具。发现新版后，由你决定是否安装，正在进行的下载或采集不会因此被打断。更新完成后开启新会话。

## 下一步

[把自己的工程交给 AI](first-project.md)，从一次编译、下载和运行检查开始。遇到问题看[连接与使用排障](safety.md)。
