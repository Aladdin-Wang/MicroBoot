# HPM6E80：波形、RTT 与 FreeRTOS 时间线

测试平台：HPM6E00EVK / HPM6E80，HPM SDK 1.12.1，核心 0，Flash XIP Debug 构建。探针使用 HPMLink V4。此例不操作 OTP。

把压缩包内的 CMakeLists.txt 和 src 放入 SDK samples/hello_world，对照自己的板卡配置构建。更改 RTOS 配置后执行完整 rebuild，避免旧启动对象残留。生成的 BIN 与 ELF 必须来自同一次构建。

- Wave 任务每 1 ms 更新参考值 wave_reference、响应 wave_response 和误差 wave_error。
- 四路错相波形：phase_0、phase_90、phase_180、phase_270，分别为 0°、90°、180°、270°，幅值 ±100、频率 1 Hz。取消其他变量后添加这四路，选择 30 MHz，即可复现文章第一段动画。
- response_gain 初始值 0.05，用于观察响应速度；wave_tick 是任务运行计数。
- RTT 通道 0 每 500 ms 输出运行计数，并回显收到的短文本。
- RTT 通道 1 用于 SystemView；网页 RTOS Trace 负责发起采集。
- memory_test 是 4 KiB 已知测试图案，便于逐字节验证内存读取。

为了使调试总线读取 RTT 与运行变量时保持一致，本测试例关闭了 D-cache。这是测试工程的选择，不代表所有产品都应关闭缓存；移植到自己的应用时，需要按内存属性处理调试缓冲区的一致性。

给 AI 的提示词示例：

> 使用 HPM6E80 的 hello_world 工程，完整编译并通过 MKLink 下载校验。加载本次 ELF，打开 WebGUI SuperWatch，使用 30 MHz 档观察 wave_reference、wave_response、wave_error。你订阅网页后端的同一条数据流，分析幅值和响应滞后，让我同时看真实曲线。不要再打开第二个探针连接，不要操作 OTP。

下载镜像基址以实际构建为准。本次配置为 0x80000400，采用 HPM ROM API，不需要 FLM。

## 配置与复现

下载 [源码压缩包](hpm6e80-sdk1121-fixture.zip)。将其中 `CMakeLists.txt`、`src/hello_world.c`、`src/FreeRTOSConfig.h` 放入 SDK 1.12.1 的 `samples/hello_world`。SDK 依赖和 SEGGER 组件沿用 SDK，不在压缩包重复分发。

使用 SDK 环境生成 `hpm6e00evk` 的 `flash_xip` 工程并完整构建，下载本次 BIN、加载同次 ELF。程序中 SystemView 只初始化一次，接入 FreeRTOS OS 回调。将包内 `project_info.json` 放到工程的 `.mklink/project_info.json`，其中 `systemview_ram_base=0x01200000`、`systemview_id_shift=2` 用于本例任务地址解码，不适用于所有芯片。

打开 SuperWatch，添加 `wave_reference`、`wave_response`、`wave_error`，选择 30 MHz 并应用；间隔 `0.000001 s` 请求满速采样，实际组率由读链路决定。AI 通过同一网页后端订阅，避免第二个连接占用探针。改 `response_gain` 前停止采集，写入后回读，再启动比较；RAM 修改复位后恢复默认 0.05。

要复现文章中的 GIF，先把 RAM 中的 `response_gain` 设为 `0.01`，运行后观察蓝色响应线的滞后与黄色误差；让 AI 采集约 12 秒，再按上述停采、写入、回读流程改为 `0.10`，重新采集约 12 秒。此例误差峰值约从 52.74 降到 5.64。只修改该测试变量，不把演示系数直接套到实际控制系统。

RTT 自动从当前 ELF 定位 `_SEGGER_RTT`，通道 0 看日志或发送 `HPM_OK`。停止 RTT 后使用 RTOS Trace 查看任务。Memory 读取 `memory_test` 符号地址的 4096 字节，按 `0xA5000000 ^ (i * 0x10203)` 核对每个 32 位字。地址从当前 ELF 获取，不照搬截图。

- [性能与功能验证记录](../../../mklink/hpm/hpm6e80-performance-validation.md)
- [最终版擦除、写入、校验三阶段数据](flash-three-phase-measurements.json)
- [GIF 调参前数据](gif-before-check.json) / [调参后数据](gif-after-check.json)
- [历史大镜像计时](flash-measurements.json)
- [0.05 到 0.10 的早期共享流统计](shared-stream-measurements.json)
- [网页脱机触发后的完整回读](gui-offline-verified.json)
