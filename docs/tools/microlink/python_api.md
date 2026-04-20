# Python API 列表

本列表汇总了 MKLink 常用的 Python API 接口

---

![](../../images/microlink/API.png)

## 1.cmd api列表

### 1.1 读取Ram数据

`cmd.read_ram(addr,size,path)`

**参数**:

- `addr`：读取地址
- `size`：读取的字节数
- `path`：可选参数，保存数据到文件系统

**示例**:

- `cmd.read_ram(0x20000000,128)`
- `cmd.read_ram(0x20000000,128,"ram.bin")`

**注意：数据保存到文件后，需要重启下载器，U盘中才能刷新出新文件**

```c++
cmd.read_ram(0x20000000,128)
         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
20000000 01 00 00 00 00 00 00 00 00 00 00 00 04 1a 00 20 
20000010 01 00 00 00 00 00 00 00 00 a2 4a 04 01 00 00 00 
20000020 0f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
20000030 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
20000040 53 45 47 47 45 52 20 52 54 54 00 00 00 00 00 00 
20000050 03 00 00 00 03 00 00 00 8b 49 00 08 f8 00 00 20 
20000060 00 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
20000070 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
```

### 1.2 写入Ram数据

`cmd.write_ram(addr,byte1,byte2,byte3,byte4,...N)`

**参数**:

- `addr`：写入地址
- `byte`：N 个待写入的字节数据

**示例**:

- cmd.write_ram(0x20001000,0xA5,0x5A)

```c++
cmd.write_ram(0x20001000,0xA5,0x5A)
         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
20001000 a5 5a 
    
cmd.read_ram(0x20001000,2)
         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
20001000 a5 5a 
```



### 1.3 读取Flash数据

`cmd.read_flash(addr,size,path)`

**参数**:

- `addr`：读取地址
- `size`：读取的字节数
- `path`：可选参数，保存数据到文件系统

**示例**:

- `cmd.read_flash(0x08000000,128)`
- `cmd.read_flash(0x08000000,128,"flash.bin")`

**注意：数据保存到文件后，需要重启下载器，U盘中才能刷新出新文件资源。**

```c++
cmd.read_flash(0x08000000,128)
         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
08000000 b0 1f 00 20 49 01 00 08 f1 0f 00 08 e9 0f 00 08 
08000010 ed 0f 00 08 1d 09 00 08 fd 0f 00 08 00 00 00 00 
08000020 00 00 00 00 00 00 00 00 00 00 00 00 f5 0f 00 08 
08000030 1f 09 00 08 00 00 00 00 f3 0f 00 08 f7 0f 00 08 
08000040 63 01 00 08 63 01 00 08 63 01 00 08 63 01 00 08 
08000050 63 01 00 08 63 01 00 08 63 01 00 08 63 01 00 08 
08000060 63 01 00 08 63 01 00 08 63 01 00 08 63 01 00 08 
08000070 63 01 00 08 63 01 00 08 63 01 00 08 63 01 00 08 
```

### 1.4 擦除Flash扇区

- 擦除扇区：`cmd.erase_sector_flash(addr)`

- 整片擦除：`cmd.erase_chip_flash(addr)`

**参数**:

- `addr`：扇区地址，必须对齐扇区

**示例**:

- cmd.erase_sector_flash(0x08005000)
- cmd.erase_chip_flash(0x08000000)

**注意：擦写flash 需要调用flash下载算法的函数接口，所以需要先使用load.flm()加载flash下载算法**

```c++
load.flm("FLM/STM32F10x_1024.FLM",0x08000000,0x20000000)
0
>>> 
cmd.erase_sector_flash(0x08005000)
0
cmd.erase_chip_flash(0x08000000)
0    
>>> 
```

### 1.5 写入Flash数据

`cmd.write_flash(addr,byte1,byte2,byte3,byte4,...N)`

**参数**:

- `addr`：写入地址
- `byte`：N 个待写入的字节数据

**示例**:

- cmd.write_flash(0x08005000,0xA5,0x5A)

**注意：擦写flash 需要调用flash下载算法的函数接口，所以需要先使用load.flm()加载flash下载算法**

