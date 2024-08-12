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

#include "msg_check_str.h"
#if defined(USING_MSG_MAP)
def_simple_fsm( check_string,
    def_params(
            const char *pchStr;
            uint16_t hwIndex;
            uint8_t  chByte;
            get_byte_t *ptGetByte;
    )
)

fsm_initialiser( check_string,
    args(
        const char *pchString,
        get_byte_t *ptGetByte
    ))

    init_body (
        if (NULL == pchString || NULL == ptGetByte ) {
            abort_init();
        }
        this.pchStr = pchString;
        this.ptGetByte = ptGetByte;       
    )

fsm_implementation(  check_string )
{
    def_states( IS_END_OF_STR, INPUT_CHAR,IS_TIMEOUT)

    body_begin();

    on_start(
        this.hwIndex = 0; 
        update_state_to(IS_END_OF_STR);
    )

    state(IS_END_OF_STR) {
        if(this.pchStr[this.hwIndex] == '\0') {
            fsm_cpl();
        }
        update_state_to(INPUT_CHAR);
    }

    state(INPUT_CHAR) {
        if(this.ptGetByte->fnGetByte(this.ptGetByte,&(this.chByte),1)) {       
            if(this.chByte != this.pchStr[this.hwIndex++]){         
                fsm_user_req_drop();
            }
            update_state_to(IS_END_OF_STR);
        }
        reset_fsm();
    }

    body_end();
}

#endif


