# MicroKeen 官网

官网首页与 MkDocs 文档共用这个仓库。首页包括产品型号切换、SuperWatch 实机波形与性能、AI 使用场景、常用工具、安装下载与技术支持。支持中英文首页、手机导航、键盘切换标签和截图放大；文档与原始截图保留中文。

## 构建与预览

在仓库根目录执行，Windows 优先使用 PowerShell 7：

```sh
python -m pip install -r scripts/requirements-website.txt
python scripts/build_website.py
python scripts/preview_website.py
```

打开 <http://127.0.0.1:4173/>；文档位于 `/docs/`。预览只监听本机，端口占用可加 `--port 4174`。

只修改首页时，可以运行 `python scripts/build_website.py --website-only`，再刷新页面。首次构建或修改文档后必须完整构建。网站产物为 `dist/`，不提交 Git；服务器只需提供静态文件。

## 编辑入口

| 文件 | 用途 |
| --- | --- |
| `index.html` | 默认中文内容、结构、SEO、备案页脚 |
| `content.js` | 英文翻译、型号资料、AI 提示词与案例链接 |
| `app.js` | 型号、案例、语言、菜单、截图交互 |
| `styles.css` | 电脑与手机布局 |
| `assets.json` | 指向文档原图的清单；构建时派生 WebP |
| `mkdocs.website.yml`（根目录） | 自有域名 `/docs/` 构建，保留原 Read the Docs 配置 |

产品照片保持原有外观和尺寸，不生成虚构硬件。性能条件与案例一起维护，不把单变量采集率作为多通道界面刷新率。文档保留原路径、重定向和图片兼容副本。

## 发布前验证

```sh
python -m mkdocs build --strict
python scripts/audit_docs.py
python scripts/build_website.py
```

完整构建检查首页资源、锚点、文档目标和 canonical。GitHub Actions 在主分支推送和 PR 时执行相同检查，生成 `microkeen-website` 静态产物，不自动操作服务器。

部署与后续维护见根目录 [HANDOFF_DOT.md](../HANDOFF_DOT.md)。
