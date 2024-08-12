/****************************************************************************
*  Copyright 2022 kk (https://github.com/Aladdin-Wang)                                    *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

#ifndef __SERVE_SUBSCRIBE_AND_PUBLISH_H_
#define __SERVE_SUBSCRIBE_AND_PUBLISH_H_
#include "./app_cfg.h"
#if defined(USING_SUBSCRIBE_PUBLISH)
#include "../msg_map/msg_map.h"
#include "../signals_slots/signals_slots.h"
#include "../../generic/queue/ring_queue.h"
#ifdef __cplusplus
extern "C" {
#endif

/* example:

  //初始化发布订阅模块
	subscribe_publish_init(&tShellSubPub);
	
  //定义topic
  def_topic( __OBJ,__TOPIC, ...)  
  example：
  def_topic(&tShellSubPub, echo_topic, uint8_t*, uint16_t);
	
  //发布topic
  publish( __obj, __topic, ...)  
  example：
  publish(&tShellSubPub, __MSG_TOPIC(echo_topic), pchData, hwLength);

  //订阅topic
  subscribe( __SentObj, __topic,__RecObj, __callBack)
  example：
  subscribe(&tShellSubPub, __MSG_TOPIC(echo_topic), &ptObj, wl_shell_echo);
*/

/********************************************************************************************/
#define MSG_FUNCTION_EXPORT_CMD(name, cmd, desc)                      \
    const char __vsym_##cmd##_name[] __section(".rodata.name") = #cmd;    \
    const char __vsym_##cmd##_desc[] __section(".rodata.name") = #desc;   \
    const __used struct _msg_t __vsym_##cmd __section("VSymTab")={ \
          __vsym_##cmd##_name,    \
          (msg_hanlder_t *)&name, \
          __vsym_##cmd##_desc,    \
    };                            \
		COMPILER_ASSERT((IS_FUNCTION_POINTER(name)));
/**
 * @ingroup msg
 *
 * This macro exports a command to module shell.
 *
 * @param command is the name of the command.
 * @param desc is the description of the command, which will show in help list.
 */
#define MSG_CMD_EXPORT(command, desc)   \
    MSG_FUNCTION_EXPORT_CMD(command, command, desc)


typedef struct wl_subscribe_publish_t {
    SIG_SLOT_OBJ;
    fsm(search_msg_map)  fsmSearchTopicMap;
    byte_queue_t         tByteInQueue;
    get_byte_t           tGetByte;
    uint8_t              chQueueBuf[MSG_ARG_LEN];
} wl_subscribe_publish_t;

#define __MSG_TOPIC(x)   SIGNAL(x)

#define __SIGNALS_0(__OBJ,__TOPIC)                             \
    signals(__TOPIC,wl_subscribe_publish_t *ptThis);          \
    static int CONNECT2(__TOPIC,_fun)(int argc, char **argv) \
    {                         \
        emit(__TOPIC,__OBJ);	    \
        return 0;          \
    }  \
    MSG_FUNCTION_EXPORT_CMD(CONNECT2(__TOPIC,_fun),__TOPIC,__TOPIC)

#define __SIGNALS_1(__OBJ,__TOPIC,__TYPE1)             \
    signals(__TOPIC,wl_subscribe_publish_t *ptThis,    \
            args(                                         \
                    __TYPE1                            \
                ));                                    \
    static int CONNECT2(__TOPIC,_fun)(int argc, char **argv) \
    {                         \
        emit(__TOPIC,__OBJ,                \
             args(                            \
                    *((__TYPE1*)argv[1])     \
                 ));	    \
        return 0;          \
    }  \
    MSG_FUNCTION_EXPORT_CMD(CONNECT2(__TOPIC,_fun),__TOPIC,__TOPIC)

#define __SIGNALS_2(__OBJ,__TOPIC,__TYPE1,__TYPE2)     \
    signals(__TOPIC,wl_subscribe_publish_t *ptThis,    \
            args(                                         \
                    __TYPE1,                            \
                    __TYPE2                              \
                ));                                    \
    static int CONNECT2(__TOPIC,_fun)(int argc, char **argv) \
    {                         \
        emit(__TOPIC,__OBJ,                \
             args(                            \
                     *((__TYPE1*)argv[1]),     \
                     *((__TYPE2*)argv[2])     \
                 ));	    \
        return 0;          \
    }  \
    MSG_FUNCTION_EXPORT_CMD(CONNECT2(__TOPIC,_fun),__TOPIC,__TOPIC)