```c++

cmd.write_flash(0x08005000,0xA5,0x5A)
         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
08005000 a5 5a 
    
cmd.read_flash(0x08005000,16)
         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
08005000 a5 5a ff ff ff ff ff ff ff ff ff ff ff ff ff ff 
```

## 2 load api列表

### 2.1 加载下载算法

`load.flm(path,flash_addr,ram_addr)`

**参数**:

- `path`：FLM文件的目录
- `flash_addr`：flash的基地址
- `ram_addr`：ram的基地址

**示例**:

- load.flm("FLM/STM32F10x_1024.FLM",0x08000000,0x20000000)

```c
load.flm("FLM/STM32F10x_1024.FLM",0x08000000,0x20000000)
0
```

### 2.2 烧录bin文件

`load.bin(path,addr,path,addr,...)`

**参数**:

- `path`：bin文件目录
- `addr`：addr烧录地址
- 可选参数，可依次烧录多个文件到不同地址

**示例**:

- load.bin("bootloader.bin",0x08000000,"app.bin",0x08005000)

```c
 load.bin("bootloader.bin",0x08000000,"app.bin",0x08005000)
fileName bootloader.bin, Addr 0x8000000
Download:   5% ,used 234 msDownload:  11% ,used 508 msDownload:  17% ,used 780 msDownload:  23% ,used 1053 msDownload:  29% ,used 1327 msDownload:  34% ,used 1600 msDownload:  40% ,used 1873 msDownload:  46% ,used 2147 msDownload:  52% ,used 2421 msDownload:  58% ,used 2694 msDownload:  63% ,used 2967 msDownload:  69% ,used 3241 msDownload:  75% ,used 3514 msDownload:  81% ,used 3788 msDownload:  87% ,used 4062 msDownload:  93% ,used 4335 msDownload:  98% ,used 4609 msDownload: 100% ,used 4686 ms
 /bootloader.bin loaded success.
fileName app.bin, Addr 0x8005000
Download:   2% ,used 234 msDownload:   5% ,used 507 msDownload:   8% ,used 779 msDownload:  11% ,used 1053 msDownload:  13% ,used 1327 msDownload:  16% ,used 1600 msDownload:  19% ,used 1874 msDownload:  22% ,used 2147 msDownload:  24% ,used 2421 msDownload:  27% ,used 2694 msDownload:  30% ,used 2968 msDownload:  33% ,used 3241 msDownload:  35% ,used 3514 msDownload:  38% ,used 3788 msDownload:  41% ,used 4062 msDownload:  44% ,used 4336 msDownload:  46% ,used 4609 msDownload:  49% ,used 4881 msDownload:  52% ,used 5154 msDownload:  55% ,used 5428 msDownload:  57% ,used 5701 msDownload:  60% ,used 5974 msDownload:  63% ,used 6248 msDownload:  66% ,used 6521 msDownload:  68% ,used 6795 msDownload:  71% ,used 7069 msDownload:  74% ,used 7342 msDownload:  77% ,used 7616 msDownload:  79% ,used 7890 msDownload:  82% ,used 8163 msDownload:  85% ,used 8437 msDownload:  88% ,used 8710 msDownload:  90% ,used 8984 msDownload:  93% ,used 9257 msDownload:  96% ,used 9531 msDownload:  99% ,used 9805 msDownload: 100% ,used 9882 ms
 /app.bin loaded success.
```

### 2.3 烧录hex文件

`load.hex(path,...)`

**参数**:

- `path`：hex文件目录
- 可选参数，可依次烧录多个文件到不同地址

**示例**:

- load.hex("bootloader.hex","app.hex")

