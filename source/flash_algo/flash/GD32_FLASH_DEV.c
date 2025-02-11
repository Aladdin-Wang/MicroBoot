#include "../flash_algo.h" 
#ifdef GD32E50X
#include "gd32e50x_fmc.h"
static  flash_dev_t const FlashDevice  =  {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "GD32E50X 512kB Flash",     // Device Name (512kB)
    ONCHIP,                     // Device Type
    0x08000000,                 // Device Start Address
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
#endif // GD32E50X_512

#ifdef GD32F30X_CL
#include "gd32f30x_fmc.h"
static  flash_dev_t const FlashDevice  =  {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "GD32F30X 256kB Flash",     // Device Name (256kB)
    ONCHIP,                     // Device Type
    0x08000000,                 // Device Start Address
    0x00040000,                 // Device Size in Bytes (256kB)
    0x00000800,                 // Programming Page Size
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    100,                        // Program Page Timeout 100 mSec
    6000,                       // Erase Sector Timeout 6000 mSec
    // Specify Size and Address of Sectors
    0x0800, 0x000000,           // Sector Size  2kB (128 Sectors)
    SECTOR_END
  };
#endif // GD32E50X_256

#ifdef GD32F30X_HD
#include "gd32f30x_fmc.h"
static  flash_dev_t const FlashDevice  =  {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "GD32F30X 256kB Flash",     // Device Name (256kB)
    ONCHIP,                     // Device Type
    0x08000000,                 // Device Start Address
    0x00040000,                 // Device Size in Bytes (256kB)
    0x00000800,                 // Programming Page Size
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    100,                        // Program Page Timeout 100 mSec
    6000,                       // Erase Sector Timeout 6000 mSec
    // Specify Size and Address of Sectors
    0x0800, 0x000000,           // Sector Size  2kB (128 Sectors)
    SECTOR_END
  };
#endif // GD32E50X_256

#ifdef GD32F10X_HD
#include "gd32f10x_fmc.h"
static  flash_dev_t const FlashDevice  =  {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "GD32F10X 256kB Flash",     // Device Name (256kB)
    ONCHIP,                     // Device Type
    0x08000000,                 // Device Start Address
    0x00040000,                 // Device Size in Bytes (256kB)
    0x00000800,                 // Programming Page Size
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    100,                        // Program Page Timeout 100 mSec
    6000,                       // Erase Sector Timeout 6000 mSec
    // Specify Size and Address of Sectors
    0x0800, 0x000000,           // Sector Size  2kB (128 Sectors)
    SECTOR_END
  };
#endif // GD32E50X_256
