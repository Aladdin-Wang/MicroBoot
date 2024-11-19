# 用C宏打造一个轻量级信号与槽机制

---

## 一、引言
信号与槽机制源自Qt 框架，它是一种强大而优雅的事件驱动模型，旨在促进组件之间的松散耦合通信。该模式允许对象之间无需直接相互引用即可响应彼此的状态变更，从而极大地提高了系统的可扩展性和可维护性。

### 1.1 传统事件处理的局限性

在早期 C++ 程序中，事件处理通常通过回调函数或观察者模式来实现，但这些方法存在一些问题：

- **强耦合**：回调函数需要提前注册到特定对象中，导致调用方与被调用方紧密耦合，不利于模块化和重用。

- **代码复杂度高**：实现和维护复杂的事件处理逻辑较为困难，尤其是在多个对象之间需要进行交互时。

- **类型检查不足**：回调函数的接口不够灵活，容易引发编译期或运行时错误。
- **`this` 指针的绑定问题**：C++ 成员函数需要显式绑定到对象实例（`this`），导致回调机制实现复杂且易出错。

### 1.2 信号与槽机制的优势

信号与槽机制提供了一种声明式的方式来描述组件之间的关系，代码更加直观、易读和易维护，降低了程序的复杂度。例如：

```c++
connect(sender, &Sender::signal, receiver, &Receiver::slot);
```

这一语句清晰地表明了信号与槽之间的关系。

信号与槽机制通过 `connect` 函数将事件的发出者和处理者连接起来，实现了组件间的解耦，简化了事件驱动编程。

### 1.3 嵌入式 C 编程中的信号与槽的优势

虽然信号与槽机制主要源于面向对象语言（如 C++ 和 Qt 框架），但它的思想同样可以借鉴到嵌入式 C 编程中，用于解决某些事件通信和模块解耦问题。

- **模块解耦**：信号与槽机制允许模块之间通过事件（信号）进行通信，而无需直接调用对方的接口，降低模块间的耦合度。
- **简化事件处理**：使用信号与槽，可以轻松管理事件的分发和处理逻辑，而不需要手动维护复杂的回调函数或状态机。
- **动态性和灵活性**：可以在运行时动态注册或解除信号与槽的绑定，支持灵活的模块间通信。
- **易于扩展**：新模块可以通过注册信号与槽轻松加入系统，无需修改现有模块的实现。
- **提升代码可读性与可维护性**：信号与槽机制的代码逻辑清晰，描述事件触发和响应关系的方式更直观。
- **线程安全**：如果设计得当，信号与槽机制可以用于不同任务（或线程）之间的安全通信，避免复杂的同步机制。



**既然信号槽有如此多的优点，嵌入式开发也想拥有，下边就开始用C语言一步步实现它吧...**

## 二、用C宏打造一个轻量级信号与槽机制
### 2.1 前景回顾

如果说上一篇文章通过[队列](..\queue\queue.md)的实现只是“浅尝辄止”，让我们领略了宏在代码简化和重用上的基本功，那么这篇文章，我们可以更进一步，将目光投向更复杂、更具挑战性的场景。通过深入挖掘宏的强大能力，我们不仅要展示它的“锦上添花”，更要看到它在解决复杂问题时“化繁为简”的真正价值。

### 2.2 基本功能封装

信号与槽机制的核心在于**信号与槽的动态连接与断开**，以及**通过信号调用槽函数完成事件通知**。

为此，我们先无脑定义一些与 Qt 中接口相同的宏，为后续的功能实现奠定基础：

```c

#define signals //定义信号

#define emit   //发射信号

#define slots  //定义槽

#define connect //链接信号与槽

#define disconnect //断开信号与槽


```

信号槽机制的本质是**发射信号时，通过信号与槽的对应关系找到槽函数并调用它**。我们需要实现的，正是这个机制。

###  2.3 实现声明信号的宏
在 C 语言中，信号本质上是一个函数，其触发的行为是调用与之绑定的槽函数。因此，我们需要通过 `signals` 宏声明信号的函数类型。

