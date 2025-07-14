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

#include "subscribe_publish.h"
#if defined(USING_SUBSCRIBE_PUBLISH)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef this
#define this        (*ptThis)

static uint16_t get_byte (get_byte_t *ptThis, uint8_t *pchByte, uint16_t hwLength);

wl_subscribe_publish_t *wl_subscribe_publish_init(wl_subscribe_publish_t *ptObj)
{
    assert(NULL != ptObj);

    wl_subscribe_publish_t *(ptThis) = ptObj;

    queue_init(&this.tByteInQueue, this.chQueueBuf, sizeof(this.chQueueBuf), true);
    this.tGetByte.pTarget = (void *)(&this.tByteInQueue);
    this.tGetByte.fnGetByte = get_byte;
    #ifdef __ARMCC_VERSION
    extern const int VSymTab$$Base;
    extern const int VSymTab$$Limit;
    init_fsm(search_msg_map, &this.fsmSearchTopicMap, args((msg_t *)&VSymTab$$Base, (msg_t *)&VSymTab$$Limit, &this.tGetByte, false));
    #elif defined (__GNUC__) || defined(__TI_COMPILER_VERSION__) || defined(__TASKING__)
    /* GNU GCC Compiler and TI CCS */
    extern const int __vsymtab_start;
    extern const int __vsymtab_end;
    init_fsm(search_msg_map, &this.fsmSearchTopicMap, args((msg_t *)&__vsymtab_start, (msg_t *)&__vsymtab_end, &this.tGetByte, false));
    #elif defined(__ADSPBLACKFIN__) /* for VisualDSP++ Compiler */
    init_fsm(search_msg_map, &this.fsmSearchTopicMap, args((msg_t *)&__vsymtab_start, (msg_t *)&__vsymtab_end, &this.tGetByte, false));
    #elif defined(_MSC_VER)
    unsigned int *ptr_begin, *ptr_end;
    ptr_begin = (unsigned int *)&__vsym_begin;
    ptr_begin += (sizeof(struct finsh_syscall) / sizeof(unsigned int));

    while (*ptr_begin == 0) ptr_begin ++;

    ptr_end = (unsigned int *) &__vsym_end;
    ptr_end --;

    while (*ptr_end == 0) ptr_end --;

    init_fsm(search_msg_map, &this.fsmSearchTopicMap, args((msg_t *)ptr_begin, (msg_t *)ptr_end, &this.tGetByte, false));
    #endif
    return ptObj;
}

/**
 * @brief Execute shell commands
 *
 * @param ptObj Pointer to the wl_shell_t object
 */
void wl_subscribe_publish_exec(wl_subscribe_publish_t *ptObj)
{
    uint8_t chByte;
    wl_subscribe_publish_t *(ptThis) = ptObj;
    assert(NULL != ptObj);

    fsm_rt_t tFsm = call_fsm( search_msg_map,  &this.fsmSearchTopicMap );

    if(fsm_rt_cpl == tFsm) {
        get_all_peeked(&this.tByteInQueue);
    }

    if(fsm_rt_user_req_drop == tFsm) {
        dequeue(&this.tByteInQueue, &chByte);
    }

    if(fsm_rt_on_going == tFsm) {
        reset_peek(&this.tByteInQueue);
    }
}

static uint16_t get_byte (get_byte_t *ptThis, uint8_t *pchByte, uint16_t hwLength)
{
    return peek_queue(ptThis->pTarget, pchByte, hwLength);
}

#endif

