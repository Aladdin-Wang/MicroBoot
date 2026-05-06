# IAP升级固件——基于ymodem协议

## 1.V4下载器准备工作

- 在python文件夹中新建一个比如叫`ymodem.py`的脚本，代码如下：

```python
import PikaStdLib
import cmd
import ym
#发送字节数据
#data = bytes([0x01,0x05,0x00,0x01,0x00,0x01,0X5D,0XCA])
#发送字符串数据
data = b'boot\r\n'
#使用串口发送
#uart = serial.uart(115200)
#uart.write_bytes(data,len(data))
#使用485发送
rs485 = serial.rs485(115200)
rs485.write_bytes(data,len(data))
#开始ymodem升级
ymodem = ym.ymodem("uart",115200)
ymodem.send("rtthread.bin")
```

整个升级流程分两步：

**（1）先让 APP 进入 BootLoader**

```
data = b'boot\r\n'
rs485.write_bytes(data, len(data))
```

这一步就是**发一条指令给 APP**，让它跳转到 BootLoader。

这条指令可以是任意格式，比如：

- 字符串命令（如 `boot`）
- Modbus 指令
- 自定义协议数据

只要你的 APP 能识别并执行“跳转 BootLoader”即可。

**（2）通过 YModem 发送固件**

```
ymodem = ym.ymodem("uart", 115200)
ymodem.send("rtthread.bin")
```

进入 BootLoader 后，使用 YMODEM 协议传输固件，完成升级。

**说明：**

- 触发命令和 YModem **可以使用不同波特率**
- APP 阶段用什么协议都可以，只要能识别到跳转指令

## 2.APP准备工作

app需要能接收到下载器发来的跳转指令，比如字符串命令（如 `boot`），以下是我使用的app跳转代码：

```c
typedef struct {
    char chProjectName[16];
    char chHardWareVersion[16];
    char chSoftBootVersion[16];
    char chSoftAppVersion[16];	
    char chPortName[16];
	int wPortBaudrate;
} msgSig_t;
typedef struct {
    union {
        msgSig_t sig;
        char B[sizeof(msgSig_t)];
    } msg_data;
} user_data_t;

user_data_t  tUserData = {
    .msg_data.sig.chProjectName = "app",
};

typedef struct {
    void (*fnGoToBoot)(uint8_t *pchDate, uint16_t hwLength);
    bool (*target_flash_init)(uint32_t addr);
    bool (*target_flash_uninit)(uint32_t addr);
    int  (*target_flash_read)(uint32_t addr, uint8_t *buf, size_t size);
    int  (*target_flash_write)(uint32_t addr, const uint8_t *buf, size_t size);
    int  (*target_flash_erase)(uint32_t addr, size_t size);
} boot_ops_t;

void boot()
{
    memcpy(tUserData.msg_data.sig.chPortName, "UART1", strlen("UART1"));
    tUserData.msg_data.sig.wPortBaudrate = 115200;
    boot_ops_t *ptBootOps = (boot_ops_t *) BOOT_FLASH_OPS_ADDR;
    ptBootOps->fnGoToBoot((uint8_t *)tUserData.msg_data.B, sizeof(tUserData));
    reboot();
}
MSH_CMD_EXPORT(boot, go to bootloader);
```

APP 的核心任务只有一个：**收到指令后跳转到 BootLoader**

**（1）用户数据结构**

```
typedef struct {
    char chProjectName[16];
    char chHardWareVersion[16];
    char chSoftBootVersion[16];
    char chSoftAppVersion[16];	
    char chPortName[16];
    int wPortBaudrate;
} msgSig_t;
```

这个结构体用于**传递参数给 BootLoader**，主要包含：

- 当前工程信息（可选）
- 通信接口（如 `"UART1"`）
- 通信波特率（如 `115200`）

BootLoader 可以根据这些信息初始化通信接口。

**（2）数据封装**

```
typedef struct {
    union {
        msgSig_t sig;
        char B[sizeof(msgSig_t)];
    } msg_data;
} user_data_t;
```

作用：

- 将结构体转换为**字节数组**
- 方便作为参数传递给 BootLoader

**（3）BootLoader 接口表**

```
typedef struct {
    void (*fnGoToBoot)(uint8_t *pchDate, uint16_t hwLength);
    bool (*target_flash_init)(uint32_t addr);
    bool (*target_flash_uninit)(uint32_t addr);
    int  (*target_flash_read)(uint32_t addr, uint8_t *buf, size_t size);
    int  (*target_flash_write)(uint32_t addr, const uint8_t *buf, size_t size);
    int  (*target_flash_erase)(uint32_t addr, size_t size);
} boot_ops_t;
```

这是 BootLoader 提供的一组函数接口，其中最关键的是：

- `fnGoToBoot()`：用于**跳转到 BootLoader**

**（4）跳转函数**

```
void boot()
{
    memcpy(tUserData.msg_data.sig.chPortName, "UART1", strlen("UART1"));
    tUserData.msg_data.sig.wPortBaudrate = 115200;

    boot_ops_t *ptBootOps = (boot_ops_t *) BOOT_FLASH_OPS_ADDR;
    ptBootOps->fnGoToBoot((uint8_t *)tUserData.msg_data.B, sizeof(tUserData));

    reboot();
}
```

执行流程：

设置通信参数

- 指定使用哪个串口（如 `UART1`）
- 指定波特率（如 `115200`）

获取 BootLoader 接口地址

- `BOOT_FLASH_OPS_ADDR` 是 BootLoader 中接口表的固定地址

调用跳转函数

- 将参数（字节流）传给 BootLoader
- 触发跳转

软件复位

- 确保系统正确进入 BootLoader

**（5）命令注册**（可选）

```
MSH_CMD_EXPORT(boot, go to bootloader);
```

作用：

- 将 `boot` 函数注册为命令行指令
- 用户可以直接输入：

```
boot
```

即可触发升级流程。

## 3.bootloader准备工作

参考[基于 CMSIS-PACK 移植教程](https://microboot.readthedocs.io/zh-cn/latest/quick-start/cmsis-pack/)使用microboot。