**实现逻辑**：

- `signals` 宏通过 `typedef` 声明信号函数类型。
- 发射信号时，会通过调用该函数类型的函数指针，执行槽函数。

**实现代码**：

```c
#define signals(__NAME,...)               \
          typedef void __NAME( __VA_ARGS__);
```
**示例**：

```c
signals(my_signal, int value, const char *message);
```

上述代码将定义一个信号 `my_signal`，它可以传递一个 `int` 和一个 `const char *` 参数。

###  2.4 实现发射信号的宏

发射信号的本质是通过函数指针调用槽函数。在实现发射信号的宏时，我们需要处理以下情况：

- 动态调用与信号绑定的槽函数。

- 支持不定长参数传递。

**实现步骤**：

1. 通过信号的函数指针调用槽函数。
2. 利用参数宏解析 `...` 的参数数量，从而正确调用不同参数数量的槽函数。

**实现代码**：

```c
#define __emit(__OBJ,...) \
           __PLOOC_EVAL(__RecFun_,##__VA_ARGS__) ((__OBJ)->ptRecObj,##__VA_ARGS__); 

#define emit(__NAME,__OBJ,...)                     \
           do {                                      \
               sig_slot_t *ptObj = &((__OBJ)->tObject); \
               __NAME *__RecFun = ptObj->ptRecFun;     \
               __emit(ptObj, __VA_ARGS__)              \
           } while(0)
```
在这里：

- `__PLOOC_EVAL` 是一个辅助宏，根据参数数量自动选择对应的 `__RecFun_N` 宏。
- `__RecFun_N` 定义了多种参数调用的槽函数适配逻辑。

```c
#define __RecFun_0(__OBJ)                                     \
            __RecFun((__OBJ))

#define __RecFun_1(__OBJ, __ARG1)                         \
            __RecFun((__OBJ),(__ARG1))

#define __RecFun_2(__OBJ, __ARG1, __ARG2)                         \
            __RecFun((__OBJ),(__ARG1), (__ARG2))

#define __RecFun_3(__OBJ, __ARG1, __ARG2, __ARG3)                 \
            __RecFun((__OBJ),(__ARG1), (__ARG2), (__ARG3))                         

#define __RecFun_4(__OBJ, __ARG1, __ARG2, __ARG3, __ARG4)                 \
            __RecFun((__OBJ),(__ARG1), (__ARG2), (__ARG3), (__ARG4))    
            
#define __RecFun_5(__OBJ, __ARG1, __ARG2, __ARG3, __ARG4, __ARG5)                 \
            __RecFun((__OBJ),(__ARG1), (__ARG2), (__ARG3), (__ARG4), (__ARG5))  

#define __RecFun_6(__OBJ, __ARG1, __ARG2, __ARG3, __ARG4, __ARG5, __ARG6)                 \
            __RecFun((__OBJ),(__ARG1), (__ARG2), (__ARG3), (__ARG4), (__ARG5), (__ARG6)) 

#define __RecFun_7(__OBJ, __ARG1, __ARG2, __ARG3, __ARG4, __ARG5, __ARG6, __ARG7)                 \
            __RecFun((__OBJ),(__ARG1), (__ARG2), (__ARG3), (__ARG4), (__ARG5), (__ARG6), (__ARG7)) 

#define __RecFun_8(__OBJ, __ARG1, __ARG2, __ARG3, __ARG4, __ARG5, __ARG6, __ARG7, __ARG8)                 \
            __RecFun((__OBJ),(__ARG1), (__ARG2), (__ARG3), (__ARG4), (__ARG5), (__ARG6), (__ARG7), (__ARG8)) 
 

```



###  2.5 取代QObject类
为了在结构体中存储信号与槽的元信息，我们定义一个 `sig_slot_t` 结构体，用于管理信号与槽的动态连接。

