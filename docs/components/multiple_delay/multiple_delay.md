# **多路软件延时服务**

**整体结构**

它对外暴露的是一个接口对象 MULTIPLE_DELAY，在头文件里能看到 5 个主要入口：

- Init
- Task
- RequestDelay
- Cancel
- Dependent.TimerTickService

你可以把它理解成两条并行通路：

1. TimerTickService()
   这个函数应该在“定时中断/tick 节拍”里周期调用，负责推进时间。
2. Task()
   这个函数在主循环里跑，负责处理普通/低优先级的超时事件。

也就是说，它不是简单的“每次 tick 扫一遍所有 timer 然后直接回调”，而是做了**优先级分层**：

- HIGH 优先级：在 TimerTickService() 里直接触发回调
- NORMAL / LOW 优先级：先挂入事件队列，再由 Task() 在主循环里分发

它的目的很明显：

- 高优先级延时超时，允许更及时地响应
- 普通和低优先级回调，不在 tick 上下文里直接执行，避免中断里做太重的事

------

**核心数据结构**

每一个延时请求对应一个 multiple_delay_item_t：

- wTargetTime：目标超时时刻
- tPriority：优先级
- pTag：用户自定义指针，回调时原样传回去
- fnHandler：超时/取消时调用的回调函数
- 还继承了一个单链表节点 __single_list_node_t

服务本体 multiple_delay_t 里有几组关键成员：

- ptHighPriorityDelayList：高优先级等待链表
- ptDelayList：普通 + 低优先级等待链表
- NormalPriorityEvent：普通优先级“待分发事件队列”
- LowPriorityEvent：低优先级“待分发事件队列”
- wCounter：当前 tick 计数
- wOldCounter / wSavedCounter：FSM 处理用的辅助计数

还有一个很关键的点：它不是 malloc/free，而是用了 EPOOL 内存池。也就是说：

- 初始化时你给它一块 buffer
- 每次 RequestDelay() 从池里拿一个 item
- 超时或取消后把 item 还回池里

这对 MCU 很友好，避免堆碎片。

------

**初始化在做什么**

init() 做了几件事：

1. 检查参数是否合法
2. memset(ptThis, 0, sizeof(multiple_delay_t))
3. 初始化 item 内存池 EPOOL_INIT
4. 把用户提供的 buffer 加入内存池 EPOOL_ADD_HEAP
5. 初始化内部状态机 multiple_delay_task

这里的 multiple_delay_cfg_t 本质上就是一个内存块配置，要求你传一块可供 item 分配的缓冲区。

所以这个模块的容量，不是动态无限的，而是由你给的 buffer 大小决定的。

------

**请求一个延时是怎么做的**

request_delay() 的流程：

1.检查参数

- ptObj != NULL

- wDelay != 0
- fnHandler != NULL

2.读取当前计数 wCurrentCounter = this.wCounter

3.做一个溢出保护：

`if ((uint32_t)(wCurrentCounter + wDelay) < wCurrentCounter) `

如果加法回绕了，就拒绝这次请求

4.从内存池分配一个 multiple_delay_item_t

5.填好字段：

- fnHandler
- pTag
- tPriority
- wTargetTime = wCurrentCounter + wDelay

6.按优先级插入对应链表：

- HIGH 进 ptHighPriorityDelayList
- NORMAL / LOW 进 ptDelayList

------

**链表为什么能高效工作**

add_to_delay_list() 会把 item 插入到一个**按目标时间升序排序**的单链表里。

关键判断是：

```
if (target.wTargetTime <= ptListItem->wTargetTime) 
```

也就是说，越早到期的 item 越靠前。

这带来一个直接好处：

- 检查超时时，不需要扫完整个链表
- 只需要一直看表头
- 一旦发现表头还没超时，后面的肯定也没超时，可以立刻停

这就是它为什么能在 MCU 上比较轻量。

------

**tick 到来时到底做了什么**

insert_timer_tick_event_handler() 是整个模块的“时间推进器”。

逻辑可以概括成：

1.先处理 HIGH 优先级链表表头

2.只要表头已经到时间：

- 把它从链表弹出
- 立即调用回调 fnHandler(MULTIPLE_DELAY_TIMEOUT, pTag)
- 释放 item

3.如果高优先级表头没超时，就停

4.最后 wCounter++

这里有个很重要的细节：

**它在 tick 函数里只直接处理高优先级链表，不处理普通/低优先级链表。**

普通/低优先级是交给 Task() 去做的。

------

**普通/低优先级为什么要走 FSM**

Task() 最终会调用内部 FSM：multiple_delay_task

它的职责不是“推进时间”，而是“消费已经到期的普通/低优先级事件”。

FSM 大致分三步：

1.on_start

- 把当前 wCounter 存到 wSavedCounter
- 如果 wOldCounter == wSavedCounter，说明 tick 没变化，这次就没必要继续做

2.CHECK_LIST

- 从 ptDelayList 表头开始检查
- 凡是 wTargetTime <= wSavedCounter 的 item 都弹出来
- 如果是 LOW，挂进 LowPriorityEvent
- 否则挂进 NormalPriorityEvent

