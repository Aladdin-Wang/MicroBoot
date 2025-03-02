# 常见移植问题
## 1、串口无法发送字符`c`
- 获取时间函数 `get_system_time_ms` 需要提供毫秒计时，超时3秒发送字符 `c` 依赖这个函数计时，此函数是 `weak` 定义的
- 检查移植示例串口发送函数 `uart_sent_data` 使用 `connect` 连接方式是否正确，例如：
```c
  //连接信号与槽
  //#define connect(__SIG_OBJ,__SIG_NAME,__SLOT_OBJ,__SLOT_FUN)    \
            direct_connect(__SIG_OBJ.tObject,__SIG_NAME,__SLOT_OBJ,__SLOT_FUN)
  //example：
  //connect(&tCanMsgObj,SIGNAL(can_sig),&s_tFIFOin,SLOT(enqueue_bytes));

// SLOT_OBJ 不能指定为 NULL
connect(&s_tYmodemOtaReceive.tYmodemReceive, SIGNAL(ymodem_rec_sig), NULL, SLOT(uart_sent_data));
// SLOT_OBJ 传递非空的值即可
connect(&s_tYmodemOtaReceive.tYmodemReceive, SIGNAL(ymodem_rec_sig), &huart6, SLOT(uart_sent_data));

```

## 2、使用新工程移植microboot调试没有进入`main`函数
- microboot 函数修饰符 `__attribute__((constructor))` 告诉编译器在程序启动时自动调用这个函数 `enter_application` 函数主要操作 flash 检查系统状态，所以 `user_app_cfg.h` 需要根据芯片实际配置 app 启动位置和实际大小，参考:[模块配置](./quick-start/cmsis-pack.md#3.添加MicroBoot到工程)
- 如果调试查看汇编发现卡在 `BKPT` 指令，那就是遇到了semihosting问题，解决这个可以参考:[Semihosting真的是嵌入式阑尾么？](https://mp.weixin.qq.com/s/lIdCRfp04I0kf0L7bHRoFQ?poc_token=HAa_w2ejsxpCqNmcvgwFyBTvDqDa6cLBjrXhu787)

## 3、使用 Ymodem 协议传输固件发现进度条不动
检查移植适配的 flash 驱动操作接口写入和擦除是否正常
