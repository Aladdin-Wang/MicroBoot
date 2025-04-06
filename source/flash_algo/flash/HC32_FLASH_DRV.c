#include "../flash_algo.h"
#include "HC32_FLASH_DEV.c"
/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    EFM_FWMC_Cmd(ENABLE);	
    return (0);
}

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t UnInit(uint32_t fnc)
{
    EFM_FWMC_Cmd(DISABLE);	
    return (0);
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t EraseChip(void)
{
    EFM_ChipErase(EFM_CHIP_ALL);
    return (0);
}

/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */
static int32_t EraseSector(uint32_t adr)
{		
    EFM_SectorErase(adr);	
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
	 /* program */
	if (LL_OK != EFM_Program(addr, buf, sz))
	{
		result = 1;
	}
    return result;
}

/*
 *  Read Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */
static int32_t ReadPage(uint32_t addr, uint32_t sz, uint8_t* buf)
{
    int32_t result = 0;	
	 /* read */
	if (LL_OK != EFM_ReadByte(addr, buf, sz))
	{
		result = 1;
	}
    return result;
}

const  flash_algo_t  onchip_flash_device = {
    .tFlashops.Init = Init,
    .tFlashops.UnInit = UnInit,
    .tFlashops.EraseChip = EraseChip,
    .tFlashops.EraseSector = EraseSector,
    .tFlashops.Program = ProgramPage,
    .tFlashops.Read = ReadPage,
    .ptFlashDev = &FlashDevice,
};

