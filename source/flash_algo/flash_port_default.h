#ifndef __FLASH_PORT_DEFAULT_H__
#define __FLASH_PORT_DEFAULT_H__
#include "cmsis_compiler.h"	

typedef uint32_t flash_global_interrupt_status_t;

static
inline 
flash_global_interrupt_status_t flash_port_disable_global_interrupt(void)
{
    flash_global_interrupt_status_t tStatus = __get_PRIMASK();
    __disable_irq();
    
    return tStatus;
}

static
inline 
void flash_port_resume_global_interrupt(flash_global_interrupt_status_t tStatus)
{
    __set_PRIMASK(tStatus);
}

/* ===================== Flash device Configuration ========================= */ 
typedef struct flash_algo_t flash_algo_t;
extern const  flash_algo_t  onchip_flash_device;
/* flash device table */
#ifndef FLASH_DEV_TABLE
    #define FLASH_DEV_TABLE                                          \
    {                                                                   \
        &onchip_flash_device                                            \
    };                                                                
#endif
/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
 

#endif
/* EOF */


