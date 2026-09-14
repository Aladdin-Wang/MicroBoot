# 文档与素材维护指南

本次整理于 2026-09-10，直接作用于 `E:\software\MicroBoot`。技术正文、示例代码和原始图片保留；调整文件路径、分类、导航与引用，草稿明确标识。

## 内容放在哪里

| 目录 | 内容 |
| --- | --- |
| `docs/index.md` | 原 MicroBoot 简介，保留既有文档首页地址 |
| `docs/mklink/` | MKLink 产品、入门、烧录、观测、故障定位、通信、开发、支持、HPM 生态 |
| `docs/mklink/articles/` | 原理和产品文章，与日常操作教程分开 |
| `docs/mklink/legacy/` | 已拆分或被新版替代的历史入口，保留内容与旧书签 |
| `docs/embedded-ai/` | AI 入门、工作流、真实案例和原理文章 |
| `docs/microboot/` | MicroBoot 移植、配置；简介仍在根首页 |
| `docs/components/` | 已有正文的通用组件：队列、信号槽、多路延时 |
| `docs/knowledge/` | C 语言及通用技术知识 |
| `docs/drafts/` | 空白页、占位文案和未完成提纲；保留原文，等待补充 |

文件和目录使用小写英文与连字符：`offline-flash.md`、`signals-slots.md`。页面标题仍用自然中文。新页面按内容主题放置，避免重新建立 `tools/microlink`、`AI`、`usage-guide` 等旧入口。

## 图片与绘图源文件

| 目录 | 内容 |
| --- | --- |
| `docs/images/mklink/products/` | 产品实拍 |
| `docs/images/mklink/gui/` | 官方 Web GUI 截图 |
| `docs/images/mklink/cases/hpm5301/` | HPM5301 实机案例 |
| `docs/images/mklink/custom-gui/` | 企业定制界面示例 |
| `docs/images/mklink/flashing/` 等主题目录 | 烧录、RTT、串口、SystemView、VOFA、HPM、开发与支持素材 |
| `docs/images/embedded-ai/` | AI 架构与案例图片 |
| `docs/images/microboot/` | 框架原理图和 CMSIS-Pack 操作步骤 |
| `docs/images/components/` | 组件配图 |
| `docs/images/knowledge/` | USB 等通用原理素材 |
| `docs/images/source/` | 可编辑 `.drawio`、`.emmx` 原文件 |
| `docs/images/pending-review/` | 暂时无法确定用途的素材，不删除、不猜测 |

命名建议为“对象-场景-状态”：`ses-gdb-server.jpg`、`rtt-auto-detection.png`、`online-flash-succeeded.png`。连续步骤保留两位编号，例如 `step-01.png`。只有有依据的型号才能进入文件名：原 `MKLinkV3 LCD.*` 暂命名为 `mklink-lcd-angle.jpg` / `mklink-lcd-front.png`，等确认型号后再细分。

上传补充素材时，附上：产品型号/硬件批次、拍摄或截图场景、是否为正式界面、对应教程。高清原图直接保留；官网所需压缩图在构建阶段生成，不覆盖原图。不把显示结果不同的截图当作重复图删除。

## 查看本次整理结果

- [汇总与待补充项](maintenance/docs-organization/README.md)
- [全部素材预览索引](maintenance/docs-organization/assets.html)：浏览器打开，可切换查看待确认素材。
- [新旧文件路径表](maintenance/docs-organization/path-map.csv)
- [机器可读路径表](maintenance/docs-organization/path-map.json)
- [素材清单](maintenance/docs-organization/assets.csv)：新旧路径、尺寸、文件大小、引用和相同文件组。

`before-2026-09-10.zip` 是本机恢复备份，包含整理前完整 `docs/` 和 `mkdocs.yml`，已排除出 Git。编辑器 `.bkp` 文件已移到发布目录外的 `maintenance/docs-organization/editor-backups/`，没有删除。

## 构建与核对

```powershell
python -m pip install -r docs/requirements.txt
python -m mkdocs build --strict
python scripts/audit_docs.py
```

严格构建检查文档配置与页面引用；审计检查移动文件的校验值、旧页面跳转、旧图片地址、构建后的本地链接与图片，并生成可浏览的素材清单。审计不访问外部网站、不验证图片里的性能数字。

`mkdocs.yml` 中的 `redirects` 保留旧文档 URL 与锚点。`scripts/docs_asset_aliases.py` 只在构建产物中生成旧图片路径兼容副本，源目录中不维护两份图片。Read the Docs 仍使用原配置入口。

`plan_docs_organization.py`、`organize_docs.py` 是本次迁移的留档工具，不作为日常维护命令重复运行。以后再改路径时，同步更新引用、重定向和新旧路径表。

## 官网衔接与备案

官网前三屏样稿仍在 `C:\Users\akqbd\.codex\worktrees\33e3\MicroBoot`，未合并到本目录。这次不跨 worktree 覆盖文件；将来合并官网时，依据路径表更新 `website/assets.json` 和页面中的文档目标。旧官网入口已有构建后的兼容跳转。

用户 2026-09-10 提供的腾讯云截图显示：腾讯云审核通过、已提交管局、短信核验完成，目前为管局审核中。尚未收到备案通过确认，本次不修改服务器或公开上线。
