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

#include "msg_map.h"
#if defined(USING_MSG_MAP)
#include <string.h>

def_simple_fsm( search_msg_map,
    def_params(
        fsm(check_string)  fsmCheckStr;
        fsm(check_arg)     fsmCheckArg;
        msg_t              *ptMsgTableBase;
        msg_t              *ptMsgTableLimit;
        get_byte_t         *ptGetByte;
        uint8_t            chByte;
        uint16_t           hwIndex;
        bool               bArgIsString;
        bool               bIsRequestDrop;
        char               *argv[MSG_ARG_MAX];
        int                argc;
     )
)

fsm_initialiser(search_msg_map,
    args(
        msg_t *ptMsgTableBase,
        msg_t *ptMsgTableLimit,
        get_byte_t *ptGetByte,
        bool bArgIsString
    ))

    init_body (

    if (NULL == ptGetByte || NULL == ptMsgTableBase || NULL == ptMsgTableLimit)
    {
        abort_init();
    }
    this.ptMsgTableBase = ptMsgTableBase;
    this.ptMsgTableLimit = ptMsgTableLimit;
    this.ptGetByte = ptGetByte;
    this.bArgIsString = bArgIsString;
)

fsm_implementation(search_msg_map)
{
    def_states( IS_END_OF_MSG, MSG, ARG)

    body_begin();

    on_start(
        this.bIsRequestDrop = true;
        this.hwIndex = 0;
        update_state_to(IS_END_OF_MSG);
    )

    state(IS_END_OF_MSG) {
        if(&this.ptMsgTableBase[this.hwIndex] == this.ptMsgTableLimit) {
            if(this.bIsRequestDrop != false) {
                fsm_user_req_drop();
            }
            reset_fsm();
        } else {
            init_fsm(check_string, &(this.fsmCheckStr), args((const char *)(this.ptMsgTableBase[this.hwIndex].pchMessage), this.ptGetByte));
            transfer_to(MSG);
        }
    }

    state(MSG) {
        fsm_rt_t tFsm = call_fsm( check_string, &(this.fsmCheckStr));

        if(fsm_rt_cpl == tFsm) {
            memset(this.argv, 0, sizeof(this.argv));
            init_fsm(check_arg, &(this.fsmCheckArg), args((const char *)(this.ptMsgTableBase[this.hwIndex].pchMessage),
                     this.ptGetByte, &this.argc, this.argv, this.bArgIsString));
            update_state_to(ARG);
        }

        if(fsm_rt_user_req_drop == tFsm) {
            this.hwIndex++;
            update_state_to(IS_END_OF_MSG);
        }

        if(fsm_rt_on_going == tFsm) {
            this.bIsRequestDrop = false;
            this.hwIndex++;
            update_state_to(IS_END_OF_MSG);
        }
    }

    state(ARG) {
        fsm_rt_t tFsm = call_fsm( check_arg, &(this.fsmCheckArg));

        if(fsm_rt_cpl == tFsm) {
            this.ptMsgTableBase[this.hwIndex].fnHandler(this.argc, (void *)this.argv);
            fsm_cpl();
        }

        if(fsm_rt_user_req_drop == tFsm) {
            this.hwIndex++;
            update_state_to(IS_END_OF_MSG);
        }

        if(fsm_rt_on_going == tFsm) {
            this.hwIndex++;
            this.bIsRequestDrop = false;
            update_state_to(IS_END_OF_MSG);
        }
    }
    body_end();
}

#endif