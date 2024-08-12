#ifndef __EVENT_H
#define __EVENT_H
#include ".\app_cfg.h"
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool bAutoReset;
    bool bIsSet;
}event_t;

#define INIT_EVENT(__EVENT, __INIT_VALUE,__MANUAL)    init_enent(__EVENT, __INIT_VALUE,__MANUAL)
#define SET_EVENT(__EVENT)                            set_event(__EVENT)
#define RESET_EVENT(__EVENT)                          reset_event(__EVENT)
#define WAIT_EVENT(__EVENT)                           wait_event(__EVENT)

extern void init_enent(event_t *ptEvent,bool bInitValue,bool bManual);   
extern void reset_event(event_t *ptEvent);
extern bool wait_event(event_t *ptEvent);
extern void set_event(event_t *ptEvent);

#ifdef __cplusplus
}
#endif

#endif 
