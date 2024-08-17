---
# 一、介绍
一个C语言编写的支持任意类型的环形队列。

# 二、API 接口

```c
#define QUEUE_INIT(__QUEUE, __BUFFER, __BUFFER_SIZE )                           \
            queue_init_byte(__QUEUE, __BUFFER, __BUFFER_SIZE )

#define DEQUEUE(__QUEUE, __ADDR,...)                                            \
            __PLOOC_EVAL(__DEQUEUE_,##__VA_ARGS__)                              \
                (__QUEUE,__ADDR,##__VA_ARGS__)                 

#define ENQUEUE(__QUEUE, __ADDR,...)                                            \
            __PLOOC_EVAL(__ENQUEUE_,##__VA_ARGS__)                              \
                (__QUEUE,__ADDR,##__VA_ARGS__) 

#define PEEK_QUEUE(__QUEUE, __ADDR,...)                                         \
            __PLOOC_EVAL(__PEEK_QUEUE_,##__VA_ARGS__)                           \
                (__QUEUE,__ADDR,##__VA_ARGS__) 

#define IS_ENQUEUE_EMPTY(__QUEUE)                                               \
            is_byte_queue_empty(__QUEUE)

#define RESET_PEEK(__QUEUE)                                                     \
            reset_peek_byte(__QUEUE)

#define GET_ALL_PEEKED(__QUEUE)                                                 \
            get_all_peeked_byte(__QUEUE)

#define GET_PEEK_STATUS(__QUEUE)                                                \
            get_peek_status(__QUEUE)

#define RESTORE_PEEK_STATUS(__QUEUE,__COUNT)                                    \
            restore_peek_status(__QUEUE,__COUNT)

#define GET_QUEUE_COUNT(__QUEUE)                                                \
            get_queue_count(__QUEUE)

extern byte_queue_t * queue_init_byte(byte_queue_t *ptObj, void *pBuffer, uint16_t hwItemSize);
extern bool enqueue_byte(byte_queue_t *ptQueue, uint8_t chByte);
extern int16_t enqueue_bytes(byte_queue_t *ptObj, void *pchByte, uint16_t hwLength);
extern bool dequeue_byte(byte_queue_t *ptQueue, uint8_t *pchByte);
extern int16_t dequeue_bytes(byte_queue_t *ptObj, void *pchByte, uint16_t hwLength);
extern bool is_byte_queue_empty(byte_queue_t *ptQueue);
extern bool peek_byte_queue(byte_queue_t *ptQueue, uint8_t *pchByte);
extern bool reset_peek_byte(byte_queue_t *ptQueue);
extern bool get_all_peeked_byte(byte_queue_t *ptQueue);
extern int16_t peek_bytes_queue(byte_queue_t *ptObj, void *pchByte, uint16_t hwLength);
extern uint16_t get_peek_status(byte_queue_t *ptQueue);
extern bool restore_peek_status(byte_queue_t *ptQueue ,uint16_t hwCount);
extern int16_t get_queue_count(byte_queue_t *ptObj);
extern int16_t get_queue_available_count(byte_queue_t *ptObj);

```

#  三、API 说明


## 1. 初始化队列

```c
QUEUE_INIT(__QUEUE, __BUFFER, __BUFFER_SIZE ) 
```
参数说明：
| 参数名        | 描述             |
| ------------- | ---------------- |
| __QUEUE       | 队列的地址       |
| __BUFFER      | 队列缓存的首地址 |
| __BUFFER_SIZE | 队列长度         |

初始化队列之前首先需要通过byte_queue_t 结构体定义一个队列对象，和缓冲区的buf。
参考代码：

```c
uint8_t s_cFIFOinBuffer[1024];
static byte_queue_t s_tFIFOin;
QUEUE_INIT(&s_tFIFOin, s_cFIFOinBuffer, sizeof(s_cFIFOinBuffer));
```

## 2. 入队

```c
#define ENQUEUE(__QUEUE, __ADDR,...)  
```
参数说明：
| 参数名  | 描述                                                         |
| ------- | ------------------------------------------------------------ |
| __QUEUE | 队列的地址                                                   |
| __ADDR  | 待入队的数据或者数据的地址                                   |
| ...     | 可变参数，需要入队的数据个数，或者数据类型和个数，如果为空，则只入队一个数据 |

参考代码：

