# HPM5301 hello_world 复测附件

2026-09-12，配套 [MKLink 与先楫 HPM 生态](../../../mklink/hpm/overview.md)。

- `hello_world.c` 与 `CMakeLists.txt`：本次实际编译源码，保留原版权。
- `flash-verified.json`：ROM API 烧录和回读结果。
- `benchmark.json` / `benchmark.py`：12 组完整样本吞吐及执行脚本。
- `wave16-capture.csv` / `wave16-validation.json`：独立 5 秒数据和校验结果。
- `flash_verify.py` / `capture_wave.py`：本轮执行脚本。

脚本是实测留档，包含本机 E 盘路径、COM488 和固定地址；运行前按当前工程与 ELF 核对并修改，不直接用于其他板卡。先停止 GUI、释放探针；每次连续采集最多 30 秒。JSON 中 parser_dropped_bytes 是启动文本前导，与 parser_dropped_frames 不同。


[可复现源码包](hpm5301-doc-fixtures.zip)包含基准、RTT、RTOS与可选异常测试源码。RTOS的SystemView依赖按包内说明从已安装SDK引入，保留原许可。各构建必须使用对应ELF。

## 使用说明的参考资料

正文按使用场景组织；以下文件保留测量条件和结果，供需要复核的读者查阅。

| 资料 | 内容 |
| --- | --- |
| [运行时清单](runtime-manifest.json) | 开发运行时的文件摘要，区分开发构建与正式版本 |
| [性能数据](benchmark.json) | 三档时钟、四种数据布局的吞吐与样本间隔 P99 |
| [Memory 校验](memory-verified.json) | 4 KB 读取、局部写入及恢复结果 |
| [SystemView 事件](systemview-events-fixed.json) | 解析修复后的独立 SDK 采集结果 |
| [脱机回读](offline-verified.json) | 网页触发下载后全量回读及运行检查 |
| [GUI 导出 CSV](superwatch-export.csv) | 用户保存的 2635 行、16 通道波形数据 |
| [VOFA 原始流校验](vofa-raw-validation.json) | 启动文本与 JustFloat 帧的逐字节区分 |
| [RISC-V 异常记录](trap-verified.json) | 非法指令现场、SDK 回调地址解释；并非 ARM HardFault 面板结果 |

GUI 工程 JSON 已取得并解析为 16 个通道；尚未完成加载后往返验证，不作为配置恢复成功的证据。GUI 导出 CSV 与独立采集的 `wave16-capture.csv` 来自不同轮次，不应混合统计。

性能矩阵对应原基线探针；后续 SystemView 开发固件的验证不替代该矩阵，也未据此新增 25 / 30 MHz 数据。
