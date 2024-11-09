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

#ifndef __SERVE_MSG_MAP_H_
#define __SERVE_MSG_MAP_H_
#include "./app_cfg.h"
#if defined(USING_MSG_MAP)
#include "msg_check_str.h"
#include "msg_check_arg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __section(x)               __attribute__((section(x)))
#define __used                     __attribute__((used))

#define IS_FUNCTION_POINTER(name) \
    __builtin_types_compatible_p(typeof(name), typeof(msg_hanlder_t))
			
#define COMPILER_CONCAT_(a, b) a##b
#define COMPILER_CONCAT(a, b) COMPILER_CONCAT_(a, b)

// Divide by zero if the the expression is false.  This
// causes an error at compile time.
//
// The special value '__COUNTER__' is used to create a unique value to
// append to 'compiler_assert_' to create a unique token.  This prevents
// conflicts resulting from the same enum being declared multiple times.
#define COMPILER_ASSERT(e) enum { COMPILER_CONCAT(compiler_assert_, __COUNTER__) = 1/((e) ? 1 : 0) }

/********************************************************************************************/
typedef struct _msg_t msg_t;
typedef int msg_hanlder_t(int argc, char **argv);
struct _msg_t{
    const char *pchMessage;
    msg_hanlder_t *fnHandler;
    const char *pchDesc;
};

declare_simple_fsm(search_msg_map);
extern_fsm_implementation(search_msg_map);
extern_fsm_initialiser( search_msg_map,
        args(
                msg_t *ptMsgTableBase,
                msg_t *ptMsgTableLimit,
                get_byte_t *ptGetByte,
                bool bArgIsString
        ))
extern_simple_fsm(search_msg_map,
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

#ifdef __cplusplus
}
#endif
#endif
#endif /* MSG_MAP_MSG_MAP_H_ */


