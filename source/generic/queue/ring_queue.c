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

#define __BYTE_QUEUE_CLASS_IMPLEMENT__
#include "ring_queue.h"

#if defined(USING_RINGEQUEUE)
#undef this
#define this        (*ptThis)

/****************************************************************************
* Function: queue_init_byte                                               *
* Description: Initializes a byte queue object.                           *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object to be initialized.       *
*   - pBuffer: Pointer to the buffer for storing data.                    *
*   - hwItemSize: Size of each item in the buffer.                        *
*   - bIsCover: Indicates whether the queue should overwrite when full.  *
* Returns: Pointer to the initialized byte_queue_t object or NULL.       *
****************************************************************************/
byte_queue_t * queue_init_byte(byte_queue_t *ptObj, void *pBuffer, uint16_t hwItemSize,bool bIsCover)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    if(pBuffer == NULL || hwItemSize == 0) {
        return NULL;
    }

    safe_atom_code() {
        this.pchBuffer = pBuffer;
        this.hwSize = hwItemSize;
        this.hwHead = 0;
        this.hwTail = 0;
        this.hwLength = 0;
        this.hwPeek = this.hwHead;
        this.hwPeekLength = 0;
        this.bIsCover = bIsCover;
        this.bMutex = 0;
    }
    return ptObj;
}
/****************************************************************************
* Function: reset_queue                                                   *
* Description: Resets the byte queue to its initial state.                *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object to be reset.             *
* Returns: True if the reset is successful, false otherwise.             *
****************************************************************************/
bool reset_queue(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    safe_atom_code() {
        this.hwHead = 0;
        this.bMutex = 0;
        this.hwTail = 0;
        this.hwLength = 0;
        this.hwPeek = this.hwHead;
        this.hwPeekLength = 0;
    }
    return true;
}

/****************************************************************************
* Function: enqueue_byte                                                  *
* Description: Enqueues a single byte into the byte queue.               *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
*   - chByte: Byte to be enqueued.                                       *
* Returns: True if the enqueue is successful, false otherwise.           *
****************************************************************************/

bool enqueue_byte(byte_queue_t *ptObj, uint8_t chByte)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);
    bool  bResult = false;
    safe_atom_code() {
        if(this.hwHead == this.hwTail &&
           0 != this.hwLength ){
           if(this.bIsCover == false){
                /* queue is full */
                continue;
            }else{
               /*  overwrite */
                this.hwHead++;
                if(this.hwHead >= this.hwSize){
                    this.hwHead = 0;
                }
                this.hwLength--;
                this.hwPeek = this.hwHead;
                this.hwPeekLength = this.hwLength;
            }
        }
        this.pchBuffer[this.hwTail++] = chByte;
        if(this.hwTail >= this.hwSize){
            this.hwTail = 0;
        }
        this.hwLength++;
        this.hwPeekLength++;
        bResult = true;
    }
    return bResult;
}

/****************************************************************************
* Function: enqueue_bytes                                                 *
* Description: Enqueues multiple bytes into the byte queue.              *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
*   - pDate: Pointer to the data to be enqueued.                         *
*   - hwLength: Number of bytes to enqueue.                               *
* Returns: Number of bytes actually enqueued.                             *
****************************************************************************/

