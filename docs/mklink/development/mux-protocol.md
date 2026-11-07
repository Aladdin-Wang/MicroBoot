# CDC MUX 协议

MUX 在一条命令 CDC 上复用内存、RTT、SystemView 与采样，后台只保留一个 reader
和命令锁。GUI、CLI、MCP 接入共享后台；不要另开串口或在 MUX 中混入 Pika 文本。

本页按 [0.4.3 main（186b6a0）](https://github.com/MicroKeen/Mklink-AI-Probe/tree/186b6a0ffa2408d48b8c558eb01782fae84e96bc/Mklink-AI-Probe/mklink)的主机契约整理。
正式 v0.4.2 已有会话复位、4096B 单次读取、RTT 增量订阅和 480B 量子；
下文的新采样契约、窄样本及 Bulk 属于开发版，须配套固件能力，不能按版本号直接启用。

## 1. 建立会话

1. 用 `cmd.get_version()` 确认 `MUX_TARGET=1`，向同一命令口写入 `b'~MKLINK-MUX1\n'`。
2. 发送 HELLO（0x01，epoch=0）。成功数据为 8B，首 u32 是能力位，必须包含 bits 0、1；使用应答的 epoch。
3. 发送 CLAIM（0x04），payload 为随机 8B nonce，成功数据须原样回传 nonce。
4. 目标请求 0x10～0x3F 的 payload 统一前置该 nonce。每次请求使用新的 request ID，同一时刻仅一个请求在途。

空闲约 1 秒发 HEARTBEAT（0x02）；CLOSE（0x03）正常结束会话。
请求 3 秒超时即失效，不重放、不切回 Pika。status=3/4（claim 过期 / 重复请求）同样须显式重连；
status=6 表示 busy，交由调用者确认后重新操作，不自动循环重试。

## 2. 帧格式

所有整数均为小端；通用头为 `struct '<4sBBHII'`，长 16B。

| 偏移 | 长度 | 字段 |
| --- | ---: | --- |
| 0 | 4 | magic：ASCII `MLX1` |
| 4 | 1 | version：一般为 1 |
| 5 | 1 | opcode |
| 6 | 2 | payload_length |
| 8 | 4 | epoch |
| 12 | 4 | request_id；异步事件使用事件序号 |
| 16 | N | payload |
| 16+N | 4 | CRC32（header + payload），小端 |

响应 opcode 为请求 opcode `| 0x80`，payload 首字节为 status，0 表示成功，后面才是结果数据。
响应须匹配 request ID 和 epoch；CRC 错误、错误响应或会话变化均不能当作成功。

只有能力 bit4 开启时，0x41 Watch 事件可使用 version=2，省略末尾 CRC；
其余命令、响应、RTT 和 Bulk 仍使用 version=1 + CRC。串口读块不等于完整帧，应累积后解析。

### 能力位

| 位 | 用途 |
| --- | --- |
| 0、1 | MUX 目标操作基础能力，握手必需 |
| 3、4 | Watch 批处理 / 无 CRC 的 v2 Watch 事件 |
| 5 | 会话内软件复位 0x15 |
| 6 | 4096B 块读取 0x16 |
| 7 | RTT 掩码增量更新 0x23 |
| 8 | 可协商 480B 传输量子；不代表已启用 |
| 9 | Watch timing v2，START 回传周期与时基 |
| 10 | 窄样本批采 0x33（开发版） |
| 11 | 4096B 连续 Bulk 采样（开发版） |

## 3. 命令速查

下面 payload 不含统一的 8B nonce；“结果”不含响应 status。空表示无额外字段。

| opcode | 功能 | payload | 成功结果 |
| --- | --- | --- | --- |
| 0x10 | 目标状态 | 空 | 16B；末字节接口：1=SWD，2=JTAG |
| 0x11 | 读取内存 | `address:u32, size:u16` | 请求的字节数 |
| 0x12 | 写入内存 | `address:u32, data` | ACK；不代替回读校验 |
| 0x13 | SWD attach | 空 | 不复位目标 |
| 0x14 | Watch 统计 | 空 | `<IIII`：batches、collected、collect_cycles、pack_cycles |
| 0x15 | 软件复位 | 空 | 空；需 bit5，不重建会话 |
| 0x16 | 大块读取 | `address:u32, size:u16` | 最多 4096B，需 bit6 |
| 0x20 | RTT 启动 | `<IBB`：控制块地址、Up mask、Down mask | 2B：最大 Up / Down 描述符数，不是掩码 |
| 0x21 | RTT 下行 | `channel:u8, data` | 接收字节数 u8 |
| 0x22 | RTT 停止 | 空 | ACK |
| 0x23 | RTT 更新 | `up_mask:u8, down_mask:u8` | ACK；需 bit7 |
| 0x24 | RTT 快速下行 | `channel:u8, data` | 接收字节数 u16；须已协商 480B |
| 0x25 | 设置传输量子 | `quantum:u16`（128 或 480） | 空；需 bit8 |
| 0x31 | Watch 停止 | 空 | ACK |
| 0x32 | Watch 启动 | 见下节 | 周期 u32 + 时基 Hz u32 |
| 0x33 | 窄样本启动 | 同 0x32 | 同 0x32；需 bit10 |
| 0x34 / 0x35 | Bulk 启动 / 停止 | 见下节 | 需 bit11 |

CLAIM 后量子默认 128B。收到 0x25 的成功空 ACK 后才能采用 480B，HELLO 声明本身不够。
0x11/0x12 按协商量子分块；0x16 独立允许 4096B，响应连同 status 最多 4097B。
地址范围不得越过 32 位空间；写入完成后按需要回读验证，不能用写 ACK 推断已校验。
0x14 仅在已有且确认停止采样的会话中读取，不为获取统计自动停止采集或新建会话。

## 4. RTT / SystemView 事件

0x40 payload 为 `channel:u8, status:u8, timestamp:u32, data`，最多 8 个通道。
上行、下行分别使用掩码，目标必须初始化相应缓冲区；未启用通道不能由下载器自动创建。
RTT 与 SystemView 共享一个 RTT 控制块，各消费者有独立队列，停止一个消费者不应抢停另一个。

<a id="watch"></a>

## 5. Watch（开发版采样契约）

当前 main 要求 `(capabilities & 0x618) == 0x618`，即 bits 3、4、9、10 全部具备。
此要求不套用到正式 v0.4.2 的旧采样适配器。

- 0x32 payload：`<IB` 周期 μs、区域数，随后每区 `<IB` 地址、长度。最多 15 个物理区域，每区 1～128B。
- 单个自然对齐的 1B / 2B 区域改用 0x33；不是仅有 bit10 就改变普通订阅的事件宽度。
- START 结果 `<II` 为接受周期和时基 Hz；周期须与请求一致，时基必须非零。
- 周期上限 60 秒。显式 `fastest=True, period=0` 才发送线上的 0；普通 `period=0` 兼容成 2000 μs，不表示全速或单次。

0x41 普通事件：`<BBI` 区域索引、status、32 位时间戳，随后区域数据。
当前主机按微秒解释事件时间戳并展开 32 位回绕；START 时基字段仍需核对，
配置 ACK 不代表实测采样率。

status=9 为批帧：头 4B 是 `0, 9, sample_count, sample_bytes`，随后每个样本为
`timestamp:u32 + data[sample_bytes]`。按实际帧长核对样本数量，不固定假定每批 127 个。
status=5 表示读取错误，7 表示调试使采集失效，8 表示复位后恢复超时。
错误、事件序号缺口和队列丢弃必须保留为数据中断，不把缺失数据补成连续曲线。

### Bulk：较大连续样本（开发版，需 bit11）

每个完整样本最多 4096B，用于明确请求的大块采集，不放宽普通 GUI 的变量布局限制。

- START 0x34：`<IIB` 流 ID、周期 μs、区域数，随后每区 `<IH` 地址、长度。
- START 结果 `<IIHHI`：流 ID、接受周期、总样本长度、片长 452、时基 Hz，均须核对。
- STOP 0x35：payload 和成功结果均为该流 ID（u32）。
- 事件 0x42 始终为 CRC-v1，payload 是 40B 头 + 最多 452B 数据。

| 格式 `<IIQQIIHHHBB` | 含义 |
| --- | --- |
| u32、u32 | stream_id、sample_id |
| u64、u64 | 读取起始、结束时间戳 |
| u32、u32 | 读取首尾 DAP epoch |
| u16、u16、u16 | 样本总长、片偏移、片长度 |
| u8、u8 | status、flags |

flags：bit0 首片，bit1 尾片，bit2 期间穿插调试。相同样本各片的元数据必须一致，
偏移连续且收齐总长才交付。丢片、复位或错误时不返回半个样本；即使 CRC 正确，也不是原子快照。

实现对照：[传输与内存](https://github.com/MicroKeen/Mklink-AI-Probe/blob/186b6a0ffa2408d48b8c558eb01782fae84e96bc/Mklink-AI-Probe/mklink/mux.py)、
[RTT](https://github.com/MicroKeen/Mklink-AI-Probe/blob/186b6a0ffa2408d48b8c558eb01782fae84e96bc/Mklink-AI-Probe/mklink/mux_rtt.py)、
[Watch](https://github.com/MicroKeen/Mklink-AI-Probe/blob/186b6a0ffa2408d48b8c558eb01782fae84e96bc/Mklink-AI-Probe/mklink/mux_watch.py)、
[Bulk](https://github.com/MicroKeen/Mklink-AI-Probe/blob/186b6a0ffa2408d48b8c558eb01782fae84e96bc/Mklink-AI-Probe/mklink/mux_bulk_watch.py)。