```c
typedef struct sig_slot_t sig_slot_t;
typedef struct sig_slot_t {
    char   chSenderName[SIG_NAME_MAX]; // 信号名称
    void  *ptSenderObj;               // 信号所在对象地址
    void  *ptRecObj;                  // 槽所在对象地址
    void  *ptRecFun;                  // 槽函数的地址
} sig_slot_t;

#define SIG_SLOT_OBJ  sig_slot_t tObject;

```
在需要信号支持的结构体中，添加 `SIG_SLOT_OBJ` 即可。

**示例**：

```c
typedef struct {
    SIG_SLOT_OBJ
} my_signal_object_t;
```

###  2.6 实现connect函数
`connect` 宏用于动态建立信号与槽的连接关系。
核心逻辑包括：

- 检查参数有效性。

- 将信号的函数地址与槽函数地址绑定。

```c
void connect(void *SenderObj, const char *ptSender, void *RecObj, void *RecFun) {
    if (SenderObj == NULL || ptSender == NULL || RecObj == NULL || RecFun == NULL) {
        return;
    }
    sig_slot_t *ptMetaObj = SenderObj;
    ptMetaObj->ptRecFun = RecFun;
    ptMetaObj->ptRecObj = RecObj;
    memcpy(ptMetaObj->chSenderName, ptSender, strlen(ptSender));
}
```
###  2.7 可选的 slots 宏
虽然槽函数在本质上是普通函数，但为了和 `signals` 宏对应，我们可以通过 `slots` 宏为槽函数提供一个语法糖，方便统一管理。

**实现代码**：

```
#define __slots(__NAME,...)             \
            void __NAME(__VA_ARGS__);
            
#define slots(__NAME,__OBJ,...)  \
            __slots(__NAME,_args(__OBJ,##__VA_ARGS__))
```

**示例**：

```
slots(my_slot, void *obj, int value, const char *message);
```

### **2.8 设计总结**

到此为止，我们实现了信号与槽的以下核心功能：

- **信号声明**：通过 `signals` 宏定义信号函数类型。

- **信号发射**：通过 `emit` 宏调用信号对应的槽函数。

- **动态连接**：通过 `connect` 宏在运行时动态绑定信号与槽。

- **统一语法**：使用 `slots` 宏声明槽函数，与信号配合使用。

在这一机制下，C 语言通过宏实现了类似 Qt 的信号与槽机制，为模块化和解耦设计提供了强大的工具支持。

## 三、完整的代码实现

以上代码只是展示核心部分，并且仅实现了一个信号对应一个槽，不能一个信号对应多个信号和槽，还有诸多类型检查，空指针检查等需要优化的地方，完整的代码开源链接：https://github.com/Aladdin-Wang/signals_slots

完整代码信号与槽具备以下核心特征：

- **不定长参数支持**：信号和槽函数可携带任意参数；

- **多路复用能力**：支持一对多、多对多的灵活连接；
- **连接与断开机制**：运行时动态连接和断开信号与槽；

- **类型安全与检查**：防止重复连接或非法操作；
- **链表结构**：高效管理信号与槽的关系；
- **宏封装**：简化信号与槽的定义和操作，提升代码可读性和可维护性。



## 四、使用方法与QT中的区别

###  1. SIG_SLOT_OBJ取代QObject
 SIG_SLOT_OBJ取代QObject，且只需要在信号所在的类中定义。

###  2. 定义信号不同
QT在类里面声明信号，signals宏是在结构体外声明信号，并且要指定信号名称，信号所在的对象地址，和一些自定义的参数：
```c
  signals(__NAME,__OBJ,...) 
  example：
  signals(send_sig,can_data_msg_t *ptThis,
      args(              
            uint8_t *pchByte,
            uint16_t hwLen
          ));
```
###  3. 发射信号不同
emit宏的括号内需要指定信号名称，信号所在的对象地址，和自定义的参数的数据：
```c
   emit(__NAME,__OBJ,...) 
   example：  
   emit(send_sig,&tCanMsgObj,
      args(
            tCanMsgObj.CanDATA.B,
            tCanMsgObj.CanDLC
         ));
```
###  4. 定义槽不同
与定义信号语法类似
```c
   slots(__NAME,__OBJ,...) 
   example：
   slots(enqueue_bytes,byte_queue_t *ptObj,
      args(
            void *pchByte,
            uint16_t hwLength
         ));
```
###  5. 连接信号与槽
与QT一样一个信号可以连接多个信号或者槽