uint16_t enqueue_bytes(byte_queue_t *ptObj, void *pDate, uint16_t hwDataLength)
{
    assert(NULL != ptObj);
    assert(NULL != pDate);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);		
    bool bEarlyReturn = false;      
    safe_atom_code() {
		    if(!this.bMutex){
				    this.bMutex  = true;
		    }else{
            bEarlyReturn = true;
				}					
		}
    if(bEarlyReturn){
        return 0;
    }	
    uint16_t	hwLength = this.hwLength;
    uint16_t	hwPeek = this.hwPeek;
    uint16_t	hwPeekLength = this.hwPeekLength;
    uint16_t	hwHead = this.hwHead;	
    uint16_t	hwTail = this.hwTail;		
    uint8_t  *pchByte = pDate;
    do{
        if(hwDataLength > this.hwSize){
            hwDataLength = this.hwSize;
        }
        if(this.hwHead == this.hwTail &&
           0 != this.hwLength ){
            if(this.bIsCover == false){
                /* queue is full */
                hwDataLength = 0;
                break;
            }else{
                 /* overwrite */
                if(hwDataLength < (this.hwSize - this.hwHead)) {
                    hwHead += hwDataLength;
                }else{
                    hwHead = hwDataLength - (this.hwSize - this.hwHead);
                } 
                hwLength -= hwDataLength;
                hwPeek = this.hwHead;
                hwPeekLength = this.hwLength;                                  
            }
        }
        if(hwDataLength > (this.hwSize - this.hwLength)){
            if(this.bIsCover == false){
                /* drop some data */
                hwDataLength = this.hwSize - this.hwLength;
            }else{
                /* overwrite some data */ 
                uint16_t hwOverLength = hwDataLength - ((this.hwSize - this.hwLength));
                if(hwOverLength < (this.hwSize - this.hwHead)) {
                    hwHead += hwOverLength;
                }else{
                    hwHead = hwDataLength - (this.hwSize - this.hwHead);
                } 
                hwLength -= hwOverLength;
                hwPeek = this.hwHead;
                hwPeekLength = this.hwLength;                                         
            }
        }


				if(hwDataLength < (this.hwSize - this.hwTail)) {
						memcpy(&this.pchBuffer[this.hwTail], pchByte, hwDataLength);
						hwTail += hwDataLength;
						break;
				}

				memcpy(&this.pchBuffer[this.hwTail], &pchByte[0], this.hwSize - this.hwTail);
				memcpy(&this.pchBuffer[0], &pchByte[this.hwSize - this.hwTail], hwDataLength - (this.hwSize - this.hwTail));
				hwTail = hwDataLength - (this.hwSize - this.hwTail);
    } while(0);
		
    safe_atom_code() {				
        this.hwLength = hwLength;
        this.hwPeek = hwPeek;
        this.hwPeekLength = hwPeekLength;
        this.hwHead = hwHead;	
        this.hwTail = hwTail;
        this.hwLength += hwDataLength;
        this.hwPeekLength += hwDataLength;
    }
    return hwDataLength;
}

/****************************************************************************
* Function: dequeue_byte                                                  *
* Description: Dequeues a single byte from the byte queue.               *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
*   - pchByte: Pointer to store the dequeued byte.                       *
* Returns: True if the dequeue is successful, false otherwise.           *
****************************************************************************/

bool dequeue_byte(byte_queue_t *ptObj, uint8_t *pchByte)
{
    assert(NULL != ptObj);
    assert(NULL != pchByte);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);
    bool  bResult = false;
    safe_atom_code() {

        if(this.hwHead == this.hwTail &&
           0 == this.hwLength ){
            /* queue is empty */
            continue;
        }

        *pchByte = this.pchBuffer[this.hwHead++];

        if(this.hwHead >= this.hwSize){
            this.hwHead = 0;
        }

        this.hwLength--;
        this.hwPeek = this.hwHead;
        this.hwPeekLength = this.hwLength;
        bResult = true;
    }
    return bResult;
}

/****************************************************************************
* Function: dequeue_bytes                                                 *
* Description: Dequeues multiple bytes from the byte queue.              *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
*   - pDate: Pointer to store the dequeued data.                         *
*   - hwLength: Number of bytes to dequeue.                               *
* Returns: Number of bytes actually dequeued.                             *
****************************************************************************/

uint16_t dequeue_bytes(byte_queue_t *ptObj, void *pDate, uint16_t hwDataLength)
{
    assert(NULL != ptObj);
    assert(NULL != pDate);

    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);
    bool bEarlyReturn = false;
    safe_atom_code() {
		    if(!this.bMutex){
				    this.bMutex  = true;
		    }else{
            bEarlyReturn = true;
				}					
		}
    if(bEarlyReturn){
        return 0;
    }	
    uint16_t hwHead = this.hwHead;	
    uint8_t *pchByte = pDate;		
    do{			
        if(this.hwHead == this.hwTail &&
           0 == this.hwLength ){
            /* queue is empty */
            hwDataLength = 0;
            break;
        }
        if(hwDataLength > this.hwLength){
            /* less data */
            hwDataLength = this.hwLength;
        }
				if(hwDataLength < (this.hwSize - this.hwHead)) {
						memcpy(pchByte, &this.pchBuffer[this.hwHead], hwDataLength);
						hwHead += hwDataLength;
						break;
				}
				memcpy(&pchByte[0], &this.pchBuffer[this.hwHead], this.hwSize - this.hwHead);
				memcpy(&pchByte[this.hwSize - this.hwHead], &this.pchBuffer[0], hwDataLength - (this.hwSize - this.hwHead));
				hwHead = hwDataLength - (this.hwSize - this.hwHead);
    } while(0);
		
    safe_atom_code() {
        this.bMutex = false;			
        this.hwHead = hwHead;
        this.hwLength -= hwDataLength;
        this.hwPeek = this.hwHead;
        this.hwPeekLength = this.hwLength;
    }
    return hwDataLength;
}

