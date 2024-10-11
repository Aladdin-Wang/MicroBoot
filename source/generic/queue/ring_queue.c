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

    bool bEarlyReturn = false;      
    safe_atom_code() {
        if(this.hwHead == this.hwTail &&
           0 != this.hwLength ){
            /* queue is full */
            if(this.bIsCover == false){
               bEarlyReturn = true;			 
               continue;
            }
        }			
        if(!this.bMutex){
            this.bMutex  = true;
        }else{
            bEarlyReturn = true;
        }					
    }
    if(bEarlyReturn){
        return false;
    }		
    uint16_t hwTail = this.hwTail;
    safe_atom_code() {
        if(this.hwHead == this.hwTail &&
           0 != this.hwLength ){
            /* queue is full */
            if(this.bIsCover != false){
               /*  overwrite */
                this.hwHead++;
                if(this.hwHead >= this.hwSize){
                    this.hwHead = 0;
                }
                this.hwLength--;
                this.hwPeek = this.hwHead;
            }
        }				
        this.hwTail++;
        if(this.hwTail >= this.hwSize){
            this.hwTail = 0;
        }
        this.hwLength++;
        this.hwPeekLength++;				
    }
    this.pchBuffer[hwTail] = chByte;
    this.bMutex = false;				
    return true;
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
        if(this.hwHead == this.hwTail &&
           0 != this.hwLength ){
           /* queue is full */
            if(this.bIsCover == false){
                bEarlyReturn = true;							
                continue;
            }
        }			
        if(!this.bMutex){
            this.bMutex  = true;
        }else{
            bEarlyReturn = true;
        }					
    }
    if(bEarlyReturn){
        return 0;
    }		
    uint8_t *pchByte = pDate;
    uint16_t hwTail = this.hwTail;		
    safe_atom_code() {	
        if(hwDataLength > this.hwSize){
            hwDataLength = this.hwSize;
        }			
        if(hwDataLength > (this.hwSize - this.hwLength)){
            if(this.bIsCover == false){
                /* drop some data */
                hwDataLength = this.hwSize - this.hwLength;
            }else{
                /* overwrite some data */ 
                uint16_t hwOverLength = hwDataLength - ((this.hwSize - this.hwLength));
                if(hwOverLength < (this.hwSize - this.hwHead)) {
                    this.hwHead += hwOverLength;
                }else{
                    this.hwHead = hwDataLength - (this.hwSize - this.hwHead);
                } 
                this.hwLength -= hwOverLength;
                this.hwPeek = this.hwHead;
                this.hwPeekLength = this.hwLength;                                         
            }
        }
        if(hwDataLength < (this.hwSize - this.hwTail)) {
            this.hwTail += hwDataLength;
        }else{
            this.hwTail = hwDataLength - (this.hwSize - this.hwTail);
        }
        this.hwLength += hwDataLength;
        this.hwPeekLength += hwDataLength;
    } 
    if(hwDataLength <= (this.hwSize - hwTail)) {
        memcpy(&this.pchBuffer[hwTail], pchByte, hwDataLength);
    }else{
        memcpy(&this.pchBuffer[hwTail], &pchByte[0], this.hwSize - hwTail);
        memcpy(&this.pchBuffer[0], &pchByte[this.hwSize - hwTail], hwDataLength - (this.hwSize - hwTail));
    }
    this.bMutex = false;
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
    uint16_t hwHead = this.hwHead;
    bool bEarlyReturn = false;      
    safe_atom_code() {
        if(this.hwHead == this.hwTail &&
           0 == this.hwLength ){
            /* queue is empty */
            bEarlyReturn = true;				 
            continue;
        }			
        if(!this.bMutex){
            this.bMutex  = true;
        }else{
            bEarlyReturn = true;
        }					
    }
    if(bEarlyReturn){
        return false;
    }	
    safe_atom_code() {
        this.hwHead++;
        if(this.hwHead >= this.hwSize){
            this.hwHead = 0;
        }
        this.hwLength--;
        this.hwPeek = this.hwHead;
        this.hwPeekLength = this.hwLength;			
    }
   *pchByte = this.pchBuffer[hwHead];
    this.bMutex = false;
    return true;
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
        if(this.hwHead == this.hwTail &&
           0 == this.hwLength ){
            /* queue is empty */
            bEarlyReturn = true;						 
            continue;
        }			
        if(!this.bMutex){
            this.bMutex  = true;
        }else{
            bEarlyReturn = true;
        }					
    }
    if(bEarlyReturn){
        return 0;
    }	
    uint8_t *pchByte = pDate;	
    uint16_t hwHead = this.hwHead;
    safe_atom_code() {
        if(hwDataLength > this.hwLength){
            /* less data */
            hwDataLength = this.hwLength;
        }			
        if(hwDataLength < (this.hwSize - this.hwHead)) {
            this.hwHead += hwDataLength;
        }else{
            this.hwHead = hwDataLength - (this.hwSize - this.hwHead);
        }					
        this.hwLength -= hwDataLength;
        this.hwPeek = this.hwHead;
        this.hwPeekLength = this.hwLength;        
    }	
    if(hwDataLength <= (this.hwSize - hwHead)) {
        memcpy(pchByte, &this.pchBuffer[hwHead], hwDataLength);
    }else{
        memcpy(&pchByte[0], &this.pchBuffer[hwHead], this.hwSize - hwHead);
        memcpy(&pchByte[this.hwSize - hwHead], &this.pchBuffer[0], hwDataLength - (this.hwSize - hwHead));
    }			
    this.bMutex = false;			
    return hwDataLength;
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
    uint16_t hwPeek = this.hwPeek;		
    bool bEarlyReturn = false;      
    safe_atom_code() {
        if(this.hwPeek == this.hwTail &&
           0 == this.hwPeekLength ){
            /* empty */
            bEarlyReturn = true;
            continue;
        }			
        if(!this.bMutex){
            this.bMutex  = true;
        }else{
            bEarlyReturn = true;
        }					
    }
    if(bEarlyReturn){
        return false;
    }
    safe_atom_code() {	
        this.hwPeek++;			
        if(this.hwPeek >= this.hwSize){
            this.hwPeek = 0;
        }
        this.hwPeekLength--;			
    }
    *pchByte = this.pchBuffer[hwPeek];
    this.bMutex = false;
    return true;
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
		
    bool bEarlyReturn = false;
    safe_atom_code() {
        if(this.hwPeek == this.hwTail &&
           0 == this.hwPeekLength ){
            /* empty */
            bEarlyReturn = true;
            continue;
        }			
        if(!this.bMutex){
            this.bMutex  = true;
        }else{
            bEarlyReturn = true;
        }					
    }
    if(bEarlyReturn){
        return 0;
    }
    uint8_t *pchByte = pDate;
    uint16_t hwPeek = this.hwPeek;		
    safe_atom_code() {	
        if(hwDataLength > this.hwPeekLength){
            /* less data */
            hwDataLength = this.hwPeekLength;
        }			
        if(hwDataLength < (this.hwSize - this.hwPeek)) {
            this.hwPeek += hwDataLength;
        }else{
            this.hwPeek = hwDataLength - (this.hwSize - this.hwPeek);					
        }
        this.hwPeekLength -= hwDataLength;       				
    }
    if(hwDataLength <= (this.hwSize - hwPeek)) {
        memcpy(pchByte, &this.pchBuffer[hwPeek], hwDataLength);
    }else{
        memcpy(&pchByte[0], &this.pchBuffer[hwPeek], this.hwSize - hwPeek);
        memcpy(&pchByte[this.hwSize - hwPeek], &this.pchBuffer[0], hwDataLength - (this.hwSize - hwPeek));				
    }		
    this.bMutex = false;	
    return hwDataLength;
}

/****************************************************************************
* Function: reset_peek                                                    *
* Description: Resets the peek buffer to its initial state.               *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
****************************************************************************/

void reset_peek(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);

    safe_atom_code() {
        this.hwPeek = this.hwHead;
        this.hwPeekLength = this.hwLength;
    }
}

/****************************************************************************
* Function: get_all_peeked                                                *
* Description: Moves all peeked elements back to the queue.              *
* Parameters:                                                             *
*   - ptObj: Pointer to the byte_queue_t object.                         *
****************************************************************************/

void get_all_peeked(byte_queue_t *ptObj)
{
    assert(NULL != ptObj);
    /* initialise "this" (i.e. ptThis) to access class members */
    class_internal(ptObj, ptThis, byte_queue_t);
    safe_atom_code() {
        this.hwHead = this.hwPeek;
        this.hwLength = this.hwPeekLength;
    }
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
****************************************************************************/

void restore_peek_status(byte_queue_t *ptObj, uint16_t hwCount)
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
}

#endif
