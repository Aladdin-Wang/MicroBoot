# 官方下载中心

该页面部署到现有 `https://download.microkeenai.com/` 静态服务，独立于官网首页产物和自动更新协议。

## 构建

运行 `python scripts/build_download_center.py`，也会随 `python scripts/build_website.py` 自动构建。输出 `dist-download/`。测试：`python -m unittest discover -s scripts -p 'test_*.py'`。

部署白名单只有：
- index.html
- download-center.css
- download-center.js
- download-catalog.json

只复制这四个文件到下载站根目录。禁止用 `rsync --delete` 或整目录替换；禁止改写 `mklink/preview/latest.json`、`mklink/stable/latest.json`、`mklink/releases/`，不改 Nginx 配置、缓存策略、Git 标签或 Release。不启用 stable 更新索引。0.3.6 仅为 preview。

## 结构化版本目录

`download-catalog.json` 是页面专用目录，不是自动更新接口。`sections.skill`、`sections.software` 是发布列表，`sections.firmware.V2/V3/V4` 分别独立维护。每个系列按正式发布时间/官方版本顺序从新到旧排列；页面取第一个正式版本为最新。V4 的 MicroLink / HPMLink 按 `family` 分开选择，不能相互覆盖。

发布项：version、date（官方未注明时 null）、channel（stable/preview）、notes、source、hardware/family（固件）、files。文件项：name、platform、hardware（如适用）、bytes、sha256、urls（tencent/github/gitee，仅真实存在的链接）。可用 dateNote 解释未公开日期；不能用资产上传日期冒充原始发布日期。

后续发布时，使用官方 release/校验清单或实测完整文件生成目录：
1. 核对正式发布状态与硬件/系列；不把 preview 转为 stable，除非正式发布已完成。
2. 下载实际文件并计算 SHA-256、长度，与官方值对比；验证每个镜像是否为同一文件。缺失或无法核实的地址不写入 urls。
3. 将有证据的新版本按系列插入列表首部，保留历史项和来源；补齐 generatedAt、missingResources。
4. 构建、运行测试，验证浏览器版本选择及实际下载，再只部署上述白名单文件。页面静态资源修改时同步更新 index.html 中的版本查询值；目录发布时同步更新 JS 的目录查询值，不改服务器缓存规则。

正式发布、固件日期、镜像缺失等限制列于目录 missingResources，页面会公开展示。GitHub 和 Gitee 仓库入口不是二进制镜像，不冒充文件下载。
