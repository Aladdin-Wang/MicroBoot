

#include "../flash_algo.h" 
#ifdef HC32F460
#include "hc32_ll_efm.h"
static  flash_dev_t const FlashDevice  =  {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "HC32F460 512kB Flash",     // Device Name (512kB)
    ONCHIP,                     // Device Type
    0x00000000,                 // Device Start Address
    0x00080000,                 // Device Size in Bytes (512kB)
    0x00002000,                 // Programming Page Size
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    100,                        // Program Page Timeout 100 mSec
    6000,                       // Erase Sector Timeout 6000 mSec
    // Specify Size and Address of Sectors
    0x2000, 0x000000,           // Sector Size  8kB (64 Sectors)
    SECTOR_END
  };
#endif 
