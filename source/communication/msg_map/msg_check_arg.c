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

#include "msg_check_arg.h"
#if defined(USING_MSG_MAP)
#include <string.h>
def_simple_fsm( check_arg,
    def_params(
            const char *pchStr;
            uint16_t hwIndex;
            int16_t hwCount;
            uint8_t chByte;
            bool bArgIsString;
            get_byte_t *ptGetByte;
            uint8_t chArgNums;
            int16_t chArgLen;
            char chArgs[MSG_ARG_LEN];
            int32_t *argc;           
            char **argv;
    )
)

fsm_initialiser( check_arg,
    args(
        const char *pchString,
        get_byte_t *ptGetByte,
        int32_t *argc,
        char **argv,
        bool bArgIsString
    ))

    init_body (
        if (NULL == pchString || NULL == ptGetByte || NULL == argc || NULL == argv  ) {
            abort_init();
        }
        this.pchStr = pchString;
        this.ptGetByte = ptGetByte;  
        this.hwCount = 0;		
        this.hwIndex = 0;
        this.bArgIsString = bArgIsString;
        this.argc = argc;
        this.argv = argv;
    )

fsm_implementation(  check_arg )
{
    def_states(IS_END_OF_ARG,IS_START_OF_ARG,RECEIVE_ARG)

    body_begin();

    on_start(
        this.hwIndex = 0;
        memset(this.chArgs,0,sizeof(this.chArgs));    
        this.argv[0] = (char *)this.pchStr;
        *(this.argc) = 1;
        update_state_to(IS_START_OF_ARG);
    )

    state(IS_START_OF_ARG) {
        if(this.bArgIsString == false){
            if(this.ptGetByte->fnGetByte(this.ptGetByte,&(this.chByte),1)) {  
                if(this.chByte > 0){   
                    this.chArgNums =  this.chByte;               
                    update_state_to(RECEIVE_ARG);
                }else{
                    fsm_cpl();
                }
                fsm_user_req_drop();
            }
            reset_fsm();        
        }else{
            if(this.ptGetByte->fnGetByte(this.ptGetByte,&(this.chByte),1)) {  
                if(this.chByte == ' '){             
                    update_state_to(RECEIVE_ARG);
                }else if(this.chByte == '\r' || this.chByte == '\n') {
                    fsm_cpl();
                }
                fsm_user_req_drop();
            }
            reset_fsm();
        }
    }

    state(RECEIVE_ARG) {
        if(this.bArgIsString == false){
            if(this.ptGetByte->fnGetByte(this.ptGetByte,(uint8_t *)&(this.chArgLen),sizeof(this.chArgLen))) {       
                if(this.ptGetByte->fnGetByte(this.ptGetByte,(uint8_t *)&(this.chArgs[this.hwIndex]),this.chArgLen)) { 
                    this.argv[(*this.argc)++] = &this.chArgs[this.hwIndex]; 
                    this.hwIndex += this.chArgLen;               
                    update_state_to(IS_END_OF_ARG);
                }
            }
            reset_fsm();        
        }else{
            if(this.ptGetByte->fnGetByte(this.ptGetByte,&(this.chByte),1)) {       
                if(this.chByte == ' '){
                    this.chByte = '\0';
                    this.argv[(*this.argc)++] = &this.chArgs[this.hwIndex - this.hwCount];
                    this.hwCount = -1;
                }
                this.chArgs[this.hwIndex++] = this.chByte;
                this.hwCount++;            
                update_state_to(IS_END_OF_ARG);
            }
            reset_fsm();
        }
    }   
	
    state(IS_END_OF_ARG) {
        if(this.bArgIsString == false){
            if((*this.argc) > this.chArgNums || this.hwIndex >= MSG_ARG_LEN - 1){
                fsm_cpl();
            }
            update_state_to(RECEIVE_ARG);
        }else{    
            if(this.chByte == '\r' || this.chByte == '\n' || this.hwIndex >= MSG_ARG_LEN - 1) {
                this.chByte = '\0';
                this.argv[(*this.argc)++] = &this.chArgs[this.hwIndex - this.hwCount];
                this.chArgs[--this.hwIndex] = this.chByte;
                fsm_cpl();
            }
            update_state_to(RECEIVE_ARG);
        }
    }
    body_end();
}
#endif
