#ifndef __CRITICAL_SECTOR_H
#define __CRITICAL_SECTOR_H
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool bLocked;
}critical_sector_t;
typedef critical_sector_t mutex_t;

#define ENTER_CRITICAL_SECTOR(__CRITICAL)    enter_critical_sector(__CRITICAL)
#define LEAVE_CRITICAL_SECTOR(__CRITICAL)    leave_critical_sector(__CRITICAL)
#define INIT_CRITICAL_SECTOR(__CRITICAL)     init_critical_sector(__CRITICAL)

extern bool enter_critical_sector(mutex_t *ptMutex);   
extern void leave_critical_sector(mutex_t *ptMutex);
extern void init_critical_sector(mutex_t *ptMutex);

#ifdef __cplusplus
}
#endif

#endif 
