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

#ifndef __SERVE_SHELL_H_
#define __SERVE_SHELL_H_
#include "./app_cfg.h"
#if defined(WL_USING_SHELL)
#include "../msg_map/msg_map.h"
#include "../signals_slots/signals_slots.h"
#include "../../generic/queue/ring_queue.h"
#include "check_agent_engine.h"
/**
 * @ingroup msh
 *
 * This macro exports a command to module shell.
 *
 * @param command is the name of the command.
 * @param desc is the description of the command, which will show in help list.
 */
#define MSH_FUNCTION_EXPORT_CMD(name, cmd, desc)                      \
    const char __fsym_##cmd##_name[] __section(".rodata.name") = #cmd;    \
    const char __fsym_##cmd##_desc[] __section(".rodata.name") = #desc;   \
    const __used struct _msg_t __fsym_##cmd __section("FSymTab")={  \
          __fsym_##cmd##_name,    \
          (msg_hanlder_t *)&name, \
          __fsym_##cmd##_desc,    \
    };                            \
		COMPILER_ASSERT((IS_FUNCTION_POINTER(name)));

#define MSH_CMD_EXPORT(command, desc)   \
    MSH_FUNCTION_EXPORT_CMD(command, command, desc)

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
#endif
		

typedef struct wl_shell_t wl_shell_t;		
/* Callback type definitions for various shell operations */
typedef uint16_t (shell_call_back)(wl_shell_t *ptObj, uint8_t *pchBuffer, uint16_t hwSize);

/* virtual function table for encapsulating shell operation functions */
typedef struct shell_ops_t {
    shell_call_back           *fnReadData; /* Callback for reading data */
    shell_call_back           *fnWriteData; /* Callback for writing data */
} shell_ops_t;

typedef struct shell_read_timeout_t {
    uint8_t  chState;
    uint16_t hwRemainSize;
    uint16_t hwIndex;
    int64_t lTimeCountms;
} shell_read_timeout_t;

typedef struct wl_shell_t {
    uint8_t                   chState;
    fsm(search_msg_map)       fsmSearchMsgMap;
    byte_queue_t              tByteInQueue;
    get_byte_t                tGetByte;
	  shell_read_timeout_t      tReadDataTimeout;
    shell_ops_t               tOps;
    uint16_t                  hwLineLen;
    uint16_t                  hwLinePosition;
    uint16_t                  hwCurposPosition;
    uint16_t                  hwCurrenthistory;
    uint16_t                  hwHistoryCount;
    uint8_t                   chDate;
	  char                      chQueueBuf[MSG_ARG_LEN];
    char                      chLineBuf[MSG_ARG_LEN];
    char                      cHistoryCmdBuf[SHELL_HISTORY_LINES][MSG_ARG_LEN];
} wl_shell_t;

typedef struct check_shell_t {
    check_agent_t tCheckAgent;
    wl_shell_t    tshell;
} check_shell_t;

static inline uint16_t __shell_read_data_timeout(wl_shell_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    return (*ptObj->tOps.fnReadData)(ptObj, pchByte, hwSize);
}

static inline uint16_t __shell_write_data(wl_shell_t *ptObj, uint8_t* pchByte, uint16_t hwSize)
{
    return (*ptObj->tOps.fnWriteData)(ptObj, pchByte, hwSize);
}

extern fsm_rt_t wl_shell_exec(wl_shell_t *ptObj);
extern check_shell_t *wl_shell_init(check_shell_t *ptObj, shell_ops_t *ptOps);
#endif
#endif /* APPLICATIONS_CHECK_AGENT_XMODEM_H_ */