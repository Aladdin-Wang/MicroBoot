#include "../flash_blob.h"

#ifdef STM32F10x_16
#include "stm32f1xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F10x Low-density Flash", // Device Name (16kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00004000,                 // Device Size in Bytes (16kB)
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

#ifdef STM32F10x_CL
#include "stm32f1xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F10x Connectivity Line Flash",// Device Name (256kB/128kB/64kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00040000,                 // Device Size in Bytes (256kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x0800, 0x000000,           // Sector Size 2kB (128 Sectors)
   SECTOR_END
};
#endif

#ifdef STM32F103xE

#include "stm32f1xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F10x Flash",// Device Name (256kB/128kB/64kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00080000,                 // Device Size in Bytes (64kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x0800, 0x000000,           // Sector Size 2kB (128 Sectors)
   SECTOR_END
};
#endif

#ifdef STM32F105xC

#include "stm32f1xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F10x Flash",// Device Name (256kB/128kB/64kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00040000,                 // Device Size in Bytes (64kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x0800, 0x000000,           // Sector Size 2kB (128 Sectors)
   SECTOR_END
};
#endif

#ifdef STM32F303x8
#include "stm32f1xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F30x Flash",// Device Name (256kB/128kB/64kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00010000,                 // Device Size in Bytes (64kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x0800, 0x000000,           // Sector Size 2kB (128 Sectors)
   SECTOR_END
};
#endif

#ifdef STM32F407xx
#include "stm32f4xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32F4xx 1MB Flash",// Device Name (256kB/128kB/64kB)
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
#ifdef STM32G431xx
#include "stm32g4xx.h"
static struct FlashDevice const FlashDevice  =  {
   FLASH_DRV_VERS,             // Driver Version, do not modify!
   "STM32G4xx 128KB Flash",// Device Name (256kB/128kB/64kB)
   ONCHIP,                     // Device Type
   0x08000000,                 // Device Start Address
   0x00020000,                 // Device Size in Bytes (1024kB)
   1024,                       // Programming Page Size
   0,                          // Reserved, must be 0
   0xFF,                       // Initial Content of Erased Memory
   100,                        // Program Page Timeout 100 mSec
   500,                        // Erase Sector Timeout 500 mSec

// Specify Size and Address of Sectors
   0x0800, 0x00000000,
   SECTOR_END
};
#endif
#ifdef STM32H743xx
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