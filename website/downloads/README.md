# 官方下载中心

该页面部署到现有 `https://download.microkeenai.com/` 静态服务，独立于官网首页产物和自动更新协议。

## 构建与验证

```sh
python scripts/sync_download_catalog.py --check
python -m unittest discover -s scripts -p 'test_*.py'
node --test scripts/test_download_center.cjs
python scripts/build_download_center.py
```

`python scripts/build_website.py` 也会构建下载中心。下载站输出 `dist-download/`，白名单只有：
- index.html
- download-center.css
- download-center.js
- download-catalog.json

只复制这四个文件到下载站根目录。禁止用 `rsync --delete` 或整目录替换；不触碰 `mklink/` 下的任何更新索引、发布文件、下载重定向，也不改 Nginx 配置、缓存规则、Git 标签或 Release。`metadata/` 和其他源码不部署。自动更新接口已由维护者独立发布，不由网站构建管理。

代码通过测试并提交 PR 后，只在用户明确授权且现有服务器会话有效时部署上述白名单文件；构建成功不代表线上已更新，须另行核验线上字节和功能。

## 结构化版本目录

`download-catalog.json` 是页面专用目录，不是自动更新接口。它由 `scripts/sync_download_catalog.py` 从 `metadata/` 中的已审阅快照确定性生成：
- `history.json`：原目录中的历史正式版本，完整保留原文件、摘要和已核实地址。
- `stable-latest.json`：正式应用/完整 Skill 索引快照。
- `v0.3.6-release-manifest.json`、`v0.3.6-SHA256SUMS.txt`：七份正式资源中的清单与校验文件原始字节；发布清单列五个载荷，其余两份为清单自身和校验表。
- `firmware-latest.json`、`firmware-latest-v2.json`：独立固件索引快照。
- `verified-sources.json`：本次实际下载并核对大小和 SHA-256 后允许展示的地址，不猜测缺失镜像。

快照只读；同步器不连接网络、不改写上游索引，只生成网站目录。当前明确锁定正式软件 0.3.6 及源提交 `d000370b901a72e4f8ec8fe09cf8b837f2579f0c`，固件 MicroLink V2.9.2、V3.6.7、V4.7.1 和 HPMLink V4.7.0。将来发布需审查版本锁定、快照与校验记录后再运行生成器，不能将未经审核的远程 latest 自动当作正式发布。

`sections.skill`、`sections.software`、`sections.siteAgent` 是独立发布列表；Site Agent 仍位于页面的安装包区域，不另开第四主分区。`sections.firmware.V2/V3/V4` 独立维护；V4 的 MicroLink / HPMLink 按 `family` 分别选择，不能相互覆盖。每个系列第一个条目为最新正式版，其余为历史正式版。没有历史 Site Agent 记录时不猜测补包。

发布项：version、date（官方未注明时 null）、channel（stable）、notes、source、hardware/family/build_id（固件）、files、可选 supportFiles（签名/清单，明确非安装包）。文件项：name、platform、hardware（如适用）、bytes、sha256、urls（tencent/github/gitee，仅已核实链接）。公开时间戳按北京时间 UTC+8 显示，日期型历史值保留原文；固件索引发布日期与无时区 build_id 分开标注，不用资产上传时间冒充原始发布日期。

## 后续发布

1. 核对正式发布状态、源提交、硬件系列、完整资源清单与更新说明。仅本轮七份软件资源和四份新固件新增到目录。
2. 完整下载实际文件，计算 SHA-256 和长度，并核对每个镜像。缺失或无法核实的地址不写入 verified-sources。Windows 安装包超过 Gitee 100 MB 限制，不生成其 Gitee 地址。
3. 更新只读快照、版本锁定和测试，运行 `python scripts/sync_download_catalog.py`，保留历史正式项；不要构建本轮未发布的 Linux/macOS 包。
4. 运行上述回归测试和完整官网构建；检查每个版本选择、手机导航及真实文件下载。修改资源时同步调整页面和 JS 的版本查询值，不改服务器缓存策略。
5. 审查后仅部署四个白名单文件；验证线上字节摘要与构建产物一致。部署前后检查原自动更新接口不变。

历史日期、功能更新说明及缺失镜像的限制列于目录 missingResources，页面会公开展示。GitHub 和 Gitee 仓库入口不是二进制镜像，不冒充文件下载。

## 产品图片

