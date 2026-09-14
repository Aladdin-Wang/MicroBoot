# 2026-09-10 文档与图片整理记录

已直接整理 `E:\software\MicroBoot\docs`，尚未提交 Git、推送或部署。官网样稿所在 worktree 不受本次源文件移动影响。

## 完成情况

| 项目 | 结果 |
| --- | --- |
| 原有文档 | 71 篇全部保留；70 篇移动，根目录 MicroBoot 简介保持原地址 |
| 正文与草稿 | 18 篇空白、占位或提纲归入 `docs/drafts/`，标记待补充；新增一页草稿目录 |
| 图片与绘图源文件 | 191 份归类保留；另有 10 份编辑器备份移到发布目录外 |
| 文件移动 | 新旧路径表共 270 条 |
| 原始素材完整性 | 200 份被移动的图片、绘图源文件、备份逐一校验，字节未改变 |
| 旧文档网址 | 71 条跳转，包括原有已失效的 HPM 教程入口；保留 URL 锚点 |
| 未引用素材 | 90 份全部保留，不因“当前没引用”而删除 |
| 相同文件 | 未发现 SHA-256 完全相同的素材；未合并外观相似的不同截图 |
| 用途待确认 | 初筛 28 份，逐图识别 23 份后，余下 5 份 |

“未引用”只表示当前 Markdown 页面没有引用，不表示无用；它们可能是历史教程、待写内容或官网候选素材。绘图源文件也常常不会被页面直接引用。

## 现在如何找资料

- [分类与命名指南](../../DOCS_GUIDE.md)
- [全部素材索引](assets.html)：在浏览器打开，可搜索、筛选待确认项、点击查看原图。
- [素材清单 CSV](assets.csv)：UTF-8 BOM，便于 Excel 查看，含尺寸、大小、引用页面和旧名称。
- [新旧路径 CSV](path-map.csv) / [JSON](path-map.json)
- [完整检查结果](audit.json)

几个常用路径的变化：

| 原路径（相对 docs） | 新路径 |
| --- | --- |
| `tools/microlink/microlink.md` | `mklink/overview.md` |
| `tools/microlink/offline_download.md` | `mklink/flashing/offline-flash.md` |
| `tools/microlink/RTT_printf.md` | `mklink/observation/rtt.md` |
| `AI/AI.md` | `embedded-ai/overview.md` |
| `AI/hpm5301-ai-debug-article.md` | `embedded-ai/cases/hpm5301-debug-session.md` |
| `images/microlink/MKLink V3.png` | `images/mklink/products/mklink-v3-front.png` |
| `images/microlink/MKLinkV3 LCD.jpg` | `images/mklink/products/mklink-lcd-angle.jpg` |

## 之后需要补充什么

### 5 份用途待确认素材

| 素材 | 需要确认 |
| --- | --- |
| [armlink.png](../../docs/images/pending-review/armlink.png) | 图片写有“ARM 仿真器”；与当前 MKLink 产品的型号/历史关系 |
| [armlink1.jpg](../../docs/images/pending-review/armlink1.jpg) | 电路板的具体型号、版本和对应产品 |
| [jscope.jpg](../../docs/images/pending-review/jscope.jpg) | 内容与原文件名 jscope 不一致，确认原本用途 |
| [microlink.pack.png](../../docs/images/pending-review/microlink.pack.png) | 二维码指向哪里、是否仍有效 |
| [sjtb.png](../../docs/images/pending-review/sjtb.png) | 淘宝商品二维码具体指向与是否继续使用 |

其他需要补充的内容：

- 原文件名含 V3 LCD 的图片，在当前产品文档中按 V4 使用。新文件名暂不写型号，原文标注保留，等确认归属。
- 官网用的高分辨率产品实拍、接口特写与真实演示视频。
- 18 篇待补充资料，包括 RT-Thread 移植、部分组件、贡献指南、Arm-2D/FSM/PLOOC 等。原有内容完整保留，没有编造教程。

### 备案状态

据用户 2026-09-10 的腾讯云截图：腾讯云审核已通过，资料已提交管局，工信部短信核验已完成，当前为管局审核中。尚未确认备案通过。此次未连接或修改服务器。

## 验证与兼容

- `python -m mkdocs build --strict` 通过。
- `python scripts/audit_docs.py --migration` 通过：检查 6,862 个本地链接，0 个失效目标、0 个缺失锚点。
- 对比整理前 ZIP 中的所有 Markdown，所有代码围栏内容保持一致。
- 图片文件未重绘、压缩、改格式或丢弃；当前预览索引直接引用原文件。
- `mkdocs-redirects` 原本已列在 `docs/requirements.txt`，本次启用该插件生成旧文档跳转；Read the Docs 入口配置保留。
- 旧图片路径只在 `site/` 构建产物中生成兼容副本，源文件不重复维护。

本机回退备份为此目录下的 `before-2026-09-10.zip`，已忽略，不参与提交。迁移脚本有目标路径检查和重复执行保护；日常补充只需按指南编辑内容后重新构建。

官网合并时请使用 `path-map.json` 更新素材与文档引用。当前官网 worktree 保留它自己的旧文档副本，不能只复制官网目录后假设原图路径仍存在。
