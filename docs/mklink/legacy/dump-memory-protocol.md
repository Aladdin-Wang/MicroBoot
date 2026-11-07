# 历史 dump_memory 协议

本页仅供维护旧固件和解析历史 `MPMDMPMD` 数据。当前开发版在线采样使用
[MUX](../development/mux-protocol.md)，不发送旧 `cmd.dump_memory` 命令，也不在失败后回退。

`cmd.dump_memory(addr1, size1, ..., period)` 的参数成对指定地址和字节数；
旧接口 `period > 0` 持续采样，`0` 单次，`-1` 停止，单位秒。
Pika 文本入口最多使用 15 组区域；协议能表示 16 组不等于文本入口安全支持。
旧流停止后至少排空 50 ms，再释放连接；这不是 MUX 的停止步骤。

所有多字节字段为小端。总数据不超过 2048B 使用普通帧，更大数据分块输出，
每块数据最多 2048B。`SW01` 是另一种历史 SuperWatch 格式，不能按此表解码。

**普通帧格式**:

| 偏移 |  长度 | 字段           | 说明                                                        |
| ---- | ----: | -------------- | ----------------------------------------------------------- |
| 0x00 |     8 | `magic`        | 固定同步头 `4D 50 4D 44 4D 50 4D 44`（ASCII 为 `MPMDMPMD`） |
| 0x08 |     8 | `timestamp_us` | 设备端时间戳，单位 us                                       |
| 0x10 |     2 | `frame_length` | 整帧长度，包含头、数据、`flags` 和 `crc32`                  |
| 0x12 |     1 | `region_count` | 本帧区域数量，最大 16                                       |
| 0x13 | 3 + n | `region[i]`    | 区域数据，格式见 region 结构                                |
| ...  |     2 | `flags`        | 状态位，小端                                                |
| ...  |     4 | `crc32`        | 对 `magic` 到 `flags` 的所有字节计算 CRC32，结果小端存放    |

**大数据分块帧格式**:

| 偏移 |  长度 | 字段           | 说明                                                        |
| ---- | ----: | -------------- | ----------------------------------------------------------- |
| 0x00 |     8 | `magic`        | 固定同步头 `4D 50 4D 44 4D 50 4D 44`                       |
| 0x08 |     8 | `timestamp_us` | 设备端时间戳，单位 us                                       |
| 0x10 |     2 | `frame_length` | 整帧长度，包含头、数据和帧 `crc32`                          |
| 0x12 |     1 | `region_count` | 当前 block 中包含的区域数量                                 |
| 0x13 |     2 | `flags`        | 状态位，小端                                                |
| 0x15 |     4 | `total_size`   | 本次 dump 请求的总数据量                                    |
| 0x19 |     2 | `block_size`   | block 数据负载大小，当前固定为 2048                         |
| 0x1B |     2 | `block_index`  | 当前 block 序号，从 0 开始                                  |
| 0x1D |     2 | `block_count`  | 总 block 数量                                               |
| 0x1F |     4 | `block_crc32`  | 对当前 block 的原始数据负载计算 CRC32                       |
| 0x23 | 3 + n | `region[i]`    | 当前 block 内的区域数据，格式见 region 结构                 |
| ...  |     4 | `crc32`        | 对 `magic` 到最后一个 `region_data` 的所有字节计算 CRC32    |

**region 结构**:

| 字段           |          长度 | 说明                                                         |
| -------------- | ------------: | ------------------------------------------------------------ |
| `region_index` |             1 | 区域序号，从 0 开始，对应命令里第几个 `(addr, size)`         |
| `region_size`  |             2 | 当前帧/当前 block 中该区域读取到的字节数                     |
| `region_data`  | `region_size` | 原始内存数据                                                 |

**flags**:

- `0x0001`：Tick overflow
- `0x0002`：Timing violation
- `0x0004`：Region error
- `0x0008`：Sample dropped
- 其余位保留

**示例**:

- `cmd.dump_memory(0x20000054, 4, 0x2000006C, 2, 0.1)`
- `cmd.dump_memory(0x20002000, 17408, 0)`

```c++
[10:26:24.946]收←◆4D 50 4D 44 4D 50 4D 44 2E 83 A4 31 00 00 00 00 25 00 02 00 04 00 68 3D 00 20 01 02 00 6C 00 00 00 DC EF ED 86
```

```text
4D 50 4D 44 4D 50 4D 44   // magic
2E 83 A4 31 00 00 00 00   // timestamp_us
25 00                     // frame_length = 37
02                        // region_count = 2
00 04 00 68 3D 00 20      // region 0: size=4, data=4 bytes
01 02 00 6C 00            // region 1: size=2, data=2 bytes
00 00                     // flags
DC EF ED 86               // CRC32
```