#define __SIGNALS_3(__OBJ,__TOPIC,__TYPE1,__TYPE2,__TYPE3)     \
    signals(__TOPIC,wl_subscribe_publish_t *ptThis,    \
            args(                                         \
                     __TYPE1,                            \
                     __TYPE2,                              \
                     __TYPE3                              \
                ));                                    \
    static int CONNECT2(__TOPIC,_fun)(int argc, char **argv) \
    {                         \
        emit(__TOPIC,__OBJ,                \
             args(                            \
                     *((__TYPE1*)argv[1]),     \
                     *((__TYPE2*)argv[2]),     \
                     *((__TYPE3*)argv[3])     \
                 ));	    \
        return 0;          \
    }  \
    MSG_FUNCTION_EXPORT_CMD(CONNECT2(__TOPIC,_fun),__TOPIC,__TOPIC)

#define __SIGNALS_4(__OBJ,__TOPIC,__TYPE1,__TYPE2,__TYPE3,__TYPE4)     \
    signals(__TOPIC,wl_subscribe_publish_t *ptThis,    \
            args(                                         \
                    __TYPE1,                            \
                    __TYPE2,                              \
                    __TYPE3,                              \
                    __TYPE4                              \
                ));                                    \
    static int CONNECT2(__TOPIC,_fun)(int argc, char **argv) \
    {                         \
        emit(__TOPIC,__OBJ,                \
             args(                            \
                     *((__TYPE1*)argv[1]),     \
                     *((__TYPE2*)argv[2]),     \
                     *((__TYPE3*)argv[3]),     \
                     *((__TYPE4*)argv[4])     \
                 ));	    \
        return 0;          \
    }  \
    MSG_FUNCTION_EXPORT_CMD(CONNECT2(__TOPIC,_fun),__TOPIC,__TOPIC)

#define __SIGNALS_5(__OBJ,__TOPIC,__TYPE1,__TYPE2,__TYPE3,__TYPE4,__TYPE5)     \
    signals(__TOPIC,wl_subscribe_publish_t *ptThis,    \
            args(                                         \
                    __TYPE1,                            \
                    __TYPE2,                              \
                    __TYPE3,                              \
                    __TYPE4,                              \
                    __TYPE5                              \
                ));                                    \
    static int CONNECT2(__TOPIC,_fun)(int argc, char **argv) \
    {                         \
        emit(__TOPIC,__OBJ,                \
             args(                            \
                    *((__TYPE1*)argv[1]),     \
                    *((__TYPE2*)argv[2]),     \
                    *((__TYPE3*)argv[3]),     \
                    *((__TYPE4*)argv[4]),     \
                    *((__TYPE5*)argv[5])     \
                 ));	    \
        return 0;          \
    }  \
    MSG_FUNCTION_EXPORT_CMD(CONNECT2(__TOPIC,_fun),__TOPIC,__TOPIC)


#define __SIGNALS_6(__OBJ,__TOPIC,__TYPE1,__TYPE2,__TYPE3,__TYPE4,__TYPE5,__TYPE6)     \
    signals(__TOPIC,wl_subscribe_publish_t *ptThis,    \
            args(                                         \
                    __TYPE1,                            \
                    __TYPE2,                              \
                    __TYPE3,                              \
                    __TYPE4,                              \
                    __TYPE5,                              \
                    __TYPE6                              \
                ));                                    \
    static int CONNECT2(__TOPIC,_fun)(int argc, char **argv) \
    {                         \
        emit(__TOPIC,__OBJ,                \
             args(                            \
                    *((__TYPE1*)argv[1]),     \
                    *((__TYPE2*)argv[2]),     \
                    *((__TYPE3*)argv[3]),     \
                    *((__TYPE4*)argv[4]),     \
                    *((__TYPE5*)argv[5]),     \
                    *((__TYPE6*)argv[6])     \
                 ));	    \
        return 0;          \
    }  \
    MSG_FUNCTION_EXPORT_CMD(CONNECT2(__TOPIC,_fun),__TOPIC,__TOPIC)

#define __SIGNALS_7(__OBJ,__TOPIC,__TYPE1,__TYPE2,__TYPE3,__TYPE4,__TYPE5,__TYPE6,__TYPE7)     \
    signals(__TOPIC,wl_subscribe_publish_t *ptThis,    \
            args(                                         \
                    __TYPE1,                            \
                    __TYPE2,                              \
                    __TYPE3,                              \
                    __TYPE4,                              \
                    __TYPE5,                              \
                    __TYPE6,                              \
                    __TYPE7                              \
                ));                                    \
    static int CONNECT2(__TOPIC,_fun)(int argc, char **argv) \
    {                         \
        emit(__TOPIC,__OBJ,                \
             args(                            \
                    *((__TYPE1*)argv[1]),     \
                    *((__TYPE2*)argv[2]),     \
                    *((__TYPE3*)argv[3]),     \
                    *((__TYPE4*)argv[4]),     \
                    *((__TYPE5*)argv[5]),     \
                    *((__TYPE6*)argv[6]),    \
                    *((__TYPE7*)argv[7])     \
                 ));	    \
        return 0;          \
    }  \
    MSG_FUNCTION_EXPORT_CMD(CONNECT2(__TOPIC,_fun),__TOPIC,__TOPIC)

