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

#include "check_agent_engine.h"
#include <string.h>
#if USE_SERVICE_CHECK_USE_PEEK == ENABLED
static uint16_t peek_byte (peek_byte_t *ptThis,uint8_t *pchByte, uint16_t hwLength);
def_simple_fsm( check_use_peek,
    def_params(
            check_agent_t *ptFreeList;
            check_agent_t *ptCheckList;
            byte_queue_t *ptByteInQueue;
            peek_byte_t  tPeekByte;						
            bool bIsRequestDrop;
            uint8_t chByte;
    )
)

fsm_initialiser(check_use_peek,
        args(
                byte_queue_t *ptByteInQueue
        ))
    init_body (
        if (NULL == ptByteInQueue) {
            abort_init();
        }
        this.ptByteInQueue = ptByteInQueue;
        this.tPeekByte.pTarget = (void *)(this.ptByteInQueue);
        this.tPeekByte.fnGetByte = peek_byte;
    )


fsm_implementation(check_use_peek)
    {
        def_states( IS_END_OF_AGENT,CHECK_AGENT)

        body_begin();

        on_start(
                this.bIsRequestDrop = true;
                this.ptFreeList = this.ptCheckList;
                update_state_to(IS_END_OF_AGENT);
        )

        state(IS_END_OF_AGENT) {
            if(this.ptFreeList == NULL){
                if(this.bIsRequestDrop != false){
                    dequeue(this.ptByteInQueue,&this.chByte);
                    for(check_agent_t *ptNote = this.ptCheckList;ptNote != NULL; ptNote = ptNote->ptNext){
                        ptNote->hwPeekStatus = 0;
                    }                    
                }
                fsm_cpl();
            }
            reset_peek(this.ptByteInQueue);
            if(this.ptFreeList->bIsKeepingContext != false){
                restore_peek_status(this.ptByteInQueue,this.ptFreeList->hwPeekStatus);
            }
            update_state_to(CHECK_AGENT);
        }
        state(CHECK_AGENT) {
            fsm_rt_t tFsm = this.ptFreeList->fnCheck(this.ptFreeList->pAgent);
            if(fsm_rt_on_going == tFsm){						
                this.bIsRequestDrop = false;               
            }else if(fsm_rt_user_req_drop == tFsm){
                if(this.ptFreeList->bIsKeepingContext != false){
                    this.ptFreeList->hwPeekStatus = get_peek_status(this.ptByteInQueue);
                }                
                this.ptFreeList = this.ptFreeList->ptNext;
                transfer_to(IS_END_OF_AGENT);                
            }else if(fsm_rt_user_req_timeout == tFsm){						
                get_all_peeked(this.ptByteInQueue);
                for(check_agent_t *ptNote = this.ptCheckList;ptNote != NULL; ptNote = ptNote->ptNext){
                    ptNote->hwPeekStatus = 0;
                }
                this.ptFreeList = this.ptFreeList->ptNext;
                transfer_to(IS_END_OF_AGENT);                
            }else{
                get_all_peeked(this.ptByteInQueue);
                for(check_agent_t *ptNote = this.ptCheckList;ptNote != NULL; ptNote = ptNote->ptNext){
                    ptNote->hwPeekStatus = 0;
                }               
                fsm_cpl();
            }
        }
        body_end();
    }

peek_byte_t *get_read_byte_interface(fsm_check_use_peek_t *ptObj)
{
    __fsm_check_use_peek_t *ptThis = (__fsm_check_use_peek_t *)ptObj;
    if (NULL == ptThis) {
        return NULL;
    }
    return &(this.tPeekByte);
}

bool agent_register(fsm_check_use_peek_t *ptObj,check_agent_t *ptNewItem)
{
    check_agent_t *ptNote;
    __fsm_check_use_peek_t *ptThis = (__fsm_check_use_peek_t *)ptObj;
    if (NULL == ptThis || NULL == ptNewItem ) {
        return false;
    }
    for(ptNote = this.ptCheckList;ptNote != NULL; ptNote = ptNote->ptNext){
        if(ptNote == ptNewItem){
            return false;
        }
    }
    ptNewItem->ptNext = this.ptCheckList;
    this.ptCheckList = ptNewItem;
    this.ptFreeList = this.ptCheckList;
    return true;
}

bool agent_unregister(fsm_check_use_peek_t *ptObj,check_agent_t *ptItem)
{
    __fsm_check_use_peek_t *ptThis = (__fsm_check_use_peek_t *)ptObj;
    if (NULL == ptThis || NULL == ptItem ) {
        return false;
    }
    if(this.ptCheckList == ptItem){
        this.ptCheckList = ptItem->ptNext;
        this.ptFreeList = this.ptCheckList;
        return true;
    }
    for(this.ptFreeList = this.ptCheckList;this.ptFreeList != NULL; this.ptFreeList = this.ptFreeList->ptNext){
        if(this.ptFreeList->ptNext == ptItem){
            this.ptFreeList->ptNext = ptItem->ptNext;
            return true;
        }
    }
    return false;
}

static uint16_t peek_byte (peek_byte_t *ptThis,uint8_t *pchByte, uint16_t hwLength)
{
    return peek_queue(ptThis->pTarget,pchByte,hwLength);
}

#endif
