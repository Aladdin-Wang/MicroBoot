#include "flash_algo.h"

static  flash_dev_t const FlashDevice  =  {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "HPM5301 1MB Flash",     // Device Name (1M)
    ONCHIP,                     // Device Type
    0x80000000,                 // Device Start Address
    0x00100000,                 // Device Size in Bytes (1M)
    0x00001000,                 // Programming Page Size
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    100,                        // Program Page Timeout 100 mSec
    6000,                       // Erase Sector Timeout 6000 mSec
    // Specify Size and Address of Sectors
    0x1000, 0x000000,           // Sector Size  4kB (64 Sectors)
    SECTOR_END
  };

