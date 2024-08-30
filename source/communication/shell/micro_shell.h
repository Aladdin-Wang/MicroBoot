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
#include "../signals_slots/signals_slots.h"
#include "../../generic/queue/ring_queue.h"
#include "check_agent_engine.h"
#include "finsh.h"

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
    byte_queue_t              tByteOutQueue;		
    get_byte_t                tGetByte;
	  shell_read_timeout_t      tReadDataTimeout;
    shell_ops_t               tOps;
    uint16_t                  hwLineLen;
    uint16_t                  hwLinePosition;
    uint16_t                  hwCurposPosition;
    uint16_t                  hwCurrenthistory;
    uint16_t                  hwHistoryCount;
    uint8_t                   chDate;
		char                      chQueueOutBuf[MSG_ARG_LEN];
	  char                      chQueueInBuf[MSG_ARG_LEN];
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

extern check_shell_t *shell_init(check_shell_t *ptObj, shell_ops_t *ptOps);
#endif
#endif /* APPLICATIONS_CHECK_AGENT_XMODEM_H_ */