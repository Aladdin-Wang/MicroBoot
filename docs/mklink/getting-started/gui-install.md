# 安装 MKLink，打开网页开始用

MKLink 提供桌面应用和浏览器中的 Web GUI。想先手动使用，安装桌面应用；想让 AI 操作设备，再安装配套 Skill。

## 使用桌面安装包

从[官方发布页](https://github.com/Aladdin-Wang/Mklink-AI-Probe/releases)下载适合系统的安装包。桌面应用自带后端和网页资源，**不需要另外安装 Python 或编译前端**。

安装后连接 MKLink，打开应用，在“配置”中选择设备和符号文件。设备连接与工程配置见[配置第一个工程](project-config.md)。

## 让 AI 帮你安装

> 请从仓库 [https://github.com/Aladdin-Wang/Mklink-AI-Probe](https://github.com/Aladdin-Wang/Mklink-AI-Probe) 或者 [https://gitee.com/Aladdin-Wang/Mklink-AI-Probe](https://gitee.com/Aladdin-Wang/Mklink-AI-Probe) 安装或更新 MKLink AI Probe。读取仓库内的 `Mklink-AI-Probe/SKILL.md` 和完整安装说明，将完整 Skill 安装到当前用户的用户级 Skill 目录，不能只复制 SKILL.md。安装 Web GUI、MCP 以及运行 MKLink 所需的全部依赖，并运行依赖导入检查、Skill 自检和 MKLink 自检。
>
> 然后执行 `python -m mklink web-entry install --quick-launch` 生成统一的快速启动网页：优先保存到卷标为 MICROKEEN 的下载器 U 盘，没有检测到下载器时保存到用户桌面。
>
> 最后启动 Web GUI，等待后端健康后打开真实 Web GUI 页面，确认页面显示“后端正常”和当前后端端口，并向我报告 Skill 版本、源码提交、自检结果、快速启动网页位置和 Web GUI 地址。若启动失败，请继续检查并补齐依赖，不要在只生成网页或只打开启动页时结束。

详细说明见[安装 MKLink Skill](../../embedded-ai/getting-started/skill.md)。Skill 是 AI 的操作配套；桌面应用和 Skill 可以按需要分别安装。

## 打开浏览器版

使用安装时生成的快速启动网页，或让 AI 打开 Web GUI。快速入口通常保存在下载器 U 盘或用户桌面，按安装结果提供的位置打开即可。

启动后应该看到 **配置、仪表盘、脱机烧录、在线烧录**，以及“后端正常”的状态。端口以本次启动地址为准，不用记固定端口。

![MKLink 配置页面](../../images/mklink/cases/hpm5301/web-config-device.png)

## 遇到启动问题

| 现象 | 怎么处理 |
| --- | --- |
| 点击入口没反应 | 允许浏览器打开本地应用；仍无反应时让 AI 修复启动入口 |
| 一直停在启动页 | 让 AI 检查本地服务，不只重新打开网页 |
| 旧地址打不开 | 使用本次启动生成的地址 |
| 页面正常但设备不在线 | 检查数据线、供电与是否被其他工具占用 |

> 网页没有正常打开，帮我检查安装和本地服务，修好后再打开。

[下一步：配置第一个工程](project-config.md)