#define __SIGNALS_8(__OBJ,__TOPIC,__TYPE1,__TYPE2,__TYPE3,__TYPE4,__TYPE5,__TYPE6,__TYPE7,__TYPE8)     \
    signals(__TOPIC,wl_subscribe_publish_t *ptThis,    \
            args(                                         \
                    __TYPE1,                            \
                    __TYPE2,                              \
                    __TYPE3,                              \
                    __TYPE4,                              \
                    __TYPE5,                              \
                    __TYPE6,                              \
                    __TYPE7,                              \
                    __TYPE8                              \
                ));                                    \
    static int CONNECT2(__TOPIC,_fun)(int argc, char **argv) \
    {                         \
        emit(__TOPIC,__OBJ,              \
             args(                        \
                    *((__TYPE1*)argv[1]),    \
                    *((__TYPE2*)argv[2]),    \
                    *((__TYPE3*)argv[3]),    \
                    *((__TYPE4*)argv[4]),    \
                    *((__TYPE5*)argv[5]),    \
                    *((__TYPE6*)argv[6]),    \
                    *((__TYPE7*)argv[7]),    \
                    *((__TYPE8*)argv[8])     \
                 ));	    \
        return 0;          \
    }  \
    MSG_FUNCTION_EXPORT_CMD(CONNECT2(__TOPIC,_fun),__TOPIC,__TOPIC)


#define def_topic( __OBJ,__TOPIC, ...)              \
    CONNECT2(__SIGNALS_,__PLOOC_VA_NUM_ARGS(__VA_ARGS__))              \
    (__OBJ,__TOPIC,##__VA_ARGS__);            \


#define __PUBLISH_TOPIC_0( __OBJ, __TOPIC)  \
    ({wl_subscribe_publish_t *(ptThis) = __OBJ;\
        enqueue(&ptThis->tByteInQueue,__TOPIC,strlen(__TOPIC));\
        enqueue(&ptThis->tByteInQueue,(uint8_t)0);})

#define __PUBLISH_TOPIC_1( __OBJ, __TOPIC, __DATA1)  \
    ({wl_subscribe_publish_t *(ptThis) = __OBJ;\
        enqueue(&ptThis->tByteInQueue,__TOPIC,strlen(__TOPIC));\
        enqueue(&ptThis->tByteInQueue,(uint8_t)1);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA1));\
        enqueue(&ptThis->tByteInQueue,__DATA1);})


#define __PUBLISH_TOPIC_2( __OBJ, __TOPIC, __DATA1, __DATA2)  \
    ({wl_subscribe_publish_t *(ptThis) = __OBJ;\
        enqueue(&ptThis->tByteInQueue,__TOPIC,strlen(__TOPIC));\
        enqueue(&ptThis->tByteInQueue,(uint8_t)2);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA1));\
        enqueue(&ptThis->tByteInQueue,__DATA1);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA2));\
        enqueue(&ptThis->tByteInQueue,__DATA2);})


#define __PUBLISH_TOPIC_3( __OBJ, __TOPIC, __DATA1, __DATA2, __DATA3) \
    ({wl_subscribe_publish_t *(ptThis) = __OBJ;\
        enqueue(&ptThis->tByteInQueue,__TOPIC,strlen(__TOPIC));\
        enqueue(&ptThis->tByteInQueue,(uint8_t)3);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA1));\
        enqueue(&ptThis->tByteInQueue,__DATA1);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA2));\
        enqueue(&ptThis->tByteInQueue,__DATA2);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA3));\
        enqueue(&ptThis->tByteInQueue,__DATA3);\
    })

#define __PUBLISH_TOPIC_4( __OBJ, __TOPIC, __DATA1, __DATA2, __DATA3, __DATA4)  \
    ({wl_subscribe_publish_t *(ptThis) = __OBJ;\
        enqueue(&ptThis->tByteInQueue,__TOPIC,strlen(__TOPIC));\
        enqueue(&ptThis->tByteInQueue,(uint8_t)4);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA1));\
        enqueue(&ptThis->tByteInQueue,__DATA1);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA2));\
        enqueue(&ptThis->tByteInQueue,__DATA2);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA3));\
        enqueue(&ptThis->tByteInQueue,__DATA3);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA4));\
        enqueue(&ptThis->tByteInQueue,__DATA4);\
    })