```c
    typedef struct data_t{
        uint32_t a;
        uint32_t b;
        uint32_t c;
    }data_t;
    
    uint8_t  data1 = 0XAA;
    uint16_t data2 = 0X55AA;
    uint32_t data3 = 0X55AAAA55;
    uint16_t data4[] = {0x1234,0x5678};

    data_t data5 = {
        .a = 0X11223344,
        .b = 0X55667788,
        .c = 0X99AABBCC,
    };
    // 一下三种方式都可以正确存储数组
    ENQUEUE(&s_tFIFOin,data4,2);//可以不指名数据类型
    ENQUEUE(&s_tFIFOin,data4,uint16_t,2);//也可以指名数据类型
    ENQUEUE(&s_tFIFOin,data4,uint8_t,sizeof(data4));//或者用字节类型

    //一下两种方式都可以正确存储结构体类型
    ENQUEUE(&s_tFIFOin,data5);//根据结构体的类型，自动计算结构体的大小
    ENQUEUE(&s_tFIFOin,&data5,uint8_t,sizeof(data5));//也可以以数组方式存储

    ENQUEUE(&s_tFIFOin,(uint8_t)0X11); //常量默认为int型，需要强制转换数据类型
    ENQUEUE(&s_tFIFOin,(uint16_t)0X2233); //常量默认为int型，需要强制转换数据类型
    ENQUEUE(&s_tFIFOin,0X44556677);
    ENQUEUE(&s_tFIFOin,(char)'a');//单个字符也需要强制转换数据类型
    ENQUEUE(&s_tFIFOin,"bc");//字符串默认会存储空字符\0
    ENQUEUE(&s_tFIFOin,"def");//字符串默认会存储空字符\0

```

## 3. 出队
```c
DEQUEUE(__QUEUE, __ADDR,...)  
```
参数说明：
| 参数名  | 描述                                                         |
| ------- | ------------------------------------------------------------ |
| __QUEUE | 队列的地址                                                   |
| __ADDR  | 用于保存出队数据变量的地址                                   |
| ...     | 可变参数，需要出队的数据个数，或者数据类型和个数，如果为空，则只出队一个数据 |

参考代码：

```c
   uint8_t  data[100];
   uint16_t  data1;
   uint32_t  data2;
// 读出全部数据
   DEQUEUE(&s_tFIFOin,data,GET_QUEUE_COUNT(&my_queue));
   DEQUEUE(&s_tFIFOin,data,uint8_t,GET_QUEUE_COUNT(&my_queue))
// 读一个数据，长度为uint16_t类型
   DEQUEUE(&s_tFIFOin,&data1);
// 读一个数据，长度为uint32_t类型
   DEQUEUE(&s_tFIFOin,&data2);
```

## 4. 查看
```c
#define PEEK_QUEUE(__QUEUE, __ADDR,...)   
```
参数说明：
| 参数名  | 描述                                                         |
| ------- | ------------------------------------------------------------ |
| __QUEUE | 队列的地址                                                   |
| __ADDR  | 用于保存查看数据变量的地址                                   |
| ...     | 可变参数，数据类型和需要查看的数据个数，如果为空，则只查看一个数据 |

参考代码：

```c
   uint8_t data[32];
   uint16_t  data1;
   uint32_t  data2;
// 查看多个数据
   PEEK_QUEUE(&s_tFIFOin,&data,sizeof(data));   
// 查看一个数据，长度为uint16_t类型
   PEEK_QUEUE(&s_tFIFOin,&data1);
// 查看一个数据，长度为uint32_t类型
   PEEK_QUEUE(&s_tFIFOin,&data2);
```

## 5. 其他API
- 队列是否为空
```c
IS_ENQUEUE_EMPTY(__QUEUE) 
```
- 复位PEEK

```c
RESET_PEEK(__QUEUE)
```
- 出队所有查看的数据

```c
GET_ALL_PEEKED(__QUEUE) 
```
- 获取PEEK的状态

```c
GET_PEEK_STATUS(__QUEUE)  
```
- 恢复PEEK的状态

