# MicroLink Python API 列表

本列表汇总了 MicroLink 常用的 Python API 接口

---

1. `ReadFlm.load(flm_path: str, base_addr: int, ram_addr: int = 0) -> int`

加载 FLM 算法文件。

- **参数**:
  - `flm_path`：FLM 文件路径（通常以 `.FLM.o` 结尾）
  - `base_addr`：目标设备中用于烧写的 Flash 基地址
  - `ram_addr`：算法运行所需的 RAM 基地址（可选，默认值为 `0`）

- **返回值**:
  - `0` 表示加载成功
  - 非 `0` 表示加载失败

---

2. `load.bin(file_path: str, addr: int) -> int`

烧录二进制文件到指定地址。

- **参数**:
  - `file_path`：待烧写的 `.bin` 文件路径
  - `addr`：烧写起始地址

- **返回值**:
  - `0` 表示烧录成功
  - 非 `0` 表示烧录失败

---

3. `RTTView.start(rtt_addr: int, size: int, channel: int) -> None`

启动 RTT 数据转发功能，使 PC 可通过串口查看 MCU 的 RTT 输出。

- **参数**:
  - `rtt_addr`：RTT 控制块在 MCU RAM 中的地址（如 `0x20000000`）
  - `size`：寻找RTT 控制块的地址范围（如 `1024`）
  - `channel`：RTT 通道号（通常填 `0`）

- **说明**:
  - 启动后，可以使用串口助手或 RTT Viewer 工具查看 MCU 的实时日志数据。

---

4. `RTTView.stop() -> None`

停止 RTT 功能，释放相关资源。
