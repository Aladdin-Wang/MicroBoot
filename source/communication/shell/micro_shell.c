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

#include "micro_shell.h"

#if defined(WL_USING_SHELL)
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#undef this
#define this        (*ptThis)

static void shell_push_history(wl_shell_t *ptObj);
static void wl_shell_echo(wl_shell_t *ptObj, uint8_t *pchData, uint16_t hwLength);

__attribute__((weak))
int64_t get_system_time_ms(void)
{
    static int64_t wTimeCount = 0;
    wTimeCount ++;
    return wTimeCount;
}

static fsm_rt_t shell_read_with_timeout(shell_read_timeout_t *ptThis, uint8_t* pchByte, uint16_t hwSize, uint16_t hwTimeout)
{
    /* Macro to reset the finite state machine (FSM) */
#define SHELL_READ_TIMEOUT_RESET_FSM() do { this.chState = 0; } while(0)
    assert(NULL != ptThis);
    wl_shell_t *ptObj = container_of(ptThis, wl_shell_t, tReadDataTimeout);
    /* Enum defining FSM states for receiving a Ymodem packet */
    enum { START, READ_DOING, IS_TIMEOUT, RESET_TIME};

    /* Processing states using a switch-case statement */
    switch(this.chState) {
        case START: {
            this.hwIndex = 0;
            this.hwRemainSize = hwSize;
            this.lTimeCountms = hwTimeout + get_system_time_ms();
            /* Begin the process of reading a new packet by transitioning to the data state. */
            this.chState = READ_DOING;
        }

        case READ_DOING: {
            uint16_t hwReadLen  = __shell_read_data_timeout(ptObj, (pchByte + this.hwIndex), this.hwRemainSize);

            if(hwReadLen == this.hwRemainSize) {
                SHELL_READ_TIMEOUT_RESET_FSM();
                return fsm_rt_cpl;
            } else if(hwReadLen > 0) {
                this.hwIndex += hwReadLen;
                this.hwRemainSize = this.hwRemainSize - hwReadLen;
            }

            this.chState = IS_TIMEOUT;
        }

        case IS_TIMEOUT: {
            if(get_system_time_ms() >= this.lTimeCountms) {
                SHELL_READ_TIMEOUT_RESET_FSM();
                return fsm_rt_user_req_timeout;
            }

            this.chState = READ_DOING;
            break;
        }
    }

    /* Return the ongoing status if the packet isn't yet fully processed or an error hasn't occurred. */
    return fsm_rt_on_going;
}

/**
 * @brief Read input from the shell
 *
 * @param ptObj Pointer to the wl_shell_t object
 * @param pchData Pointer to the input data
 * @param hwLength Length of the input data
 */
