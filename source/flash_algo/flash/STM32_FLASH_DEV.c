#include "../flash_algo.h"

#ifdef STM32F10x_128
#include "stm32f1xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F10x Med-density Flash", // Device Name (128kB/64kB/32kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00020000,                 // Device Size in Bytes (128kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x0400, 0x000000,           // Sector Size 1kB (128 Sectors)
   SECTOR_END
};
#endif

#ifdef STM32F10x_512
#include "stm32f1xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F10x High-density Flash",// Device Name (512kB/384kB/256kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00080000,                 // Device Size in Bytes (512kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x0800, 0x000000,           // Sector Size 2kB (256 Sectors)
   SECTOR_END
};
#endif

#ifdef STM32F10x_1024
#include "stm32f1xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F10x XL-density Flash",// Device Name (1024kB/768kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00100000,                 // Device Size in Bytes (1024kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x0800, 0x000000,           // Sector Size 2kB (512 Sectors)
   SECTOR_END
};
#endif

#ifdef STM32F2xx_1024
#include "stm32f2xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F2xx Flash",          // Device Name (1024kB/768kB/512kB/256kB/128kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00100000,                 // Device Size in Bytes (1024kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   6000,                       // Erase Sector Timeout 6000 mSec

// Specify Size and Address of Sectors
   0x04000, 0x000000,          // Sector Size  16kB (4 Sectors)
   0x10000, 0x010000,          // Sector Size  64kB (1 Sectors)
   0x20000, 0x020000,          // Sector Size 128kB (7/5/3 Sectors)
   SECTOR_END
};
#endif // STM32F2xx_1024

#ifdef STM32F4xx_1024
#include "stm32f4xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F4xx 1MB Flash",      // Device Name (256kB/128kB/64kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00100000,                 // Device Size in Bytes (512kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x04000, 0x000000,          // Sector Size  16kB (4 Sectors)
   0x10000, 0x010000,          // Sector Size  64kB (1 Sectors)
   0x20000, 0x020000,          // Sector Size 128kB (3 Sectors)
   SECTOR_END
};
#endif

#if defined STM32F4xx_1024dual
#include "stm32f4xx.h"
static struct FlashDevice const FlashDevice  =  {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "STM32F4xx 1MB dual bank Flash",     // Device Name
    ONCHIP,                     // Device Type
    0x08000000,                 // Device Start Address
    0x00100000,                 // Device Size in Bytes (1024kB)
    1024,                       // Programming Page Size
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    1000,                       // Program Page Timeout 1000 mSec
    6000,                       // Erase Sector Timeout 6000 mSec
    // Specify Size and Address of Sectors
    0x04000, 0x000000,          // Sector Size  16kB (4 Sectors)
    0x10000, 0x010000,          // Sector Size  64kB (1 Sectors)
    0x20000, 0x020000,          // Sector Size 128kB (3 Sectors)
    0x04000, 0x080000,          // Sector Size  16kB (4 Sectors)
    0x10000, 0x090000,          // Sector Size  64kB (1 Sectors)
    0x20000, 0x0A0000,          // Sector Size 128kB (3 Sectors)
    SECTOR_END
  };
#endif // STM32F7x_1024dual

#ifdef STM32F4xx_1536
#include "stm32f4xx.h"
static struct FlashDevice const FlashDevice  =  {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "STM32F4xx 1.5MB Flash",      // Device Name (1024kB/1536kB)
    ONCHIP,                     // Device Type
    0x08000000,                 // Device Start Address
    0x00180000,                 // Device Size in Bytes (1536kB)
    1024,                       // Programming Page Size
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    100,                        // Program Page Timeout 100 mSec
    6000,                       // Erase Sector Timeout 6000 mSec
    // Specify Size and Address of Sectors
    0x04000, 0x000000,          // Sector Size  16kB (4 Sectors)
    0x10000, 0x010000,          // Sector Size  64kB (1 Sectors)
    0x20000, 0x020000,          // Sector Size 128kB (11 Sectors)
    SECTOR_END
  };
#endif // STM32F2xx_1536

#ifdef STM32F4xx_2048
#include "stm32f4xx.h"
static struct FlashDevice const FlashDevice  =  {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "STM32F4xx 2MB Flash",      // Device Name (2048kB)
    ONCHIP,                     // Device Type
    0x08000000,                 // Device Start Address
    0x00200000,                 // Device Size in Bytes (2048kB)
    1024,                       // Programming Page Size
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    100,                        // Program Page Timeout 100 mSec
    6000,                       // Erase Sector Timeout 6000 mSec
    // Specify Size and Address of Sectors
    0x04000, 0x000000,          // Sector Size  16kB (4 Sectors)
    0x10000, 0x010000,          // Sector Size  64kB (1 Sectors)
    0x20000, 0x020000,          // Sector Size 128kB (7 Sectors)
    0x04000, 0x100000,          // Sector Size  16kB (4 Sectors)
    0x10000, 0x110000,          // Sector Size  64kB (1 Sectors)
    0x20000, 0x120000,          // Sector Size 128kB (7 Sectors)
    SECTOR_END
  };
#endif // STM32F2xx_2048
  
  
#ifdef STM32G4xx_512
#include "stm32g4xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32G4xx 512KB",          // Device Name (256kB/128kB/64kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00080000,                 // Device Size in Bytes (512kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   400,                        // Program Page Timeout 400 mSec
   400,                        // Erase Sector Timeout 400 mSec

// Specify Size and Address of Sectors
   0x0800, 0x00000000,
   SECTOR_END
};
#endif
#ifdef STM32H7x_2048
#include "stm32h7xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32H7x_2048",            // Device Name 
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00200000,                 // Device Size in Bytes (2048kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   6000,                       // Erase Sector Timeout 6000 mSec

// Specify Size and Address of Sectors
   0x20000, 0x000000,          // Sector Size  128kB (16 Sectors)
   SECTOR_END
};

#endif