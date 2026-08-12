# 资料下载

本页集中提供 MKLink 开发资料、软件和源码入口。下载后先确认文件适用于自己的下载器型号和目标芯片，再按对应教程操作。

## 官方资料包

[下载 MKLink 开发资料](https://pan.baidu.com/s/1Dr8Ss16cBRWXtQpyOGrROg?pwd=zyo0)（提取码：`zyo0`）

资料包通常包含下载器固件、FLM 示例、驱动或工具、硬件说明和演示文件。不同发布日期的目录可能不同，以包内版本说明为准。

## 开源项目

| 项目 | GitHub | Gitee |
|---|---|---|
| MKLink AI Probe / Web GUI / Skill | [Aladdin-Wang/Mklink-AI-Probe](https://github.com/Aladdin-Wang/Mklink-AI-Probe) | [Aladdin-Wang/Mklink-AI-Probe](https://gitee.com/Aladdin-Wang/Mklink-AI-Probe) |
| MicroBoot | [Aladdin-Wang/MicroBoot](https://github.com/Aladdin-Wang/MicroBoot) | 以仓库说明为准 |

网络允许时优先使用 GitHub；访问失败时再使用 Gitee 镜像。安装 AI Skill 必须安装完整目录和运行依赖，不能只下载 `SKILL.md`。

## 常用文件怎么选

| 文件 | 用途 | 注意事项 |
|---|---|---|
| `.rbl` | MKLink V2 固件升级 | 必须匹配 V2 |
| `.uf2` | MKLink V3/V4 固件升级 | 按住按键进入 `CHERRYUF2` |
| `.FLM` | Arm MCU 脱机下载算法 | 核对器件、Flash 范围和 RAM 基址 |
| `.hex` | 带地址的目标固件 | 地址记录在文件内部 |
| `.bin` | 原始目标固件 | 烧录时必须提供准确基地址 |
| `.axf` / `.elf` | 符号、变量和源码定位 | 必须与已烧录固件来自同一次构建 |
| `.map` | 地址、段和 RTT 控制块查询 | 重新编译后地址可能变化 |

## 安全与版本记录

- 生产使用前记录下载来源、发布日期、文件大小和 SHA-256；
- 不使用聊天群转发但来源不明的固件或算法文件；
- 升级包、目标固件和 FLM 分目录保存，避免误选；
- 历史量产版本应保留固件、脚本、算法和验收记录；
- 文档与软件功能不一致时，以当前软件的可见字段和发布说明为准，并在[常见问题](questions.md)中反馈。

安装上位机见[安装上位机与快速启动](gui-install.md)，下载器自身升级见[固件升级](firmware-upgrade.md)。

## 使用 AI

> 列出当前任务需要的文件、来源和校验项，不下载或写入设备。