/****************************************************************************
* Function: dequeue_bytes_setup                                                 *
* Description: linear read setup, get read pointer and max linear size..   *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
*   - pchBuffer: Pointer to store the dequeued data.                         *
*   - hwLength: Number of bytes to dequeue.                               *
* Returns: Number of bytes actually dequeued.                             *
****************************************************************************/

uint16_t dequeue_bytes_setup(byte_queue_t *ptObj, uint8_t **pchBuffer, uint16_t hwLength)
{
    assert(NULL != ptObj);
    assert(NULL != pchBuffer);

    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    safe_atom_code() {
        
        if(this.hwHead == this.hwTail &&
           0 == this.hwLength ){
            /* queue is empty */
            hwLength = 0;
            continue;
        }

        if(hwLength > this.hwLength){
            /* less data */
            hwLength = this.hwLength;
        }

        do{
            *pchBuffer = &this.pchBuffer[this.hwHead];
            if(hwLength < (this.hwSize - this.hwHead)) {
                break;
            }
            hwLength = this.hwSize - this.hwHead;
        } while(0);
    }
    return hwLength;
}
/****************************************************************************
* Function: dequeue_bytes_down                                                 *
* Description: linear read done.              *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
*   - hwLength: Number of bytes to dequeue.                               *
* Returns: Number of bytes actually dequeued.                             *
****************************************************************************/

uint16_t dequeue_bytes_down(byte_queue_t *ptObj, uint16_t hwLength)
{
    assert(NULL != ptObj);

    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    safe_atom_code() {

        if(this.hwHead == this.hwTail &&
           0 == this.hwLength ){
            /* queue is empty */
            hwLength = 0;
            continue;
        }

        if(hwLength > this.hwLength){
            /* less data */
            hwLength = this.hwLength;
        }
        this.hwHead += hwLength;
        this.hwLength -= hwLength;
        this.hwPeek = this.hwHead;
        this.hwPeekLength = this.hwLength;
    }
    return hwLength;
}
/****************************************************************************
* Function: is_queue_empty                                                *
* Description: Checks if the byte queue is empty.                         *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
* Returns: True if the queue is empty, false otherwise.                  *
****************************************************************************/

bool is_queue_empty(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    if(this.hwHead == this.hwTail &&
       0 == this.hwLength ) {
        return true;
    }

    return false;
}

/****************************************************************************
* Function: get_queue_count                                               *
* Description: Gets the current number of elements in the byte queue.     *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
* Returns: Number of elements in the queue.                               *
****************************************************************************/

uint16_t get_queue_count(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    return (this.hwLength);
}
/****************************************************************************
* Function: get_queue_available_count                                     *
* Description: Gets the available space in the byte queue.               *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
* Returns: Available space in the queue.                                  *
****************************************************************************/

uint16_t get_queue_available_count(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    return (this.hwSize - this.hwLength);
}

/****************************************************************************
* Function: is_peek_empty                                                 *
* Description: Checks if the peek buffer is empty.                        *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
* Returns: True if the peek buffer is empty, false otherwise.            *
****************************************************************************/

bool is_peek_empty(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    if(this.hwPeek == this.hwTail &&
       0 == this.hwPeekLength ) {
        return true;
    }

    return false;
}

/****************************************************************************
* Function: peek_byte_queue                                               *
* Description: Peeks a single byte from the byte queue without dequeuing. *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
*   - pchByte: Pointer to store the peeked byte.                         *
* Returns: True if peek is successful, false otherwise.                  *
****************************************************************************/

