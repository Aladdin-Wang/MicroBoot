/****************************************************************************
*  Copyright 2022 KK (https://github.com/WALI-KANG)                                    *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/
#include "flash_algo.h"

/* Array containing flash devices and their configurations */
static const flash_algo_t * const flash_table[] = FLASH_DEV_TABLE;

/* Length of the flash_table array */
static const size_t flash_table_len = sizeof(flash_table) / sizeof(flash_table[0]);

/*
 * Function: flash_dev_find
 * Description: Finds the flash device based on the specified address.
 * Parameters:
 *   - addr: Flash memory address to find.
 * Returns: Pointer to the flash_fal_t structure if found, NULL otherwise.
 */
static const flash_algo_t *  flash_dev_find(uint32_t addr)
{
    for (uint16_t i = 0; i < flash_table_len; i++) {
        if(addr >= flash_table[i]->ptFlashDev->DevAdr &&
           addr < flash_table[i]->ptFlashDev->DevAdr + flash_table[i]->ptFlashDev->szDev) {
            return flash_table[i];
        }
    }

    return NULL;
}
/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
uint32_t get_flash_sector(uint32_t Address)
{
    uint32_t wSector = 0;
    const flash_algo_t *ptFlashDevice = flash_dev_find(Address);
    uint8_t chPartitionIndex = 0;

    uint32_t wSectorEnd = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex + 1].AddrSector;
    uint32_t wSectorStart = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex].AddrSector;
    uint32_t wSectorSize = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex].szSector;
    uint32_t Adr = Address - ptFlashDevice->ptFlashDev->DevAdr;

    for(wSector = 0; wSector < 0XFF; wSector++) {

        if((Adr >= wSectorStart && Adr < wSectorStart + wSectorSize)) {
            break;
        }

        wSectorStart += wSectorSize;

        if(wSectorStart >= wSectorEnd && wSectorStart < ptFlashDevice->ptFlashDev->szDev) {
            chPartitionIndex++;
            wSectorEnd = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex + 1].AddrSector;
            wSectorStart = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex].AddrSector;
            wSectorSize = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex].szSector;
        }

        if (wSectorEnd == 0xFFFFFFFF && wSectorStart >= ptFlashDevice->ptFlashDev->szDev) {
            break;
        }

    }

    return wSector;
}

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector size of a given address
  */
uint32_t get_flash_sector_size(uint32_t Address)
{
    uint32_t wSector = 0;
    const flash_algo_t *ptFlashDevice = flash_dev_find(Address);
    uint8_t chPartitionIndex = 0;

    uint32_t wSectorEnd = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex + 1].AddrSector;
    uint32_t wSectorStart = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex].AddrSector;
    uint32_t wSectorSize = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex].szSector;
    uint32_t Adr = Address - ptFlashDevice->ptFlashDev->DevAdr;

    for(wSector = 0; wSector < 0XFF; wSector++) {

        if((Adr >= wSectorStart && Adr < wSectorStart + wSectorSize)) {
            return wSectorSize;
        }

        wSectorStart += wSectorSize;

        if(wSectorStart >= wSectorEnd && wSectorStart < ptFlashDevice->ptFlashDev->szDev) {
            chPartitionIndex++;
            wSectorEnd = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex + 1].AddrSector;
            wSectorStart = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex].AddrSector;
            wSectorSize = ptFlashDevice->ptFlashDev->sectors[chPartitionIndex].szSector;
        }

        if (wSectorEnd == 0xFFFFFFFF && wSectorStart >= ptFlashDevice->ptFlashDev->szDev) {
            break;
        }

    }

    return 0;
}

/*
 * Function: target_flash_init
 * Description: Initializes the flash memory based on the specified address.
 * Parameters:
 *   - addr: Flash memory address to initialize.
 * Returns: True if initialization is successful, false otherwise.
 */
bool target_flash_init(uint32_t addr)
{
    if (addr % 4 != 0) {
        /*flash addr must be 4-byte alignment*/
        return false;
    }

    const flash_algo_t *ptFlashDevice = flash_dev_find(addr);

    if(ptFlashDevice != NULL) {
        ptFlashDevice->tFlashops.Init(addr, 0, 0);
        return true;
    }

    return false;
}
/*
 * Function: target_flash_uninit
 * Description: Uninitializes the flash memory based on the specified address.
 * Parameters:
 *   - addr: Flash memory address to uninitialize.
 * Returns: True if uninitialization is successful, false otherwise.
 */