3.RAISE_NORMAL_PRIORITY_EVENT

- 每次取一个普通优先级事件
- 调回调
- 释放 item
- 直到普通队列空了，再转去低优先级

4.RAISE_LOW_PRIORITY_EVENT

- 每次取一个低优先级事件
- 调回调
- 释放 item
- 队列空了就 fsm_cpl()

5.所以它的实际策略是：

- 先把已超时的 normal/low 请求，从“等待链表”搬到“事件队列”
- 再先处理 normal
- 最后处理 low

这就实现了一个很清晰的优先级层次：

**HIGH > NORMAL > LOW**

而且 NORMAL 和 LOW 都不会抢占 tick 处理。

------

**取消请求怎么做**

cancel_delay() 的逻辑是：

1.根据 ptItem->tPriority 推测它在哪个链表

2.尝试把它从等待链表中删除

3.删除成功后：

- 回调 fnHandler(MULTIPLE_DELAY_CANCELLED, pTag)
- 释放 item

注意一个边界：

- 它只尝试从“等待链表”里删
- 不会从 NormalPriorityEvent / LowPriorityEvent 这两个“已到期待分发队列”里删

也就是说，如果某个普通/低优先级 item 已经从等待链表转移到事件队列里了，但 Task() 还没来得及分发，此时再 Cancel()，理论上是**取消不掉**的。这个是这个实现的一个行为特征，使用时要知道。

------

**你在项目里应该怎么用**

典型流程会是这样：

1.准备一块 buffer 给内存池

2.MULTIPLE_DELAY.Init(&obj, &cfg)

3.在定时中断里周期调用：

- MULTIPLE_DELAY.Dependent.TimerTickService(&obj);

4.在主循环里反复调用：

- MULTIPLE_DELAY.Task(&obj);

5.需要一个延时时：

- MULTIPLE_DELAY.RequestDelay(&obj, delay, priority, tag, handler);

大概像这样：

```c
#include <stdio.h>
#include <stdint.h>
#include "multiple_delay.h"

static multiple_delay_t s_tDelayService;
static uint8_t s_chDelayPool[10 * sizeof(multiple_delay_item_t)];

typedef struct {
    char ch;
    uint32_t delay_tick;
    multiple_delay_request_priority_t priority;
} delay_task_param_t;

static delay_task_param_t s_tHighTask = {
    .ch = 'H',
    .delay_tick = 100,
    .priority = MULTIPLE_DELAY_HIGH_PRIORITY,
};

static delay_task_param_t s_tNormalTask = {
    .ch = 'N',
    .delay_tick = 200,
    .priority = MULTIPLE_DELAY_NORMAL_PRIORITY,
};

static delay_task_param_t s_tLowTask = {
    .ch = 'L',
    .delay_tick = 300,
    .priority = MULTIPLE_DELAY_LOW_PRIORITY,
};

void on_delay_event(multiple_delay_report_status_t status, void *pTag)
{
    delay_task_param_t *ptTask = (delay_task_param_t *)pTag;

    if (status == MULTIPLE_DELAY_TIMEOUT) {
        printf("%c", ptTask->ch);

        MULTIPLE_DELAY.RequestDelay(
            &s_tDelayService,
            ptTask->delay_tick,
            ptTask->priority,
            ptTask,
            on_delay_event
        );
    } else {
        printf("C");
    }
}

void app_init(void)
{
    multiple_delay_cfg_t cfg = {
        .pchBuffer = s_chDelayPool,
        .nSize = sizeof(s_chDelayPool),
    };

    MULTIPLE_DELAY.Init(&s_tDelayService, &cfg);

    MULTIPLE_DELAY.RequestDelay(
        &s_tDelayService,
        s_tHighTask.delay_tick,
        s_tHighTask.priority,
        &s_tHighTask,
        on_delay_event
    );

    MULTIPLE_DELAY.RequestDelay(
        &s_tDelayService,
        s_tNormalTask.delay_tick,
        s_tNormalTask.priority,
        &s_tNormalTask,
        on_delay_event
    );

    MULTIPLE_DELAY.RequestDelay(
        &s_tDelayService,
        s_tLowTask.delay_tick,
        s_tLowTask.priority,
        &s_tLowTask,
        on_delay_event
    );
}

void systick_handler(void)
{
    MULTIPLE_DELAY.Dependent.TimerTickService(&s_tDelayService);
}

void main(void)
{
    while (1) {
        MULTIPLE_DELAY.Task(&s_tDelayService);
    }
}

```

**几个你要特别注意的点**

- RequestDelay() 要求 fnHandler 不能是 NULL
- wDelay 不能是 0
- 可同时挂多少个 delay，取决于你给的 pool 大小
- 高优先级回调是在 tick 上下文执行的，所以回调必须很短，不能阻塞
- 普通/低优先级回调是在 Task() 执行的上下文里跑
- 如果 Task() 调得不勤，普通/低优先级超时事件会“延迟被处理”，但不会丢