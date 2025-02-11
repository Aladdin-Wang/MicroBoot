#include "HPM_FLASH_DEV.c"
#include "hpm_romapi.h"
#include "board.h"
static xpi_nor_config_t s_xpi_nor_config;
/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    xpi_nor_config_option_t option;
    option.header.U = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;
    rom_xpi_nor_auto_config(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, &option);
    return (0);
}

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t UnInit(uint32_t fnc)
{

    return (0);
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t EraseChip(void)
{

    return (0);
}

/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */
static int32_t EraseSector(uint32_t adr)
{
    rom_xpi_nor_erase_sector(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,  (adr - 0x80000000));
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
    rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,
        (uint32_t*)buf, (addr - 0x80000000), sz);
    return result;
}

int32_t ReadData(uint32_t adr, uint32_t sz, uint8_t* buf)
{
    int32_t result = 0;
    rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)buf, (adr - 0x80000000), sz);    
    return result;
}

const  flash_algo_t  onchip_flash_device = {
    .tFlashops.Init = Init,  
    .tFlashops.UnInit = UnInit,  
    .tFlashops.EraseChip = EraseChip,  
    .tFlashops.EraseSector = EraseSector,  
    .tFlashops.Program = ProgramPage,  
    .tFlashops.Read = ReadData,
    .ptFlashDev = &FlashDevice,
};

