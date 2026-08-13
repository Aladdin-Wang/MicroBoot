# AI 部署脱机下载：从 HEX 到真实触发

脱机下载不能停在“文件已经复制”。量产任务需要依次验证器件、Flash 算法、固件摘要、触发日志和目标程序版本。

## 已验证固件

本例使用在线烧录已经验证过的 HEX：

```text
目标：STM32F103RET6
固件：build/keil/Obj/rt-thread.hex
SHA-256：FD86D173977506E46C7CDEC2BD9C648940562B6FD86059433BEBC6FFECEF051E
```

STM32F103RET6 使用 512 KiB Flash，下载算法必须为 `STM32F10x_512.FLM`。

## 生成和部署任务

给AI 的提示词：

> 把刚验证过的 HEX 做成脱机任务，检查算法和 SHA，部署后触发一次。

AI 的部署检查输出：

```text
下载器：MKLink V4 / 固件 V4.3.5
U 盘：G:\（卷标 MICROKEEN）
器件：STM32F103RE
算法：STM32F10x_512.FLM
算法来源：Keil.STM32F1xx_DFP@2.4.1
```

脚本关键代码：

```python
AUTO_DOWNLOAD_COUNT = 1
WAIT_IDCODE_TIMEOUT = 10000
cmd.set_swd_clock(1000000)
load.flm("FLM/STM32F10x_512.FLM", 0x08000000, 0x20000000)
load.hex("stm32f103ret6_ai_demo.hex")
```

摘要一致后，任务被部署为：

```text
status: deployed
script: stm32f103ret6_ai_demo.py
files:
  stm32f103ret6_ai_demo.hex
  FLM/STM32F10x_512.FLM
  python/stm32f103ret6_ai_demo.py
```

## 触发结果

```text
Download: 100% ,used 12791 ms
/stm32f103ret6_ai_demo.hex loaded success
auto download finished
```

触发完成后继续读取 RTT：

```text
MKLink AI demo | build=20260813 | target=STM32F103RET6 | rtos=RT-Thread 5.1.0
MKLink demo | uptime=5002 ms | temp=30.0 C | speed=1050 rpm | state=0 | alarm=1
```

部署状态证明文件已写入下载器，触发日志证明脚本已执行，RTT 构建标识证明 MCU 运行的是目标固件。人工操作见[脱机下载与量产](../tools/microlink/offline_download.md)。
