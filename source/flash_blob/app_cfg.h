//! \note do not move this pre-processor statement to other places
#include "../app_cfg.h"

#ifndef __FLASH_BLOB_CFG_H__
#define __FLASH_BLOB_CFG_H__

/* ===================== Flash device Configuration ========================= */ 
typedef struct flash_blob_t flash_blob_t;
extern const  flash_blob_t  onchip_flash_device;
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

