# 安装和更新完整 Skill

## 推荐方式：让 AI 自动安装

> "请从仓库 https://github.com/Aladdin-Wang/Mklink-AI-Probe或者https://gitee.com/Aladdin-Wang/Mklink-AI-Probe安装或更新 MKLink AI Probe。读取仓库内的 Mklink-AI-Probe/SKILL.md 和完整安装说明，将完整 Skill 安装到当前用户的用户级 Skill 目录，不能只复制 SKILL.md。安装 Web GUI、MCP 以及运行 MKLink 所需的全部依赖，并运行依赖导入检查、Skill 自检和 MKLink 自检。然后执行 python -m mklink web-entry install --quick-launch 生成统一的快速启动网页：优先保存到卷标为 MICROKEEN 的下载器 U 盘，没有检测到下载器时保存到用户桌面。最后启动 Web GUI，等待后端健康后打开真实 Web GUI 页面，确认页面显示“后端正常”和当前后端端口，并向我报告 Skill 版本、源码提交、自检结果、快速启动网页位置和 Web GUI 地址。若启动失败，请继续检查并补齐依赖，不要在只生成网页或只打开启动页时结束。"

AI 会根据自己的产品和操作系统选择正确的 Skill 目录，完成下载、依赖安装和验证。

## 自动检查更新

每个 AI 会话第一次实际使用 MKLink 能力时，Skill 会检查公开发布清单。默认缓存 24 小时，优先访问 GitHub，失败时尝试 Gitee；断网不会阻塞当前硬件任务。

发现新版本时，AI 应报告当前版本、最新版本和发布说明，并取得确认后更新。已经开始的烧录或调试会话不会被更新检查打断，也不会静默覆盖用户环境。

手动检查：

```powershell
python <skill-root>\scripts\skill_update.py check --json
```

确认后安装：

```powershell
python <skill-root>\scripts\skill_update.py install --yes --json
```

更新 Skill 后开启新 AI 会话。由 Git 管理的开发工作区不会被自动覆盖。

## 安装验收

- 依赖导入输出 `MKLink dependencies OK`；
- Skill 自检和 MKLink 自检通过；
- 快速启动网页已生成；
- 双击网页后进入真实 Web GUI；
- 页面底部显示“后端正常”和实际端口。

常见问题见[安全边界与排障](safety.md)。
