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

#ifndef __SERVE_MSG_CHECK_ARG_H_
#define __SERVE_MSG_CHECK_ARG_H_
#include "./app_cfg.h"
#if defined(USING_MSG_MAP)
#include "../.././fsm/simple_fsm.h"
#include "msg_get_byte.h"
#ifdef __cplusplus
extern "C" {
#endif
declare_simple_fsm(check_arg);
extern_fsm_implementation(check_arg);
extern_fsm_initialiser( check_arg,
        args(
            const char *pchString,
            get_byte_t *ptGetByte,
            int32_t *argc,
            char **argv,
            bool bArgIsString
        ))
/*! fsm used to output specified string */
extern_simple_fsm(check_arg,
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

#ifdef __cplusplus
}
#endif
#endif
#endif 