bool peek_byte_queue(byte_queue_t *ptObj, uint8_t *pchByte)
{
    assert(NULL != ptObj);
    assert(NULL != pchByte);

    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);
    bool  bResult = false;
    safe_atom_code() {

        if(this.hwPeek == this.hwTail &&
           0 == this.hwPeekLength ){
            /* empty */
            continue;
        }

        *pchByte = this.pchBuffer[this.hwPeek++];

        if(this.hwPeek >= this.hwSize){
            this.hwPeek = 0;
        }

        this.hwPeekLength--;
        bResult = true;
    }
    return bResult;
}

/****************************************************************************
* Function: peek_bytes_queue                                              *
* Description: Peeks multiple bytes from the byte queue without dequeuing.*
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
*   - pDate: Pointer to store the peeked data.                           *
*   - hwLength: Number of bytes to peek.                                  *
* Returns: Number of bytes actually peeked.                               *
****************************************************************************/

uint16_t peek_bytes_queue(byte_queue_t *ptObj, void *pDate, uint16_t hwDataLength)
{
    assert(NULL != ptObj);
    assert(NULL != pDate);

    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    uint8_t *pchByte = pDate;

    safe_atom_code() {
        if(this.hwPeek == this.hwTail &&
           0 == this.hwPeekLength ){
            /* empty */
            hwDataLength = 0;
            continue;
        }

        if(hwDataLength > this.hwPeekLength){
            /* less data */
            hwDataLength = this.hwPeekLength;
        }

        do{
            if(hwDataLength < (this.hwSize - this.hwPeek)) {
                memcpy(pchByte, &this.pchBuffer[this.hwPeek], hwDataLength);
                this.hwPeek += hwDataLength;
                break;
            }

            memcpy(&pchByte[0], &this.pchBuffer[this.hwPeek], this.hwSize - this.hwPeek);
            memcpy(&pchByte[this.hwSize - this.hwPeek], &this.pchBuffer[0], hwDataLength - (this.hwSize - this.hwPeek));
            this.hwPeek = hwDataLength - (this.hwSize - this.hwPeek);
        } while(0);

        this.hwPeekLength -= hwDataLength;
    }
    return hwDataLength;
}

/****************************************************************************
* Function: reset_peek                                                    *
* Description: Resets the peek buffer to its initial state.               *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
* Returns: True if the reset is successful, false otherwise.             *
****************************************************************************/

bool reset_peek(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    safe_atom_code() {
        this.hwPeek = this.hwHead;
        this.hwPeekLength = this.hwLength;
    }
    return true;
}

/****************************************************************************
* Function: get_all_peeked                                                *
* Description: Moves all peeked elements back to the queue.              *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
* Returns: True if successful, false otherwise.                          *
****************************************************************************/

bool get_all_peeked(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    safe_atom_code() {
        this.hwHead = this.hwPeek;
        this.hwLength = this.hwPeekLength;
    }
    return true;
}

/****************************************************************************
* Function: get_peek_status                                               *
* Description: Gets the current status of the peek buffer.               *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
* Returns: Current number of elements in the peek buffer.                *
****************************************************************************/

uint16_t get_peek_status(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    uint16_t hwCount;

    safe_atom_code() {
        if(this.hwPeek >= this.hwHead){
            hwCount = this.hwPeek - this.hwHead;
        } else{
            hwCount = this.hwSize - this.hwHead + this.hwPeek;
        }
    }
    return hwCount;
}

/****************************************************************************
* Function: restore_peek_status                                           *
* Description: Restores the peek buffer status to a previous count.      *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
*   - hwCount: Number of elements to restore in the peek buffer.         *
* Returns: True if successful, false otherwise.                          *
****************************************************************************/

bool restore_peek_status(byte_queue_t *ptObj, uint16_t hwCount)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    safe_atom_code() {
        if(this.hwHead + hwCount < this.hwSize){
            this.hwPeek = this.hwHead + hwCount;
        } else{
            this.hwPeek =  hwCount - (this.hwSize - this.hwHead);
        }
        if(this.hwPeekLength >= hwCount){
            this.hwPeekLength = this.hwPeekLength - hwCount;
        }else{
            this.hwPeekLength = 0;        
        }
    }
    return true;
}

#endif
