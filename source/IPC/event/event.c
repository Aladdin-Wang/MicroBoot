#include "event.h"

void set_event(event_t *ptEvent)
{
    assert(NULL != ptEvent); 
    
    ptEvent->bIsSet = true;
}

void reset_event(event_t *ptEvent)
{
    assert(NULL != ptEvent); 
    
    ptEvent->bIsSet = false;
}

bool wait_event(event_t *ptEvent)
{ 
    assert(NULL != ptEvent); 
    
    if(ptEvent->bIsSet != false){ 
        if(ptEvent->bAutoReset != false){ 
            reset_event (ptEvent);                              
        } 
        return true;   
    } 
    return false;   
} 

void init_enent(event_t *ptEvent,bool bInitValue,bool bManual)
{
    assert(NULL != ptEvent); 
    
    ptEvent->bIsSet = bInitValue;
    ptEvent->bAutoReset = !bManual;
}

