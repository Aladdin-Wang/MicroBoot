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

#ifndef __SERVE_QUEUE_H_
#define __SERVE_QUEUE_H_
#include ".\app_cfg.h"
#if defined(USING_RINGEQUEUE)
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef __QUEUE_CFG_PORTING_INCLUDE__
#   include "queue_port_default.h"
#else
#   include __QUEUE_CFG_PORTING_INCLUDE__
#endif


#ifdef assert
#undef assert
#endif
#define assert(EXPR)                                                           \
if (!(EXPR))                                                                   \
{                                                                              \
    printf("(%s) has assert failed at %s.\n", #EXPR, __FUNCTION__);            \
    while (1);                                                                 \
}

#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__

#if     defined(__BYTE_QUEUE_CLASS_IMPLEMENT__)
    #define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__BYTE_QUEUE_CLASS_INHERIT__)
    #define __PLOOC_CLASS_INHERIT__
#endif

#include "..\..\plooc\plooc_class.h"

#undef __CONNECT2
#undef CONNECT2
#undef __CONNECT3
#undef CONNECT3

#define __CONNECT3(__A, __B, __C)         __A##__B##__C
#define __CONNECT2(__A, __B)              __A##__B

#define CONNECT3(__A, __B, __C)           __CONNECT3(__A, __B, __C)
#define CONNECT2(__A, __B)                __CONNECT2(__A, __B)

#ifndef SAFE_NAME
#define SAFE_NAME(__NAME)   CONNECT3(__,__NAME,__LINE__)
#endif

#ifndef safe_atom_code
      #define safe_atom_code()                                         \
              for(  uint32_t SAFE_NAME(temp) =                          \
                          ({uint32_t SAFE_NAME(temp2)=queue_port_disable_global_interrupt();  \
                       SAFE_NAME(temp2);}),*SAFE_NAME(temp3) = NULL;    \
                       SAFE_NAME(temp3)++ == NULL;                      \
                      queue_port_resume_global_interrupt(SAFE_NAME(temp)))				 					
#endif



#define __DEQUEUE_0( __QUEUE, __ADDR)                                \
    dequeue_bytes((__QUEUE), (__ADDR),(sizeof(typeof(*(__ADDR)))))

#define __DEQUEUE_1( __QUEUE, __ADDR, __ITEM_COUNT)                        \
    dequeue_bytes((__QUEUE), (__ADDR), __ITEM_COUNT*(sizeof(typeof((__ADDR[0])))))

#define __DEQUEUE_2( __QUEUE, __ADDR, __TYPE,__ITEM_COUNT)                 \
    dequeue_bytes((__QUEUE), (__ADDR), (__ITEM_COUNT * sizeof(__TYPE)))


#define __ENQUEUE_0( __QUEUE, __VALUE)                                     \
    ({typeof((__VALUE)) SAFE_NAME(value) = __VALUE;                     \
        enqueue_bytes((__QUEUE), &(SAFE_NAME(value)), (sizeof(__VALUE)));})

#define __ENQUEUE_1( __QUEUE, __ADDR, __ITEM_COUNT)                         \
    enqueue_bytes((__QUEUE), (__ADDR), __ITEM_COUNT*(sizeof(typeof((__ADDR[0])))))

#define __ENQUEUE_2( __QUEUE, __ADDR, __TYPE, __ITEM_COUNT)                 \
    enqueue_bytes((__QUEUE), (__ADDR), (__ITEM_COUNT * sizeof(__TYPE)))


#define __PEEK_QUEUE_0( __QUEUE, __ADDR)                               \
    peek_bytes_queue((__QUEUE), (__ADDR), (sizeof(typeof(*(__ADDR)))))

#define __PEEK_QUEUE_1( __QUEUE, __ADDR, __ITEM_COUNT)                       \
    peek_bytes_queue((__QUEUE), (__ADDR), __ITEM_COUNT*(sizeof(typeof((__ADDR[0])))))

#define __PEEK_QUEUE_2( __QUEUE, __ADDR, __TYPE, __ITEM_COUNT)               \
    peek_bytes_queue((__QUEUE), (__ADDR), (__ITEM_COUNT * sizeof(__TYPE)))


#define __QUEUE_INIT_0(__QUEUE, __BUFFER, __SIZE )            \
    queue_init_byte(__QUEUE, __BUFFER, __SIZE, false )
    
#define __QUEUE_INIT_1(__QUEUE, __BUFFER, __SIZE, __COVER )          \
    queue_init_byte(__QUEUE, __BUFFER, __SIZE, __COVER )
    
/*!
 * \brief Initialize the queue object.
 *
 * \param[in] __queue pointer to the queue object.
 * \param[in] __buffer address of ring buffer var
 * \param[in] __size size of the ring buffer in bytes.
 *
 * \return the address of queue item
 *
 * \details Here is an example:
    E.g.
    \code
        static uint8_t s_hwQueueBuffer[100];
        static byte_queue_t my_queue;
        queue_init(&my_queue,s_hwQueueBuffer,sizeof(s_hwQueueBuffer));
    \endcode
 */

#define queue_init(__queue,__buffer,__size ,... )                 \
    __PLOOC_EVAL(__QUEUE_INIT_,##__VA_ARGS__)        \
    (__queue,(__buffer),(__size),##__VA_ARGS__)

#define QUEUE_INIT(__QUEUE, __BUFFER, __SIZE ,... )               \
    __PLOOC_EVAL(__QUEUE_INIT_,##__VA_ARGS__)        \
    (__QUEUE,(__BUFFER),(__SIZE),##__VA_ARGS__)


/*!
 * \brief Get data from the ring buffer.
 *
 * \param[in] __queue pointer to the queue object.
 * \param[in] __addr address to the data buffer
 * \param[in] ... Optional parameters,You can add data types and data quantities
 *
 * \return Return the data size we read from the ring buffer.
 *
 * \details Here is an example:
    E.g. The size is automatically calculated based on the variable type
    \code
        uint8_t  data1;
        uint16_t data2;
        uint32_t data3;
        dequeue(&my_queue,&data1);
        dequeue(&my_queue,&data2);
        dequeue(&my_queue,&data3);
    \endcode
    E.g. Read out all data of ring buffer
    \code
         uint8_t  data[100];
         dequeue(&my_queue,data,get_queue_count(&my_queue));
         dequeue(&my_queue,data,uint8_t,get_queue_count(&my_queue));
    \endcode
 */
#define dequeue(__queue,__addr,...)                       \
    __PLOOC_EVAL(__DEQUEUE_,##__VA_ARGS__)        \
    (__queue,(__addr),##__VA_ARGS__)

#define DEQUEUE(__QUEUE, __ADDR,...)                                            \
    __PLOOC_EVAL(__DEQUEUE_,##__VA_ARGS__)                              \
    (__QUEUE,(__ADDR),##__VA_ARGS__)


/*!
 * \brief Put a block of data into the ring buffer. If the capacity of ring buffer is insufficient, it will discard out-of-range data.
 *
 * \param[in] __queue pointer to the queue object.
 * \param[in] __addr address to the data buffer
 * \param[in] ... Optional parameters,You can add data types and data quantities
 *
 * \return Return the data size we put into the ring buffer.
 *
 * \details Here is an example:
    E.g. The size is automatically calculated based on the variable type
    \code
        uint8_t  data1 = 0XAA;
        uint16_t data2 = 0X55AA;
        uint32_t data3 = 0X55AAAA55;
        enqueue(&my_queue,data1);
        enqueue(&my_queue,data2);
        enqueue(&my_queue,data3);
    \endcode
    E.g. Put a block of data into the ring buffer
    \code
         uint32_t  data[100];
         enqueue(&my_queue,data,100);
         enqueue(&my_queue,data,uint32_t,100);
    \endcode
 */

#define enqueue(__queue, __addr,...)                    \
    __PLOOC_EVAL(__ENQUEUE_,##__VA_ARGS__)      \
    (__queue,(__addr),##__VA_ARGS__)

#define ENQUEUE(__QUEUE, __ADDR,...)                    \
    __PLOOC_EVAL(__ENQUEUE_,##__VA_ARGS__)      \
    (__QUEUE,(__ADDR),##__VA_ARGS__)


/*!
 * \brief Peek data from the ring buffer.
 *
 * \param[in] __queue pointer to the queue object.
 * \param[in] __addr address to the data buffer
 * \param[in] ... Optional parameters,You can add data types and data quantities
 *
 * \return Return the data size we peek from the ring buffer.
 *
 * \details Here is an example:
    E.g. The size is automatically calculated based on the variable type
    \code
        uint8_t  data1;
        uint16_t data2;
        uint32_t data3;
        peek_queue(&my_queue,&data1);
        peek_queue(&my_queue,&data2);
        peek_queue(&my_queue,&data3);
    \endcode
 */

#define peek_queue(__queue, __addr,...)                       \
    __PLOOC_EVAL(__PEEK_QUEUE_,##__VA_ARGS__)          \
    (__queue,(__addr),##__VA_ARGS__)

#define PEEK_QUEUE(__QUEUE, __ADDR,...)                         \
    __PLOOC_EVAL(__PEEK_QUEUE_,##__VA_ARGS__)            \
    (__QUEUE,(__ADDR),##__VA_ARGS__)



declare_class(byte_queue_t)
def_class(byte_queue_t,
          private_member(
              uint8_t *pchBuffer;
              uint16_t hwSize;
              uint16_t hwHead;
              uint16_t hwTail;
              uint16_t hwLength;
              uint16_t hwPeek;
              uint16_t hwPeekLength;
              bool bMutex;
              bool bIsCover;
          )
         )
end_def_class(byte_queue_t)


extern
byte_queue_t * queue_init_byte(byte_queue_t *ptObj, void *pBuffer, uint16_t hwItemSize,bool bIsCover);

extern
bool reset_queue(byte_queue_t *ptObj);

extern
uint16_t enqueue_bytes(byte_queue_t *ptObj, void *pDate, uint16_t hwDataLength);

extern
uint16_t dequeue_bytes(byte_queue_t *ptObj, void *pDate, uint16_t hwDataLength);

extern
bool is_queue_empty(byte_queue_t *ptQueue);

extern
bool is_peek_empty(byte_queue_t *ptObj);
	
extern
uint16_t peek_bytes_queue(byte_queue_t *ptObj, void *pDate, uint16_t hwDataLength);

extern
void reset_peek(byte_queue_t *ptQueue);

extern
void get_all_peeked(byte_queue_t *ptQueue);

extern
uint16_t get_peek_status(byte_queue_t *ptQueue);

extern
void restore_peek_status(byte_queue_t *ptQueue, uint16_t hwCount);

extern
uint16_t get_queue_count(byte_queue_t *ptObj);

extern
uint16_t get_queue_available_count(byte_queue_t *ptObj);


#undef __BYTE_QUEUE_CLASS_INHERIT__
#undef __BYTE_QUEUE_CLASS_IMPLEMENT__

#endif
#endif /* QUEUE_QUEUE_H_ */
