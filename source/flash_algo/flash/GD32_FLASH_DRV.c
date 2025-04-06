#include "../flash_algo.h"
#include "GD32_FLASH_DEV.c"
/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
	fmc_unlock();
    return (0);
}

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t UnInit(uint32_t fnc)
{
    fmc_lock();	
    return (0);
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t EraseChip(void)
{

    fmc_mass_erase();

    return (0);
}

/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */
static int32_t EraseSector(uint32_t adr)
{
    fmc_page_erase(adr);	
    return (0);
}

/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */
static int32_t ProgramPage(uint32_t addr, uint32_t sz, uint8_t* buf)
{
    int32_t result = 0;
    uint32_t end_addr = addr + sz;
    while (addr < end_addr) {
        if (fmc_word_program(addr, *((uint32_t *)buf)) == FMC_READY) {
            if (*(uint32_t *)addr != *(uint32_t *)buf) {
                result = 1;
                break;
            }
            addr += 4;
            buf  += 4;
        } else {
            result = 1;
            break;
        }
    }
    return result;
}

const  flash_algo_t  onchip_flash_device = {
    .tFlashops.Init = Init,
    .tFlashops.UnInit = UnInit,
    .tFlashops.EraseChip = EraseChip,
    .tFlashops.EraseSector = EraseSector,
    .tFlashops.Program = ProgramPage,
    .tFlashops.Read = NULL,
    .ptFlashDev = &FlashDevice,
};

