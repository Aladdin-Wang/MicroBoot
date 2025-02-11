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
#include "../../check_agent_engine/check_agent_engine.h"
#include "finsh.h"

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
#endif
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
#ifdef __riscv
#define MSTATUS_MIE (1 << 3U)
#define MSTATUS_MPIE (1 << 7U)
static inline uint32_t read_mstatus(void) {
    uint32_t value;
    __asm volatile("csrr %0, mstatus" : "=r"(value));
    return value;
}
static inline void write_mstatus(uint32_t value) {
    __asm volatile("csrw mstatus, %0" ::"r"(value));
}
static inline uint32_t __get_mstatus_and_disable_irq(void) {
    uint32_t mstatus = 0;
    mstatus = read_mstatus();
    write_mstatus(mstatus & ~MSTATUS_MIE);
    return mstatus;
}
static inline void __set_mstatus(uint32_t mstatus) {
    write_mstatus(mstatus);
}
#define safe_atom_code()\
  for(uint32_t SAFE_NAME(temp) = __get_mstatus_and_disable_irq(),\
     *SAFE_NAME(temp3) = NULL ;\
       SAFE_NAME(temp3)++ == NULL;\
       __set_mstatus(SAFE_NAME(temp)))

#else
#include "cmsis_compiler.h"
#define safe_atom_code()                                            \
        for(  uint32_t SAFE_NAME(temp) =                             \
            ({uint32_t SAFE_NAME(temp2)=__get_PRIMASK();       \
                __disable_irq();                                 \
                  SAFE_NAME(temp2);}),*SAFE_NAME(temp3) = NULL;    \
                    SAFE_NAME(temp3)++ == NULL;                      \
                     __set_PRIMASK(SAFE_NAME(temp)))
#endif
#endif		

typedef struct wl_shell_t wl_shell_t;		
/* Callback type definitions for various shell operations */
typedef uint16_t (shell_write_call_back)(wl_shell_t *ptObj, const char* pchBuffer, uint16_t hwSize);
typedef uint16_t (shell_read_call_back)(wl_shell_t *ptObj,  char* pchBuffer, uint16_t hwSize);
/* virtual function table for encapsulating shell operation functions */
typedef struct shell_ops_t {
    shell_read_call_back           *fnReadData; /* Callback for reading data */
    shell_write_call_back          *fnWriteData; /* Callback for writing data */
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
    bool                      bMutex;
    char                      chDate;
    char                      chQueueInBuf[MSG_ARG_LEN];
    char                      chReadLineBuf[MSG_ARG_LEN];
    char                      chWriteLineBuf[MSG_ARG_LEN];	
    char                      cHistoryCmdBuf[SHELL_HISTORY_LINES][MSG_ARG_LEN];
} wl_shell_t;

typedef struct check_shell_t {
    check_agent_t tCheckAgent;
    wl_shell_t    tshell;
} check_shell_t;

static inline uint16_t __shell_read_data_timeout(wl_shell_t *ptObj, char* pchByte, uint16_t hwSize)
{
    if(*ptObj->tOps.fnReadData == NULL){
        return 0;
    }
    return (*ptObj->tOps.fnReadData)(ptObj, pchByte, hwSize);
}

static inline uint16_t __shell_write_data(wl_shell_t *ptObj, const char* pchByte, uint16_t hwSize)
{
    if(*ptObj->tOps.fnWriteData == NULL){
        return 0;
    }	
    return (*ptObj->tOps.fnWriteData)(ptObj, pchByte, hwSize);
}

extern check_shell_t *shell_init(check_shell_t *ptObj, shell_ops_t *ptOps);
extern wl_shell_t *shell_console_get(void);
extern void shell_console_set(wl_shell_t *ptConsoleShell);
extern void shell_printf(const char *format, ...);
#endif
#endif /* APPLICATIONS_CHECK_AGENT_XMODEM_H_ */