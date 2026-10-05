# 给 Your dot：MicroKeen 官网与文档交接

更新：2026-10-05。用户要求后续网站建设交由 ChatGPT 的 **Your dot** 接手。

## 先读取这些文件

仓库：<https://github.com/Aladdin-Wang/MicroBoot>，分支 `main`。

1. 本文：范围、部署位置和待完成事项。
2. `website/README.md`：官网构建、交互与编辑入口。
3. `DOCS_GUIDE.md`：文档路径、图片和旧链接维护规则。
4. `docs/mklink/overview.md`、`docs/embedded-ai/cases/stm32f103-rt-thread.md`、`docs/mklink/hpm/overview.md`：当前产品与实测数据。

后续请从 GitHub 最新 main 创建工作分支。原本地文档目录是 `E:\software\MicroBoot`；旧 Codex worktree 是九月样稿，不能用它覆盖当前文档。官网样稿已整合到本仓库。

## 已完成

- 独立官网：首页产品展示、V4/V3/V2 切换、真实 SuperWatch 波形、性能摘要、AI 使用提示词和截图、在线/脱机烧录、RTT、Memory/Fault 入口。
- 中文与英文首页、手机菜单、键盘标签操作、截图放大。
- 最新文档打包到 `/docs/`，保留原文档内容、73 个旧页面重定向与旧图片地址兼容。
- 用户近期增加的 STM32 实时写入演示、HPM OTP 脱机流程与 HPM 文章修改一并保留。
- 官网与文档页脚加入用户提供的 **豫ICP备2026045994号**，链接工信部备案查询页。
- canonical、favicon、robots、sitemap；去掉文档对 Google Fonts 的依赖。
- GitHub Actions 严格构建、文档审计和静态发布包产物。

## 网站与服务器

用户已确认备案成功。域名为 `microkeenai.com`，同时配置 `www.microkeenai.com`；主体为洛阳智沐科技有限公司。

九月实际操作记录显示：腾讯云轻量服务器、OpenCloudOS 9、宝塔和 Nginx，静态站点目录 `/www/wwwroot/microkeenai.com`，当时因等待备案设为停止。**本轮未连接服务器，不能据此认定当前 DNS、证书、运行状态或网站已上线。**

构建产物结构：

```text
dist/
  index.html       官网
  app.js / content.js / styles.css
  assets/          由文档原图派生的 WebP
  docs/            MkDocs 全量文档与旧地址兼容文件
  favicon.svg / robots.txt / sitemap.xml / home-sitemap.xml
```

主域名提供根目录官网，`/docs/` 提供文档。Read the Docs 继续使用 `.readthedocs.yaml` 与 `mkdocs.yml`，不强制迁移。

## dot 下一步

1. 拉取 main，安装 `scripts/requirements-website.txt`，执行下面的验证命令；不要沿用旧构建包。
2. 在用户授权的服务器会话中核验域名解析与宝塔站点。访问凭证应通过安全渠道提供，不能写进 GitHub、文章或日志。
3. 备份当前站点和 Nginx 配置，在新的发布目录上传 `dist/` 的**内容**，确认包含 `docs/`；避免把源码、`.git` 或本机测试目录传到公开目录。
4. 通过宝塔配置覆盖主域名和 www 的有效 HTTPS 证书。启用站点；www 统一跳转到 `https://microkeenai.com` 并保留路径和参数。启用 HTTP 到 HTTPS 跳转前先确认 HTTPS 可访问。
5. 使用静态目录和正常 404，不做 SPA 全路径回退到首页；深层文档目录返回其 `index.html`。保留宝塔证书续期配置。
6. 线上检查首页、语言切换、手机菜单、文档搜索、安装指南、下载与支持、RTT 原理、STM32/HPM 案例、旧图片地址、404 和备案链接。确认后记录发布时间和 commit。
7. 更新失败时恢复备份目录及配置。脚本不会删除服务器内容，GitHub Actions 也没有部署凭据。

## 验证命令

```sh
python -m pip install -r scripts/requirements-website.txt
python -m mkdocs build --strict
python scripts/audit_docs.py
python scripts/build_website.py
python scripts/preview_website.py
```

本机审计通过：264 个素材、200 项原图哈希、73 个旧页面重定向、8570 项本地链接，0 错误、0 锚点警告。严格构建和首页资源检查通过。此记录不代表服务器已经发布。

浏览器检查：1440 px 桌面与 390 px 手机视口没有页面横向溢出；型号切换、案例切换、截图弹窗、语言切换和手机导航正常，检查时未发现脚本错误或失效图片。手机尺寸为浏览器模拟，不是手机实机测试。

审计在干净云端检出中会跳过 10 份未入库的本地编辑器备份，并单独记录数量；公开素材仍全部校验。本地有备份时也校验备份。`.drawio` 禁用 Git 换行转换，保留原始字节，避免 Windows 与 Linux 校验结果不同。

## 写作与数据边界

- 官网偏克制的产品展示，文章偏工程师阅读的公众号风格。通过真实问题、短提示词和界面说明用途，不写测试流水账。
- 人只需知道如何开始；CLI/MCP 是 AI 的操作接口，不在日常入门页堆内部 Python 命令。**完整 Skill 安装提示词是用户要求保留的例外**，GitHub 与 Gitee 两个链接都保留。
- RTT 使用原理必须保留原来的讲解、代码和图片；新 GUI 方法可以追加，不能代替原理。
- 不凭旧版本印象修改最新文章。首页性能取自对应文章，分别注明目标、时钟、记录长度或镜像大小。单变量、多变量、持续接收、UI 帧率和下载写入速度不能混为一谈。
- J-Link 对比不再开展新测速，不用克隆设备结果证明正版性能，也不能把 CSV 的 2 ms 导出间隔当作 J-Scope 性能上限。
- 用户确认 MKLink V4 价格 358 元含线材；如后续放到首页，先核对当时售价。不要编造含税信息、销量、认证、客户或兼容范围。
- 最新文档内含开发候选固件功能说明。推广前核对正式软件是否已发布，不把候选功能默认为所有已售设备可用。
- HPM 与 STM32 波形优先使用真机素材，不重画数值或用 AI 生成画面冒充实测。

## 交接状态

交接文件随代码交付 GitHub。当前会话没有可直接给 Your dot 发送任务的专用入口，**不能声称 dot 已收到或已开始工作**。用户可把本文件的 GitHub 链接发给 Your dot，并要求从最新 main 继续建设、部署和维护。