#define __PUBLISH_TOPIC_5( __OBJ, __TOPIC, __DATA1, __DATA2, __DATA3, __DATA4, __DATA5) \
    ({wl_subscribe_publish_t *(ptThis) = __OBJ;\
        enqueue(&ptThis->tByteInQueue,__TOPIC,strlen(__TOPIC));\
        enqueue(&ptThis->tByteInQueue,(uint8_t)5);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA1));\
        enqueue(&ptThis->tByteInQueue,__DATA1);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA2));\
        enqueue(&ptThis->tByteInQueue,__DATA2);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA3));\
        enqueue(&ptThis->tByteInQueue,__DATA3);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA4));\
        enqueue(&ptThis->tByteInQueue,__DATA4);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA5));\
        enqueue(&ptThis->tByteInQueue,__DATA5);\
    })

#define __PUBLISH_TOPIC_6( __OBJ, __TOPIC, __DATA1, __DATA2, __DATA3, __DATA4, __DATA5, __DATA6) \
    ({wl_subscribe_publish_t *(ptThis) = __OBJ;\
        enqueue(&ptThis->tByteInQueue,__TOPIC,strlen(__TOPIC));\
        enqueue(&ptThis->tByteInQueue,(uint8_t)6);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA1));\
        enqueue(&ptThis->tByteInQueue,__DATA1);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA2));\
        enqueue(&ptThis->tByteInQueue,__DATA2);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA3));\
        enqueue(&ptThis->tByteInQueue,__DATA3);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA4));\
        enqueue(&ptThis->tByteInQueue,__DATA4);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA5));\
        enqueue(&ptThis->tByteInQueue,__DATA5);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA6));\
        enqueue(&ptThis->tByteInQueue,__DATA6);\
    })

#define __PUBLISH_TOPIC_7( __OBJ, __TOPIC, __DATA1, __DATA2, __DATA3, __DATA4, __DATA5, __DATA6, __DATA7)  \
    ({wl_subscribe_publish_t *(ptThis) = __OBJ;\
        enqueue(&ptThis->tByteInQueue,__TOPIC,strlen(__TOPIC));\
        enqueue(&ptThis->tByteInQueue,(uint8_t)7);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA1));\
        enqueue(&ptThis->tByteInQueue,__DATA1);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA2));\
        enqueue(&ptThis->tByteInQueue,__DATA2);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA3));\
        enqueue(&ptThis->tByteInQueue,__DATA3);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA4));\
        enqueue(&ptThis->tByteInQueue,__DATA4);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA5));\
        enqueue(&ptThis->tByteInQueue,__DATA5);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA6));\
        enqueue(&ptThis->tByteInQueue,__DATA6);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA7));\
        enqueue(&ptThis->tByteInQueue,__DATA7);\
    })

#define __PUBLISH_TOPIC_8( __OBJ, __TOPIC, __DATA1, __DATA2, __DATA3, __DATA4, __DATA5, __DATA6, __DATA7, __DATA8)  \
    ({wl_subscribe_publish_t *(ptThis) = __OBJ;\
        enqueue(&ptThis->tByteInQueue,__TOPIC,strlen(__TOPIC));\
        enqueue(&ptThis->tByteInQueue,(uint8_t)8);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA1));\
        enqueue(&ptThis->tByteInQueue,__DATA1);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA2));\
        enqueue(&ptThis->tByteInQueue,__DATA2);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA3));\
        enqueue(&ptThis->tByteInQueue,__DATA3);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA4));\
        enqueue(&ptThis->tByteInQueue,__DATA4);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA5));\
        enqueue(&ptThis->tByteInQueue,__DATA5);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA6));\
        enqueue(&ptThis->tByteInQueue,__DATA6);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA7));\
        enqueue(&ptThis->tByteInQueue,__DATA7);\
        enqueue(&ptThis->tByteInQueue,(uint16_t)sizeof(__DATA8));\
        enqueue(&ptThis->tByteInQueue,__DATA8);\
    })

#define publish( __obj, __topic, ...)                         \
    CONNECT2(__PUBLISH_TOPIC_,__PLOOC_VA_NUM_ARGS(__VA_ARGS__))              \
    (__obj,__topic,##__VA_ARGS__);

#define subscribe( __SentObj, __topic,__RecObj, __callBack)                \
    connect(__SentObj, __topic, __RecObj, __callBack);

#define unsubscribe( __SentObj, __topic,__RecObj, __callBack)                \
    disconnect(__SentObj, __topic, __RecObj, __callBack);

extern wl_subscribe_publish_t *wl_subscribe_publish_init(wl_subscribe_publish_t *ptObj);
extern void wl_subscribe_publish_exec(wl_subscribe_publish_t *ptObj);
#ifdef __cplusplus
}
#endif
#endif
#endif /* MSG_MAP_MSG_MAP_H_ */