static fsm_rt_t wl_shell_readline(wl_shell_t *ptObj)
{
    wl_shell_t *(ptThis) = (wl_shell_t *)ptObj;
    assert(NULL != ptObj);

    fsm_rt_t tFsm = shell_read_with_timeout(&ptObj->tReadDataTimeout, &this.chDate, 1, 10);

    if(fsm_rt_cpl == tFsm) {
        if (this.chDate == '\r' || this.chDate  == '\n') {
            if(this.hwLinePosition  == strlen(this.chLineBuf) && this.hwLinePosition != 0) {
                shell_push_history(ptObj);
                this.chLineBuf[this.hwLinePosition++] = this.chDate;
                enqueue(&this.tByteInQueue, this.chLineBuf, this.hwLinePosition );
            }

            memset(this.chLineBuf, 0, sizeof(this.chLineBuf));
            this.hwLinePosition = 0;
        } else if(this.chDate == 0x7f || this.chDate == 0x08 ) { /* handle backspace key */
            if (this.hwLinePosition != 0) {
                this.chLineBuf[--this.hwLinePosition] = 0;
                this.hwLineLen = this.hwLinePosition;
            }

        } else {
            if (isalpha(this.chDate) || isspace(this.chDate) || this.chDate == '_') {
                if (this.hwLinePosition < (MSG_ARG_LEN - 1)) {
                    this.chLineBuf[this.hwLinePosition++] = this.chDate;
                    this.hwLineLen = this.hwLinePosition;
                } else {
                    this.hwLinePosition = 0;
                    this.hwLineLen = this.hwLinePosition;
                }
            } else {
                return fsm_rt_user_req_drop;
            }
        }
    } else if(fsm_rt_user_req_timeout == tFsm) {
        wl_shell_echo(ptObj, &this.chDate, 1);
        return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}
/**
 * @brief Echo shell input
 *
 * @param ptObj Pointer to the wl_shell_t object
 * @param pchData Pointer to the input data
 * @param hwLength Length of the input data
 */
static void wl_shell_echo(wl_shell_t *ptObj, uint8_t *pchData, uint16_t hwLength)
{
    wl_shell_t *(ptThis) = ptObj;
    assert(NULL != ptObj);

    for(uint16_t i = 0; i < hwLength; i++) {
        if (isalpha(this.chDate) || isspace(this.chDate) || this.chDate == '_' || this.chDate == 0x7f || this.chDate == 0x08 ) {
            if (pchData[i] == '\r' || pchData[i]  == '\n') {
                this.hwCurposPosition = 0;
                printf("\r\nkk@shell >");
            } else if(pchData[i] == 0x7f || pchData[i] == 0x08 ) { /* handle backspace key */
                if(this.hwCurposPosition != 0) {
                    this.hwCurposPosition--;
                    printf("\b \b");
                }
            } else {
                this.hwCurposPosition++;
                printf("%c", pchData[i]);
            }

            this.chDate = 0;
        }
    }
}

/**
 * @brief Execute shell commands
 *
 * @param ptObj Pointer to the wl_shell_t object
 */
fsm_rt_t wl_shell_exec(wl_shell_t *ptObj)
{
    uint8_t chByte;
    wl_shell_t *(ptThis) = ptObj;
    assert(NULL != ptObj);

    fsm_rt_t tFsm = call_fsm( search_msg_map,  &this.fsmSearchMsgMap );

    if(fsm_rt_cpl == tFsm) {
        get_all_peeked(&this.tByteInQueue);
    }

    if(fsm_rt_user_req_drop == tFsm) {
        dequeue(&this.tByteInQueue, &chByte);
    }

    if(fsm_rt_on_going == tFsm) {
        reset_peek(&this.tByteInQueue);
    }

    return wl_shell_readline(ptObj);
}

static uint16_t get_byte (get_byte_t *ptThis, uint8_t *pchByte, uint16_t hwLength)
{
    return peek_queue(ptThis->pTarget, pchByte, hwLength);
}

/**
 * @brief Initialize the shell
 *
 * @param ptObj Pointer to the wl_shell_t object
 * @return wl_shell_t* Pointer to the initialized wl_shell_t object
 */
check_shell_t *wl_shell_init(check_shell_t *ptObj, shell_ops_t *ptOps)
{
    check_shell_t *(ptThis) = ptObj;
    assert(NULL != ptOps);
    assert(NULL != ptObj);
    this.tCheckAgent.pAgent = &this.tshell;
    this.tCheckAgent.fnCheck = (check_hanlder_t *)wl_shell_exec;
    this.tCheckAgent.ptNext = NULL;
    this.tCheckAgent.hwPeekStatus = 0;
    this.tCheckAgent.bIsKeepingContext = true;

    memcpy(&this.tshell.tOps, ptOps, sizeof(this.tshell.tOps));
    queue_init(&this.tshell.tByteInQueue, this.tshell.chQueueBuf, sizeof(this.tshell.chQueueBuf), true);
    this.tshell.tGetByte.pTarget = (void *)(&this.tshell.tByteInQueue);
    this.tshell.tGetByte.fnGetByte = get_byte;
    #ifdef __ARMCC_VERSION
    extern const int FSymTab$$Base;
    extern const int FSymTab$$Limit;
    init_fsm(search_msg_map, &this.tshell.fsmSearchMsgMap, args((msg_t *)&FSymTab$$Base, (msg_t *)&FSymTab$$Limit, &this.tshell.tGetByte, true));
    #elif defined (__GNUC__) || defined(__TI_COMPILER_VERSION__) || defined(__TASKING__)
    /* GNU GCC Compiler and TI CCS */
    extern const int __fsymtab_start;
    extern const int __fsymtab_end;
    init_fsm(search_msg_map, &this.fsmSearchMsgMap, args((msg_t *)&__fsymtab_start, (msg_t *)&__fsymtab_end, &this.tByteInQueue, true));
    #elif defined(__ADSPBLACKFIN__) /* for VisualDSP++ Compiler */
    init_fsm(search_msg_map, &this.fsmSearchMsgMap, args((msg_t *)&__fsymtab_start, (msg_t *)&__fsymtab_end, &this.tByteInQueue, true));
    #elif defined(_MSC_VER)
    unsigned int *ptr_begin, *ptr_end;
    ptr_begin = (unsigned int *)&__fsym_begin;
    ptr_begin += (sizeof(struct finsh_syscall) / sizeof(unsigned int));

    while (*ptr_begin == 0) ptr_begin ++;

    ptr_end = (unsigned int *) &__fsym_end;
    ptr_end --;

    while (*ptr_end == 0) ptr_end --;

    init_fsm(search_msg_map, &this.fsmSearchMsgMap, args((msg_t *)ptr_begin, (msg_t *)ptr_end, &this.tByteInQueue, true));
    #endif


    printf("\r\nkk@shell >");

    return ptObj;
}


static void shell_push_history(wl_shell_t *ptObj)
{
    wl_shell_t *(ptThis) = ptObj;
    assert(NULL != ptObj);

    if (this.hwLinePosition != 0) {
        /* push history */
        if (this.hwHistoryCount >= SHELL_HISTORY_LINES) {
            /* if current cmd is same as last cmd, don't push */
            if (memcmp(&this.cHistoryCmdBuf[SHELL_HISTORY_LINES - 1], this.chLineBuf, MSG_ARG_LEN)) {
                /* move history */
                int index;

                for (index = 0; index < SHELL_HISTORY_LINES - 1; index ++) {
                    memcpy(&this.cHistoryCmdBuf[index][0],
                           &this.cHistoryCmdBuf[index + 1][0], MSG_ARG_LEN);
                }

                memset(&this.cHistoryCmdBuf[index][0], 0, MSG_ARG_LEN);
                memcpy(&this.cHistoryCmdBuf[index][0], this.chLineBuf, this.hwLinePosition);
                /* it's the maximum history */
                this.hwHistoryCount = SHELL_HISTORY_LINES;
            }
        } else {
            /* if current cmd is same as last cmd, don't push */
            if (this.hwHistoryCount == 0 || memcmp(&this.cHistoryCmdBuf[this.hwHistoryCount - 1], this.chLineBuf, MSG_ARG_LEN)) {
                this.hwCurrenthistory = this.hwHistoryCount;
                memset(&this.cHistoryCmdBuf[this.hwHistoryCount][0], 0, MSG_ARG_LEN);
                memcpy(&this.cHistoryCmdBuf[this.hwHistoryCount][0], this.chLineBuf, this.hwLinePosition);
                /* increase count and set current history position */
                this.hwHistoryCount ++;
            }
        }
    }

    this.hwCurrenthistory = this.hwHistoryCount;
}

static int msh_help(int argc, char **argv)
{
    #ifdef __ARMCC_VERSION
    extern const int FSymTab$$Base;
    extern const int FSymTab$$Limit;
    printf("\r\nshell commands:\r\n");
    {
        msg_t *ptMsgTableBase = (msg_t *)&FSymTab$$Base;
        msg_t *ptMsgTableLimit = (msg_t *)&FSymTab$$Limit;

        for (uint32_t i = 0; &ptMsgTableBase[i] != ptMsgTableLimit; i++) {
            printf("%-16s - %s\r\n", ptMsgTableBase[i].pchMessage, ptMsgTableBase[i].pchDesc);
        }
    }
    #elif defined (__GNUC__) || defined(__TI_COMPILER_VERSION__) || defined(__TASKING__)
    /* GNU GCC Compiler and TI CCS */
    extern const int __fsymtab_start;
    extern const int __fsymtab_end;
    printf("\r\nshell commands:\r\n");
    {
        msg_t *ptMsgTableBase = (msg_t *)&__fsymtab_start;
        msg_t *ptMsgTableLimit = (msg_t *)&__fsymtab_end;

        for (uint32_t i = 0; &ptMsgTableBase[i] != ptMsgTableLimit; i++) {
            printf("%-16s - %s\r\n", ptMsgTableBase[i].pchMessage, ptMsgTableBase[i].pchDesc);
        }
    }
    #endif

    return 0;
}
MSH_FUNCTION_EXPORT_CMD(msh_help, help, shell help);


#endif
