#ifndef __QUEUE_PORT_DEFAULT_H__
#define __QUEUE_PORT_DEFAULT_H__
#include "cmsis_compiler.h"	

typedef uint32_t queue_global_interrupt_status_t;

static
inline 
queue_global_interrupt_status_t queue_port_disable_global_interrupt(void)
{
    queue_global_interrupt_status_t tStatus = __get_PRIMASK();
    __disable_irq();
    
    return tStatus;
}

static
inline 
void queue_port_resume_global_interrupt(queue_global_interrupt_status_t tStatus)
{
    __set_PRIMASK(tStatus);
}


#endif
/* EOF */


