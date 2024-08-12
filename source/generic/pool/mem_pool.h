
#ifndef MEMORY_POOL_PRINT_POOL_H_
#define MEMORY_POOL_PRINT_POOL_H_
#include ".\app_cfg.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct slist_item_t slist_item_t;
struct slist_item_t{
    uint32_t wKey;
    slist_item_t * ptNext;
};

typedef struct pool_t  pool_t;
struct pool_t{
    slist_item_t     *ptFreeList;
};

/**
 * return the member address of ptr, if the type of ptr is the
 * struct type.
 */
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define POOL(__NAME)         __NAME##pool_t

#define POOL_INIT(__NAME, __EPOOL)                                                              \
            __NAME##_pool_init((__EPOOL))

#define POOL_ADD_HEAP(__NAME, __EPOOL,__TYPE, __SIZE)                                         \
        static __TYPE s_tPool[__SIZE * (sizeof(__NAME##pool_t))];                              \
        __NAME##_pool_add_heap((__EPOOL), (s_tPool), (sizeof(s_tPool)))

#define POOL_FREE(__NAME, __EPOOL, __ITEM)                                                      \
            __NAME##_pool_free((__EPOOL), (__ITEM))

#define POOL_ALLOCATE(__NAME, __EPOOL)                                                          \
            __NAME##_pool_allocate((__EPOOL))


#define EXTERN_EPOOL(__NAME, __TYPE)                                                            \
        typedef struct __NAME##pool_item_t  __NAME##pool_item_t;                                          \
        struct __NAME##pool_item_t{                                                             \
            uint8_t chMask  [sizeof(struct{                                                     \
                pool_t     tPool;                                                               \
            })]  __attribute__((aligned(__alignof__(struct{                                     \
                    pool_t     tPool;                                                           \
                 }))));                                                                         \
        };                                                                                      \
typedef struct __NAME##pool_t  __NAME##pool_t;                                          \
struct __NAME##pool_t{                                                                  \
    uint8_t chMask  [sizeof(struct{                                                     \
         slist_item_t     tSlistItem;                                                        \
         __TYPE           tMem;                                                              \
    })]  __attribute__((aligned(__alignof__(struct{                                     \
              slist_item_t     tSlistItem;                                                        \
              __TYPE           tMem;                                                              \
        }))));                                                                         \
};                                                                                      \
extern bool __NAME##_pool_init(__NAME##pool_t *ptPool);                                 \
extern bool __NAME##_pool_add_heap(                                                     \
        __NAME##pool_t *ptPool, void *ptBuffer, uint32_t tSize);                        \
extern __TYPE *__NAME##_pool_allocate(__NAME##pool_t *ptPool);                          \
extern void __NAME##_pool_free(__NAME##pool_t *ptPool, __TYPE *ptItem);

#define DEF_EPOOL(__NAME, __TYPE)                                                       \
typedef struct __NAME##_pool_item_t  __NAME##_pool_item_t;                              \
struct __NAME##_pool_item_t{                                                            \
    slist_item_t     tSlistItem;                                                        \
    __TYPE           tMem;                                                              \
};                                                                                      \
typedef struct __NAME##pool_t  __NAME##pool_t;                                          \
typedef struct __NAME##_pool_t  __NAME##_pool_t;                                        \
struct __NAME##_pool_t{                                                                 \
    pool_t     tPool;                                                                   \
};                                                                                      \
bool __NAME##_pool_init(__NAME##pool_t *ptPool)                                         \
{                                                                                       \
    return pool_init((pool_t *)ptPool);                                                 \
}                                                                                       \
bool __NAME##_pool_add_heap(                                                            \
    __NAME##pool_t *ptPool, void *ptBuffer, uint32_t tSize)                             \
{                                                                                       \
    return pool_add_heap((pool_t *)ptPool, (void *)ptBuffer,                            \
                            tSize, sizeof(__NAME##_pool_item_t));                       \
}                                                                                       \
__TYPE *__NAME##_pool_allocate(__NAME##pool_t *ptPool)                                  \
{                                                                                       \
    void *ptAddr = pool_allocate((pool_t *)ptPool);                                     \
    if(ptAddr != NULL){                                                                 \
        __NAME##_pool_item_t *ptPoolItem = (__NAME##_pool_item_t *)ptAddr;              \
        return (__TYPE *) (&(ptPoolItem->tMem));                                        \
    }                                                                                   \
    return NULL;                                                                        \
}                                                                                       \
void __NAME##_pool_free(__NAME##pool_t *ptPool, __TYPE *ptItem)                         \
{                                                                                       \
    __NAME##_pool_item_t *ptItemAddr =  CONTAINER_OF(ptItem,__NAME##_pool_item_t,tMem); \
    pool_free((pool_t *)ptPool, (void *)ptItemAddr);                                    \
}

extern void *pool_allocate (pool_t *ptPool);
extern void pool_free( pool_t *ptPool , void *ptItem );
extern bool pool_init( pool_t *ptPool );
extern bool pool_add_heap( pool_t *ptPool , void *ptBuffer,uint32_t wPoolSize ,uint32_t wItemSize);

#ifdef __cplusplus
}
#endif
#endif /* MEMORY_POOL_PRINT_POOL_H_ */


