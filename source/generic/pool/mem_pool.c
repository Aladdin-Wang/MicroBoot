#include "mem_pool.h"
#include <string.h>
static void slist_push(slist_item_t **ppTop, slist_item_t *pNode)
{
    if(pNode != NULL){
        pNode->ptNext = *ppTop;
        *ppTop = pNode;
    }
}

static void slist_pop(slist_item_t **ppTop, slist_item_t **ppNode)
{
    if(*ppTop != NULL){
        *ppNode = *ppTop;
        *ppTop = (*ppTop)->ptNext;
        (*ppNode)->ptNext = NULL;
    }
}

void * pool_allocate (pool_t *ptPool)
{
    slist_item_t *ptItem = NULL;

    if(ptPool->ptFreeList != NULL){
        slist_pop(&(ptPool->ptFreeList),&ptItem);
        return (void*)(ptItem);
    }
    return NULL;
}

void pool_free( pool_t *ptPool , void *ptItem )
{
    if(ptPool == NULL || ptItem ==NULL) {
        return ;
    }
    uint32_t wKey = (uint32_t)ptItem ^ ((slist_item_t *)ptItem)->wKey;
    if(wKey == (uint32_t)(ptPool)){
        slist_push(&(ptPool->ptFreeList),(slist_item_t *)ptItem);
    }
}

bool pool_init( pool_t *ptPool )
{
    if (NULL == ptPool) {
        return false;
    }
    ptPool->ptFreeList = NULL;
    return true;
}

bool pool_add_heap( pool_t *ptPool , void *ptBuffer,uint32_t wPoolSize ,uint32_t wItemSize)
{
    if(NULL == ptBuffer || NULL == ptPool ||0 == wPoolSize || wPoolSize < wItemSize){
        return false;
    }
    do{
        slist_push(&(ptPool->ptFreeList),ptBuffer);
        ptPool->ptFreeList->wKey = (uint32_t)ptPool->ptFreeList ^ (uint32_t)(ptPool);
        ptBuffer = (void *)((uint8_t *)ptBuffer + wItemSize);
        wPoolSize = wPoolSize - wItemSize;
    }while(wPoolSize >= wItemSize);
    return true;
}