```c
load.hex("bootloader.hex","app.hex")
fileName bootloader.hex
Download:   7% ,used 44 ms Download:  15% ,used 208 ms Download:  22% ,used 371 ms Download:  30% ,used 424 ms Download:  37% ,used 588 ms Download:  45% ,used 751 ms Download:  52% ,used 804 ms Download:  60% ,used 967 ms Download:  67% ,used 1131 ms Download:  75% ,used 1294 ms Download:  82% ,used 1347 ms Download:  90% ,used 1510 ms Download:  97% ,used 1674 ms Download: 100% ,used 1726 ms 
 /bootloader.hex loaded success
fileName app.hex
Download:   2% ,used 48 ms Download:   4% ,used 212 ms Download:   6% ,used 375 ms Download:   8% ,used 428 ms Download:  10% ,used 591 ms Download:  12% ,used 755 ms Download:  14% ,used 808 ms Download:  16% ,used 971 ms Download:  18% ,used 1134 ms Download:  20% ,used 1298 ms Download:  22% ,used 1351 ms Download:  24% ,used 1514 ms Download:  26% ,used 1678 ms Download:  28% ,used 1731 ms Download:  31% ,used 1894 ms Download:  33% ,used 2057 ms Download:  35% ,used 2221 ms Download:  37% ,used 2274 ms Download:  39% ,used 2437 ms Download:  41% ,used 2601 ms Download:  43% ,used 2654 ms Download:  45% ,used 2817 ms Download:  47% ,used 2981 ms Download:  49% ,used 3144 ms Download:  51% ,used 3197 ms Download:  53% ,used 3360 ms Download:  55% ,used 3524 ms Download:  57% ,used 3577 ms Download:  59% ,used 3740 ms Download:  62% ,used 3904 ms Download:  64% ,used 4067 ms Download:  66% ,used 4120 ms Download:  68% ,used 4283 ms Download:  70% ,used 4447 ms Download:  72% ,used 4500 ms Download:  74% ,used 4663 ms Download:  76% ,used 4827 ms Download:  78% ,used 4990 ms Download:  80% ,used 5043 ms Download:  82% ,used 5206 ms Download:  84% ,used 5370 ms Download:  86% ,used 5423 ms Download:  88% ,used 5586 ms Download:  90% ,used 5750 ms Download:  93% ,used 5803 ms Download:  95% ,used 5966 ms Download:  97% ,used 6130 ms Download:  99% ,used 6293 ms Download: 100% ,used 6345 ms 
 /app.hex loaded success
0
```

## 3 SEGGER RTT api列表

### 3.1 启动RTT

RTTView.start(addr,size,Channel)

**参数**:

- `addr`：_SEGGER_RTT控制块的地址
- size，搜寻范围
- Channel，指定RTT的通道

**示例**:

- RTTView.start(0x20000200,1024,0)

```c
RTTView.start(0x20000200,1024,0)
Find SEGGER RTT addr 0x20000200
UpBuffer Channel 0 Size: 2048 Mode: 0
UpBuffer Channel 1 Size: 0 Mode: 0
UpBuffer Channel 2 Size: 0 Mode: 0
DownBuffer Channel 0 Size: 16 Mode: 0
DownBuffer Channel 1 Size: 0 Mode: 0
DownBuffer Channel 2 Size: 0 Mode: 0
```

### 3.2 停止RTT

`RTTView.stop()`

## 4 SEGGER SystemView api列表

### 4.1 启动SystemView

`SystemView.start(addr,size,Channel)`

**参数**:

- `addr`：_SEGGER_RTT控制块的地址
- size，搜寻范围
- Channel，指定RTT的通道

**示例**:

- SystemView.start(0x20000200,1024,1)

```c
>>> SystemView.start(0x20000200,1024,1)
Addr = 0x20000200,wSize = 1024,Channel = 1
Find SEGGER RTT addr 0x20000200
UpBuffer Channel 0 Size: 2048 Mode: 0
UpBuffer Channel 1 Size: 0 Mode: 0
UpBuffer Channel 2 Size: 0 Mode: 0
DownBuffer Channel 0 Size: 16 Mode: 0
DownBuffer Channel 1 Size: 0 Mode: 0
DownBuffer Channel 2 Size: 0 Mode: 0
```

## 4 VOFA+ api列表

### 4.1 启动VOFA

vofa.send(addr,num,time)

**参数**:

- addr：变量的地址
- num，连续读取的变量个数
- 周期，读取周期，单位秒，0为停止

**示例**:

- vofa.send(0x20000030,5,0.00001)

