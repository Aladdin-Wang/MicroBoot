在工程管理器中展开 **MicroBoot**，并找到新加入的用户适配器文件（**user_app_cfg.h**)，双击打开后，在编辑器的左下角选择 **Configuration Wizard**，进入图形配置界面：

![cmsis_pack_7](E:/software/MicroBoot/docs/images/quick-start/cmsis_pack_7.png)

配置bootloader的参数：

- The starting address of the app：从bootloader跳转到APP的地址；
- the reset vector offset：复位向量偏移，通常为 4，默认无需修改
- The app size：APP占用FLASH的大小，必须对扇区对齐；
- The Boot Flash Ops Addr：对flash进行擦写函数的api地址；