V2、V3、MicroLink V4 和上位机卡片复用官网已发布的真实图片；HPMLink 使用经用户授权、基于 MKLink 参考图生成的外观示意图，并明确标注 AI 生成。不新增下载站图片目录，也不改下载版本目录。图片辅助识别，刷写前仍需核对设备型号：

- V2：`https://microkeenai.com/assets/mklink-v2.webp`，来源 `docs/images/mklink/products/mklink-v2-front.png`。
- V3：`https://microkeenai.com/assets/mklink-v3.webp`，来源 `docs/images/mklink/products/mklink-v3-front.png`。
- MicroLink V4：`https://microkeenai.com/assets/mklink-v4.webp?v=v4-screen-20261007`，来源 `website/assets/mklink-v4-screen-20261007.png`。
- 上位机：`https://microkeenai.com/docs/images/mklink/gui/online-flash.png`，官方 MKLink AI Probe 在线烧录界面示例，不随历史安装包选择冒充对应版本截图。
- HPMLink V4：`https://microkeenai.com/assets/hpmlink-v4-20261025.webp`，来源 `website/assets/hpmlink-v4-20261025.png`，通过 `website/assets.json` 和官网现有构建生成。参考标识保留 `HPMsLink`，固件系列仍为 `HPMLink V4`；图片不是实拍，不作为型号兼容性证据，保留铭牌核对提示。

图片保持自然比例、完整展示，不裁切硬件；硬件图片延迟加载并提供型号 alt 文本。图片展示代码独立于目录和下载链接。仅更换图片界面时部署前三个 UI 文件，`download-catalog.json` 不必重传。


本轮 HPMLink 图片更新须先在官网现有 `assets/` 路径新增且仅新增 `hpmlink-v4-20261025.webp`，核对字节摘要和 HTTP 200，再部署下载站三个 UI 文件。下载站构建的四文件白名单不变，目录 JSON 不重传；不部署整个官网、不改任何服务器配置或下载文件。

## 2026-10-25 固件目录更新

本轮只部署 `download-catalog.json`；三个 UI 文件与全部产品图片保持原字节。应用与 Skill 保持 0.3.6，HPMLink 保持 V4.7.0。新增 MicroLink V4.7.1 / V3.6.7 / V2.9.2，并将 V4.7.0 / V3.6.6 / V2.9.1 原完整条目保留在历史版本中。

修复范围为 Keil 调试时 RTT / SuperWatch 共存。维护者确认三份升版二进制完成软件回归和编译校验，未重新刷机，不宣称三个型号本轮均通过真机验证。网页构建不修改固件、上游索引、下载保护、权限或 Nginx。


## 2026-10-25 AI 调试宣传栏

移除页首三个下载分类快捷按钮与孤立上位机截图，替换为用户已确认的 V6 全宽横幅。桌面图片区域 220px（完整图像约 660px 宽），整体约 259px，完整保留 3:1 图片，不裁切重要内容；手机按图片自然比例展示，不增加高大的图文堆叠。全站导航、两张下载卡片的共享行对齐、四张固件图片、版本目录及下载行为不变。

横幅为 AI 生成的真实案例重排，并非原始操作截图；Codex 风格对话、SuperWatch 三条曲线与 MKLink 展示同一个 RAM 调参案例，机器人背景仅为概念示意，不宣称机器人实测。页面文字与 alt 明确说明。

原图 `website/assets/mklink-codex-dark-banner-20261025-v6.png` 经 `website/assets.json` 和既有官网构建生成同名 WebP，优化文件也保留入库。仅新增该 WebP 到官网现有 `/assets/`，核对 HTTP 状态与摘要，再更新下载站三个 UI 文件；JS 内容不变，目录 JSON 不重传。不得上传原图、源码或整个官网目录，不修改服务器配置。


### V7 关节电机自动化调试展示

按用户要求删除宣传图下方可见说明，准确的概念背景/真实案例重排描述仍保留在 alt 中。两侧文案压缩为小标签与两行，中央图片增加至 800×236px，整体约 260px。V7 原图 2170×725，桌面 `cover` 居中只裁少量上下背景，保留对话、波形、下载器和标签；手机完整自然比例 `contain`。<=1100px 隐藏冗余侧文案，图片保持236px高度上限；<=760px 自然比例。

新增 `website/assets/mklink-joint-motor-automation-banner-20261025-v7.png` 与同名 WebP，经 assets.json 现有构建生成。旧 V6 不覆盖。先仅新增官网同名 WebP 并核对摘要，然后更新下载站 HTML/CSS；JS与目录不变。必须检查实际截图中所有关键内容可见，没有拉伸、横向溢出或过高页面。