bool target_flash_uninit(uint32_t addr)
{
    const flash_algo_t *ptFlashDevice = flash_dev_find(addr);

    if(ptFlashDevice != NULL) {
        ptFlashDevice->tFlashops.UnInit(addr);
        return true;
    }

    return true;
}
/*
 * Function: target_flash_write
 * Description: Writes data to the flash memory.
 * Parameters:
 *   - addr: Flash memory address to start writing.
 *   - buf: Pointer to the data to be written.
 *   - size: Number of bytes to write.
 * Returns: Number of bytes actually written.
 */
int target_flash_write(uint32_t addr, const uint8_t *buf, size_t size)
{
    const flash_algo_t *ptFlashDevice = flash_dev_find(addr);

    safe_atom_code() {
        if(ptFlashDevice != NULL) {
            if (addr % 4 != 0) {
                /*addr must be 4-byte alignment*/
                size = 0;
                continue;
            }

            if ((addr - ptFlashDevice->ptFlashDev->DevAdr + size) > ptFlashDevice->ptFlashDev->szDev) {
                /*write outrange flash size*/
                size = 0;
                continue;
            }

            if(ptFlashDevice->tFlashops.Program) {
                if( 0 != ptFlashDevice->tFlashops.Program(addr, size, (uint8_t *)buf)) {
                    /*Programming Failed*/
                    size = 0;
                    continue;
                }
            } else {
                size = 0;
                continue;
            }
        }
    }
    return size;
}

/*
 * Function: target_flash_read
 * Description: Reads data from the flash memory.
 * Parameters:
 *   - addr: Flash memory address to start reading.
 *   - buf: Pointer to store the read data.
 *   - size: Number of bytes to read.
 * Returns: Number of bytes actually read.
 */
int target_flash_read(uint32_t addr, uint8_t *buf, size_t size)
{
    const flash_algo_t *ptFlashDevice = flash_dev_find(addr);

    safe_atom_code() {
        if(ptFlashDevice != NULL) {
            if (addr % 4 != 0) {
                /*addr must be 4-byte alignment*/
                size = 0;
                continue;
            }

            if ((addr - ptFlashDevice->ptFlashDev->DevAdr + size) > ptFlashDevice->ptFlashDev->szDev) {
                /*read outrange flash size*/
                size = 0;
                continue;
            }

            if(ptFlashDevice->tFlashops.Read) {
                if( 0 != ptFlashDevice->tFlashops.Read(addr, size, (uint8_t *)buf)) {
                    /*Read Failed*/
                    size = 0;
                    continue;
                }
            } else {
                for (uint16_t i = 0; i < size; i++, buf++, addr++) {
                    *buf = *(uint8_t *) addr;
                }
            }
        }
    }

    return size;
}

/*
 * Function: target_flash_erase
 * Description: Erases a specified portion of the flash memory.
 * Parameters:
 *   - addr: Flash memory address to start erasing.
 *   - size: Number of bytes to erase.
 * Returns: Number of bytes actually erased.
 */
int target_flash_erase(uint32_t addr, size_t size)
{
    size_t wSector = 0;
    size_t wEraseSize = 0;
    const flash_algo_t *ptFlashDevice = flash_dev_find(addr);

    if(ptFlashDevice != NULL) {
        safe_atom_code() {
            if (size > ptFlashDevice->ptFlashDev->szDev) {
                /*erase outrange flash size */
                wEraseSize = 0;
                continue;
            }

            while(wEraseSize < size) {
                if(ptFlashDevice->tFlashops.EraseSector) {
                    if(0 != ptFlashDevice->tFlashops.EraseSector(addr)) {
                        /*erase Failed*/
                        break;
                    }
                } else {
                    break;
                }

                for(wSector = 0; wSector < SECTOR_NUM; wSector++) {
                    if(ptFlashDevice->ptFlashDev->sectors[wSector + 1].szSector == 0XFFFFFFFF )
                        break;

                    if(((addr - ptFlashDevice->ptFlashDev->DevAdr) < ptFlashDevice->ptFlashDev->sectors[wSector + 1].AddrSector) &&
                       ((addr - ptFlashDevice->ptFlashDev->DevAdr) >= ptFlashDevice->ptFlashDev->sectors[wSector].AddrSector) )
                        break;
                }

                addr += ptFlashDevice->ptFlashDev->sectors[wSector].szSector;
                wEraseSize += ptFlashDevice->ptFlashDev->sectors[wSector].szSector;
            }
        }
        return wEraseSize;
    }

    return 0;
}

