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

本轮只提交网站代码、测试和 PR，维护者审查后自行部署；构建成功不代表线上已更新。

## 结构化版本目录

`download-catalog.json` 是页面专用目录，不是自动更新接口。它由 `scripts/sync_download_catalog.py` 从 `metadata/` 中的已审阅快照确定性生成：
- `history.json`：原目录中的历史正式版本，完整保留原文件、摘要和已核实地址。
- `stable-latest.json`：正式应用/完整 Skill 索引快照。
- `v0.3.6-release-manifest.json`、`v0.3.6-SHA256SUMS.txt`：七份正式资源中的清单与校验文件原始字节；发布清单列五个载荷，其余两份为清单自身和校验表。
- `firmware-latest.json`、`firmware-latest-v2.json`：独立固件索引快照。
- `verified-sources.json`：本次实际下载并核对大小和 SHA-256 后允许展示的地址，不猜测缺失镜像。

快照只读；同步器不连接网络、不改写上游索引，只生成网站目录。当前明确锁定正式软件 0.3.6 及源提交 `d000370b901a72e4f8ec8fe09cf8b837f2579f0c`，固件 MicroLink V2.9.1、V3.6.6、V4.7.0 和 HPMLink V4.7.0。将来发布需审查版本锁定、快照与校验记录后再运行生成器，不能将未经审核的远程 latest 自动当作正式发布。

`sections.skill`、`sections.software`、`sections.siteAgent` 是独立发布列表；Site Agent 仍位于页面的安装包区域，不另开第四主分区。`sections.firmware.V2/V3/V4` 独立维护；V4 的 MicroLink / HPMLink 按 `family` 分别选择，不能相互覆盖。每个系列第一个条目为最新正式版，其余为历史正式版。没有历史 Site Agent 记录时不猜测补包。

发布项：version、date（官方未注明时 null）、channel（stable）、notes、source、hardware/family/build_id（固件）、files、可选 supportFiles（签名/清单，明确非安装包）。文件项：name、platform、hardware（如适用）、bytes、sha256、urls（tencent/github/gitee，仅已核实链接）。公开时间戳按北京时间 UTC+8 显示，日期型历史值保留原文；固件索引发布日期与无时区 build_id 分开标注，不用资产上传时间冒充原始发布日期。

## 后续发布

1. 核对正式发布状态、源提交、硬件系列、完整资源清单与更新说明。仅本轮七份软件资源和四份新固件新增到目录。
2. 完整下载实际文件，计算 SHA-256 和长度，并核对每个镜像。缺失或无法核实的地址不写入 verified-sources。Windows 安装包超过 Gitee 100 MB 限制，不生成其 Gitee 地址。
3. 更新只读快照、版本锁定和测试，运行 `python scripts/sync_download_catalog.py`，保留历史正式项；不要构建本轮未发布的 Linux/macOS 包。
4. 运行上述回归测试和完整官网构建；检查每个版本选择、手机导航及真实文件下载。修改资源时同步调整页面和 JS 的版本查询值，不改服务器缓存策略。
5. 审查后仅部署四个白名单文件；验证线上字节摘要与构建产物一致。部署前后检查原自动更新接口不变。

历史日期、功能更新说明及缺失镜像的限制列于目录 missingResources，页面会公开展示。GitHub 和 Gitee 仓库入口不是二进制镜像，不冒充文件下载。