```c
#define connect(__SIG_OBJ,__SIG_NAME,__SLOT_OBJ,__SLOT_FUN)    \
            direct_connect(__SIG_OBJ.tObject,__SIG_NAME,__SLOT_OBJ,__SLOT_FUN)

  example：
  connect(&tCanMsgObj,SIGNAL(send_sig),&s_tFIFOin,SLOT(enqueue_bytes));

#define disconnect(__SIG_OBJ,__SIG_NAME)    \
            auto_disconnect(__SIG_OBJ.tObject,__SIG_NAME)
  example：
  connect(&tCanMsgObj,SIGNAL(send_sig));
```
信号与槽的链接关系是同步调用的关系，不同于发布订阅系统的异步调用。

**同步调用的特点：**

- 当一个信号发出时，如果有多个槽连接到该信号，所有槽函数会按顺序被调用。
- 槽函数在信号发出时会立即执行，且不会返回控制权给发出信号的对象，直到所有槽函数执行完毕才会返回。
- 这种调用是同步的，因为发出信号时会等待槽函数的执行完成后才继续进行其他操作。

**与发布-订阅模式的区别：**

发布-订阅模式通常是基于异步通信的，其中发布者发送消息后，不会阻塞等待订阅者的处理结果。订阅者会异步地处理这些消息，并可能在处理完后通过回调等机制通知发布者。

**下一篇将结合队列，实现一个异步调用的发布-订阅模式，敬请期待。**

## 五、信号与槽使用示例

信号与槽模块作为我开源代码[MicroBoot](..\..\index.md)的核心机制，不仅提升了代码的可维护性和灵活性，还带来了更好的开发体验。

接下来实现一个将CAN接收的数据，存储到环形队列ringbuf的例子：

can.h文件

```c
#include "signals_slots.h"
typedef struct
{
    SIG_SLOT_OBJ;
    uint8_t  CanDLC;
    union {
        uint8_t B[8];
        uint16_t H[4];
        uint32_t W[2];
    } CanDATA;
}can_data_msg_t;

signals(send_sig,can_data_msg_t *ptThis,
      args(              
            uint8_t *pchByte,
            uint16_t hwLen
          ));
          
extern can_data_msg_t tCanMsgObj;          
```
can.c文件
```c
#include "can.h"
can_data_msg_t tCanMsgObj;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	HAL_StatusTypeDef status;
	CAN_RxHeaderTypeDef rxheader = {0};

	status = HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxheader, (uint8_t *)tCanMsgObj.CanDATA.B);
	if (HAL_OK != status)
			return ;
	/* get id */
	if (CAN_ID_STD == rxheader.IDE)
	{
		tCanMsgObj.CanID_t.CanID = rxheader.StdId;
	}
	else
	{
		tCanMsgObj.CanID_t.CanID = rxheader.ExtId;
	}
	/* get len */
	tCanMsgObj.CanDLC = rxheader.DLC;	

    emit(send_sig,&tCanMsgObj,
        args(
            tCanMsgObj.CanDATA.B,
            tCanMsgObj.CanDLC
         ));
}
```

main.c文件

```c
#include "signals_slots.h"
#include "can.h"

static uint8_t s_cFIFOinBuffer[1024];
static byte_queue_t s_tFIFOin;

MX_CAN1_Init();
HAL_CAN_Start(&hcan1);
HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
QUEUE_INIT(&s_tFIFOin, s_cFIFOinBuffer, sizeof(s_cFIFOinBuffer));
int main(void)
{
    connect(&tCanMsgObj,SIGNAL(send_sig),&s_tFIFOin,SLOT(enqueue_bytes));
    while(1){
     //do something
    }
}
```