```c
RESTORE_PEEK_STATUS(__QUEUE,__COUNT)  
```
- 获取队列的数据个数
```c
GET_QUEUE_COUNT(__QUEUE) 
```
#  四、多类型原理说明
以`DEQUEUE(__QUEUE, __ADDR,...)`  为例，说明如何做到支持任意类型的数据，和不同个数的参数类型。
```c
#define __DEQUEUE_0( __QUEUE, __ADDR)                                \
            dequeue_bytes((__QUEUE), __ADDR,(sizeof(typeof(*(__ADDR)))))

#define __DEQUEUE_1( __QUEUE, __ADDR, __ITEM_COUNT)                        \
            dequeue_bytes((__QUEUE), (__ADDR), __ITEM_COUNT*(sizeof(typeof((__ADDR[0])))))

#define __DEQUEUE_2( __QUEUE, __ADDR, __TYPE,__ITEM_COUNT)                 \
            dequeue_bytes((__QUEUE), (__ADDR), (__ITEM_COUNT * sizeof(__TYPE)))

#define DEQUEUE(__QUEUE, __ADDR,...)                                            \
            __PLOOC_EVAL(__DEQUEUE_,##__VA_ARGS__)                              \
                (__QUEUE,__ADDR,##__VA_ARGS__)  
```

宏DEQUEUE最终调用的是

```c
int16_t dequeue_bytes(byte_queue_t *ptObj, void *pchByte, uint16_t hwLength);
```
本队列默认只支持字节类型，而字节是最小单位的数据类型，它可以组合成其他的数据类型，所以只要知道其他数据类型的大小，就可以根据类型的大小，读出相对应类型的数据。
因此只需要利用下边两种技巧便可以达到目的：

**获取数据类型**
typeof() 是GUN C提供的一种特性，可参考C-Extensions，它可以取得变量的类型，或者表达式的类型。
使用typeof来获取接收地址的类型，然后通过sizeof获取类似的大小，从而确定需要读出的数据长度。
**宏的重载**
如果看过前边的文章[C语言变参函数和可变参数宏](https://blog.csdn.net/sinat_31039061/article/details/120496363?spm=1001.2014.3001.5502)，就可以发现这里其实使用的就是宏的重载，宏的重载原理已经在前边文章讲解过了，宏DEQUEUE直接使用了PLOOC已经实现好的`__PLOOC_EVAL`宏。

#  五、快速使用

```c
    #include "./queue/byte_queue.h"
    uint8_t  data1 = 0XAA;
    uint16_t data2 = 0X55AA;
    uint32_t data3 = 0X55AAAA55;
    uint16_t data4[] = {0x1234,0x5678};
    typedef struct data_t{
        uint32_t a;
        uint32_t b;
        uint32_t c;
    }data_t;
    data_t data5 = {
        .a = 0X11223344,
        .b = 0X55667788,
        .c = 0X99AABBCC,
    };

    uint8_t  data[100];
    static uint8_t s_hwQueueBuffer[100];
    static byte_queue_t my_queue;

    QUEUE_INIT(&my_queue,s_hwQueueBuffer,sizeof(s_hwQueueBuffer));

    QUEUE_INIT(&my_queue,s_hwQueueBuffer,sizeof(s_hwQueueBuffer));

    ENQUEUE(&my_queue,data1);//根据变量的类型，自动计算对象的大小
    ENQUEUE(&my_queue,data2);
    ENQUEUE(&my_queue,data3);

    // 一下三种方式都可以正确存储数组
    ENQUEUE(&my_queue,data4,2);//可以不指名数据类型
    ENQUEUE(&my_queue,data4,uint16_t,2);//也可以指名数据类型
    ENQUEUE(&my_queue,data4,uint8_t,sizeof(data4));//或者用其他类型

    //一下两种方式都可以正确存储结构体类型
    ENQUEUE(&my_queue,data5);//根据结构体的类型，自动计算对象的大小
    ENQUEUE(&my_queue,&data5,uint8_t,sizeof(data5));//也可以以数组方式存储

    ENQUEUE(&my_queue,(uint8_t)0X11); //常量默认为int型，需要强制转换数据类型
    ENQUEUE(&my_queue,(uint16_t)0X2233); //常量默认为int型，需要强制转换数据类型
    ENQUEUE(&my_queue,0X44556677);
    ENQUEUE(&my_queue,(char)'a');//单个字符也需要强制转换数据类型
    ENQUEUE(&my_queue,"bc");//字符串默认会存储空字符\0
    ENQUEUE(&my_queue,"def");

    // 读出全部数据
    DEQUEUE(&my_queue,data,GET_QUEUE_COUNT(&my_queue));//DEQUEUE(&my_queue,data,uint8_t,GET_QUEUE_COUNT(&my_queue))

```