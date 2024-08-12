#include "critical.h"

bool enter_critical_sector(mutex_t *ptMutex)
{
    assert(NULL != ptMutex); 
    
    if(ptMutex->bLocked == false){
        ptMutex->bLocked = true;
        return true;
    }
    
    return false;
}

void leave_critical_sector(mutex_t *ptMutex)
{
    assert(NULL != ptMutex); 
    
    ptMutex->bLocked = false;
}

void init_critical_sector(mutex_t *ptMutex)
{
    assert(NULL != ptMutex); 

    ptMutex->bLocked = false;
}

