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

#include "signals_slots.h"
#if defined(USING_SIGNALS_SLOTS)
/*
 * Function: bool direct_connect(sig_slot_t *ptSender, const char *pchSignal, void *pReceiver, void *pMethod)
 * Description: Connects a signal to a slot function in a signal-slot system.
 * Parameters:
 *   - ptSender: Pointer to the signal object from which the signal is emitted.
 *   - pchSignal: The name of the signal to be connected.
 *   - pReceiver: Pointer to the object that will receive the signal.
 *   - pMethod: Pointer to the method or function to be called when the signal is emitted.
 * Returns:
 *   - true: Connection successful.
 *   - false: Connection failed (if any of the input parameters is NULL or memory allocation fails).
 */
bool direct_connect(sig_slot_t *ptSender, const char *pchSignal, void *pReceiver, void *pMethod)
{
    /* Check if any of the input parameters is NULL */
    if (ptSender == NULL || pchSignal == NULL || pReceiver == NULL || pMethod == NULL) {
        return false;
    }

    /* Create a pointer to the metadata of the signal-slot object */
    sig_slot_t *ptMetaObj = ptSender;

    do {
        if(ptMetaObj->ptNext == NULL) {
            /* Allocate memory for a new signal object */
            sig_slot_t *ptNewSender = (sig_slot_t *)malloc(sizeof(struct sig_slot_t));

            /* Check if memory allocation is successful */
            if (ptNewSender == NULL) {
                return false;
            }

            memset(ptNewSender, 0, sizeof(struct sig_slot_t));
            ptMetaObj->ptNext = ptNewSender;
            ptMetaObj->ptNext->ptPrev = ptMetaObj;
        }

        ptMetaObj = ptMetaObj->ptNext;

        /* Support connecting one signal to multiple slot functions or multiple signals to one slot function */
        if (strcmp(ptMetaObj->pchSignal, pchSignal) == 0 || strlen(ptMetaObj->pchSignal) > 0) {
            /* Allocate memory for a new signal object */
            sig_slot_t *ptNewSender = (sig_slot_t *)malloc(sizeof(struct sig_slot_t));

            /* Check if memory allocation is successful */
            if (ptNewSender == NULL) {
                return false;
            }

            memset(ptNewSender, 0, sizeof(struct sig_slot_t));

            /* Check for duplicate connections */
            if(strcmp(ptMetaObj->pchSignal, pchSignal) == 0 && ptMetaObj->pReceiver == pReceiver &&
               ptMetaObj->pMethod == pMethod) {
                free(ptNewSender);
                return false;
            }

            /* Traverse to the end of the signal-slot list */
            while (ptMetaObj != NULL && ptMetaObj->ptNext != NULL) {
                ptMetaObj = ptMetaObj->ptNext;

                /* Check for duplicate connections */
                if(strcmp(ptMetaObj->pchSignal, pchSignal) == 0 && ptMetaObj->pReceiver == pReceiver &&
                   ptMetaObj->pMethod == pMethod) {
                    free(ptNewSender);
                    return false;
                }
            }

            /* Connect the new signal object to the list */
            ptMetaObj->ptNext = ptNewSender;
            ptMetaObj->ptNext->ptPrev = ptMetaObj;
            ptMetaObj = ptMetaObj->ptNext;
        }

        /* Set the method, receiver, and signal for the current metadata object */
        ptMetaObj->pMethod = pMethod;
        ptMetaObj->pReceiver = pReceiver;
        strncpy(ptMetaObj->pchSignal, pchSignal, sizeof(ptMetaObj->pchSignal) - 1);
        ptMetaObj->pchSignal[sizeof(ptMetaObj->pchSignal) - 1] = '\0'; // 确保字符串终止

    } while (0);

    /* Connection successful */
    return true;
}

/**
 * Function: void auto_disconnect(sig_slot_t *ptSender, const char *pchSignal, void *pReceiver, void *pMethod)
 * Description: Removes a connection between a signal and a slot function in a signal-slot system.
 * Parameters:
 *   - ptSender: Pointer to the signal object from which the signal is emitted.
 *   - pchSignal: The name of the signal to be disconnected.
 *   - pReceiver: Pointer to the object or function that is currently connected to the signal.
 *   - pMethod: Pointer to the method or function that is currently connected to the signal.
 * Returns: void
 */
void auto_disconnect(sig_slot_t *ptSender, const char *pchSignal, void *pReceiver, void *pMethod)
{
    /* Check if any of the input parameters is NULL */
    if (ptSender == NULL || pchSignal == NULL || pReceiver == NULL || pMethod == NULL) {
        return;
    }

    /* Create a pointer to the metadata of the signal-slot object */
    sig_slot_t *ptMetaObj = ptSender;

    do {
        /* Traverse the signal-slot list */
        while (ptMetaObj != NULL) {
            /* Check if the current metadata object matches the specified signal, receiver, and method */
            if (strcmp(ptMetaObj->pchSignal, pchSignal) == 0 &&
                ptMetaObj->pReceiver == pReceiver &&
                ptMetaObj->pMethod == pMethod) {
                /* Disconnect the signal from the slot function */
                if (ptMetaObj->ptPrev != NULL) {
                    ptMetaObj->ptPrev->ptNext = ptMetaObj->ptNext;
                }

                if (ptMetaObj->ptNext != NULL) {
                    ptMetaObj->ptNext->ptPrev = ptMetaObj->ptPrev;
                }
                free(ptMetaObj);
                return;
            }
            /* Move to the next metadata object in the list */
            ptMetaObj = ptMetaObj->ptNext;
        }
    } while (0);
}


#endif
