#include "../flash_algo.h"
#include "STM32_FLASH_DEV.c"

typedef volatile unsigned char    vu8;
typedef          unsigned char     u8;
typedef volatile unsigned short   vu16;
typedef          unsigned short    u16;
typedef volatile unsigned long    vu32;
typedef          unsigned long     u32;

#define M8(adr)  (*((vu8  *) (adr)))
#define M16(adr) (*((vu16 *) (adr)))
#define M32(adr) (*((vu32 *) (adr)))
	
#if defined(STM32F10x_128)||defined(STM32F10x_512)||defined(STM32F10x_1024)

    // FLASH BANK size
    #define BANK1_SIZE      0x08080000      // Bank1 Size = 512kB

    // Flash Control Register definitions
    #define FLASH_PG        0x00000001
    #define FLASH_PER       0x00000002
    #define FLASH_MER       0x00000004
    #define FLASH_OPTPG     0x00000010
    #define FLASH_OPTER     0x00000020
    #define FLASH_STRT      0x00000040
    #define FLASH_LOCK      0x00000080
    #define FLASH_OPTWRE    0x00000100
    // Flash Status Register definitions
    #define FLASH_BSY       0x00000001
    #define FLASH_PGERR     0x00000004
    #define FLASH_WRPRTERR  0x00000010
    #define FLASH_EOP       0x00000020
#endif

#if defined(STM32F2xx_1024)
	// Flash Control Register definitions
	#define FLASH_PG                ((unsigned int)0x00000001)
	#define FLASH_SER               ((unsigned int)0x00000002)
	#define FLASH_MER               ((unsigned int)0x00000004)
	#define FLASH_SNB_POS           ((unsigned int)0x00000003)
	#define FLASH_SNB_MSK           ((unsigned int)0x00000078)
	#define FLASH_PSIZE_POS         ((unsigned int)0x00000008)
	#define FLASH_PSIZE_MSK         ((unsigned int)0x00000300)
	#define FLASH_STRT              ((unsigned int)0x00010000)
	#define FLASH_LOCK              ((unsigned int)0x80000000)

	// Flash Option Control Register definitions
	#define FLASH_OPTLOCK           ((unsigned int)0x00000001)
	#define FLASH_OPTSTRT           ((unsigned int)0x00000002)


	#define FLASH_PSIZE_Byte        ((unsigned int)0x00000000)
	#define FLASH_PSIZE_HalfWord    ((unsigned int)0x00000100)
	#define FLASH_PSIZE_Word        ((unsigned int)0x00000200)
	#define FLASH_PSIZE_DoubleWord  ((unsigned int)0x00000300)


	// Flash Status Register definitions
	#define FLASH_EOP               ((unsigned int)0x00000001)
	#define FLASH_OPERR             ((unsigned int)0x00000002)
	#define FLASH_WRPERR            ((unsigned int)0x00000010)
	#define FLASH_PGAERR            ((unsigned int)0x00000020)
	#define FLASH_PGPERR            ((unsigned int)0x00000040)
	#define FLASH_PGSERR            ((unsigned int)0x00000080)
	#define FLASH_BSY               ((unsigned int)0x00010000)

	#define FLASH_PGERR             (FLASH_PGSERR | FLASH_PGPERR | FLASH_PGAERR | FLASH_WRPERR)
	
/*
 * Get Sector Number
 *    Parameter:      adr:  Sector Address
 *    Return Value:   Sector Number
 */

unsigned long GetSecNum (unsigned long adr) {
  unsigned long n;

  n = (adr >> 12) & 0x000FF;                             // only bits 12..19

  if    (n >= 0x20) {
    n = 4 + (n >> 5);                                    // 128kB Sector
  }
  else if (n >= 0x10) {
    n = 3 + (n >> 4);                                    //  64kB Sector
  }
  else                {
    n = 0 + (n >> 2);                                    //  16kB Sector
  }

  return (n);                                            // Sector Number
}
	
#endif

#if defined(STM32G4xx_512)

#define M32(adr) (*((vu32 *) (adr)))
#define FLASH_SR_MISSERR        ((u32)(  1U <<  8))
static void DSB(void)
{
    __asm("DSB");
}

/* Flash Keys */
#if !defined  (USE_HAL_DRIVER)
    #define FLASH_KEY1               0x45670123
    #define FLASH_KEY2               0xCDEF89AB
    #define FLASH_OPTKEY1            0x08192A3B
    #define FLASH_OPTKEY2            0x4C5D6E7F
#endif
/* Flash Control Register definitions */

#define FLASH_CR_PNB_MSK        ((u32)(0x7F <<  3))
#define FLASH_CR_BKER           ((u32)(  1U << 11))
#define FLASH_CR_MER2           ((u32)(  1U << 15))

/* Flash Status Register definitions */

#define FLASH_PGERR             (FLASH_SR_OPERR   | FLASH_SR_PROGERR | FLASH_SR_WRPERR  | \
                                 FLASH_SR_PGAERR  | FLASH_SR_SIZERR  | FLASH_SR_PGSERR  | \
                                 FLASH_SR_MISSERR | FLASH_SR_FASTERR | FLASH_SR_RDERR   | FLASH_SR_OPTVERR )

/* Flash option register definitions */
#define FLASH_OPTR_RDP_NO       ((u32)(0xAA      ))
#define FLASH_OPTR_DBANK        ((u32)(  1U << 22))

static u32 GetFlashType (void)
{
    u32 flashType;

    switch ((DBGMCU->IDCODE & 0xFFFU)) {
        case 0x468:             /* Flash Category 2 devices, 2k sectors */
        case 0x479:             /* Flash Category 4 devices, 2k sectors */
            /* devices have only a singe bank flash */
            flashType = 0U;       /* Single-Bank Flash type */
            break;

        case 0x469:             /* Flash Category 3 devices, 2k or 4k sectors */
        default:                /* devices have a dual bank flash, configurable via FLASH_OPTR.DBANK */
            flashType = 1U;       /* Dual-Bank Flash type */
            break;
    }

    return (flashType);
}
static u32 GetFlashBankMode (void)
{
    u32 flashBankMode;

    flashBankMode = (FLASH->OPTR & FLASH_OPTR_DBANK) ? 1U : 0U;

    return (flashBankMode);
}
static u32 GetFlashBankNum(u32 adr)
{
	
    u32 flashBankNum,flashSize,flashBankSize;
	flashSize = ((*((u32 *)FLASHSIZE_BASE)) & 0x0000FFFF) << 10;
    flashBankSize = flashSize >> 1;
    if (GetFlashType() == 1U) {
        /* Dual-Bank Flash */
        if (GetFlashBankMode() == 1U) {
            /* Dual-Bank Flash configured as Dual-Bank */
            if (adr >= (FLASH_BASE + flashBankSize)) {
                flashBankNum = 1U;
            } else {
                flashBankNum = 0U;
            }
        } else {
            /* Dual-Bank Flash configured as Single-Bank */
            flashBankNum = 0U;
        }
    } else {
        /* Single-Bank Flash */
        flashBankNum = 0u;
    }

    return (flashBankNum);
}
#endif
#if defined(STM32F4xx_1024) || defined(STM32F4xx_2048) || defined(STM32F4xx_1536) || defined(STM32F4xx_1024dual)

    // Flash Keys
#if !defined  (USE_HAL_DRIVER)		
    #define FLASH_KEY1      0x45670123
    #define FLASH_KEY2      0xCDEF89AB
    #define FLASH_OPTKEY1   0x08192A3B
    #define FLASH_OPTKEY2   0x4C5D6E7F
#endif

    // Flash Control Register definitions
    #define FLASH_PG                ((unsigned int)0x00000001)
    #define FLASH_SER               ((unsigned int)0x00000002)
    #define FLASH_MER               ((unsigned int)0x00000004)
    #define FLASH_MER1              ((unsigned int)0x00008000)
    #define FLASH_SNB_POS           ((unsigned int)0x00000003)
    #define FLASH_SNB_MSK           ((unsigned int)0x000000F8)
    #define FLASH_PSIZE_POS         ((unsigned int)0x00000008)
    #define FLASH_PSIZE_MSK         ((unsigned int)0x00000300)
    #define FLASH_STRT              ((unsigned int)0x00010000)
    #define FLASH_LOCK              ((unsigned int)0x80000000)

    // Flash Option Control Register definitions
    #define FLASH_OPTLOCK           ((unsigned int)0x00000001)
    #define FLASH_OPTSTRT           ((unsigned int)0x00000002)


    #define FLASH_PSIZE_Byte        ((unsigned int)0x00000000)
    #define FLASH_PSIZE_HalfWord    ((unsigned int)0x00000100)
    #define FLASH_PSIZE_Word        ((unsigned int)0x00000200)
    #define FLASH_PSIZE_DoubleWord  ((unsigned int)0x00000300)


    // Flash Status Register definitions
    #define FLASH_EOP               ((unsigned int)0x00000001)
    #define FLASH_OPERR             ((unsigned int)0x00000002)
    #define FLASH_WRPERR            ((unsigned int)0x00000010)
    #define FLASH_PGAERR            ((unsigned int)0x00000020)
    #define FLASH_PGPERR            ((unsigned int)0x00000040)
    #define FLASH_PGSERR            ((unsigned int)0x00000080)
    #define FLASH_BSY               ((unsigned int)0x00010000)
    #define FLASH_PGERR             (FLASH_PGSERR | FLASH_PGPERR | FLASH_PGAERR | FLASH_WRPERR)

#endif
#if defined(STM32H7x_2048)
typedef volatile unsigned long    vu32;
typedef          unsigned long     u32;

#define M32(adr) (*((vu32 *) (adr)))

/* Flash Control Register definitions */
#define FLASH_CR_PSIZE_2    (2U <<  4)   /* Flash program/erase by 32 bits */
#define FLASH_CR_PSIZE_3    (3U <<  4)   /* Flash program/erase by 64 bits */
#define FLASH_CR_PSIZE_VAL  FLASH_CR_PSIZE_3
/* Flash Status Register definitions */
#define FLASH_BSY            FLASH_SR_QW
#define FLASH_PGERR         (FLASH_SR_WRPERR   | FLASH_SR_PGSERR   | \
                             FLASH_SR_STRBERR  | FLASH_SR_INCERR   | \
                             FLASH_SR_OPERR    | FLASH_SR_RDPERR   | \
                             FLASH_SR_RDSERR   | FLASH_SR_SNECCERR | \
                             FLASH_SR_DBECCERR | FLASH_SR_CRCRDERR  )

#define FLASH_START             (0x08000000U)
unsigned long GetBankNum(unsigned long adr)
{
    unsigned long bankNum;

    if (adr >= (FLASH_START + FLASH_BANK_SIZE)) {
        bankNum = 1U;
    } else {
        bankNum = 0U;
    }

    return (bankNum);
}

#endif
/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    #if defined(STM32F4xx_1024) || defined(STM32F4xx_2048) || defined(STM32F4xx_1536) || defined(STM32F4xx_1024dual)
    FLASH->KEYR = FLASH_KEY1;                             // Unlock Flash
    FLASH->KEYR = FLASH_KEY2;

    FLASH->ACR &= 0x0000000F;                             // keep Wait States, no Cache, no Prefetch
    FLASH->SR  |= FLASH_PGERR;                            // Reset Error Flags

    #elif defined(STM32G4xx_512)
    FLASH->KEYR = FLASH_KEY1;                              /* Unlock Flash operation */
    FLASH->KEYR = FLASH_KEY2;

    /* Wait until the flash is ready */
    while (FLASH->SR & FLASH_SR_BSY);


    #elif defined(STM32F10x_128)||defined(STM32F10x_512)||defined(STM32F10x_1024)
    // Unlock Flash
    FLASH->KEYR  = FLASH_KEY1;
    FLASH->KEYR  = FLASH_KEY2;

    #ifdef STM32F10x_1024
    FLASH->KEYR2 = FLASH_KEY1;                    // Flash bank 2
    FLASH->KEYR2 = FLASH_KEY2;
    #endif
    #elif defined(STM32F2xx_1024)
    FLASH->KEYR  = FLASH_KEY1;
    FLASH->KEYR  = FLASH_KEY2;
	FLASH->ACR &= 0x0000000F;                             // keep Wait States, no Cache, no Prefetch
	FLASH->SR  |= FLASH_PGERR;                            // Reset Error Flags
    #elif defined(STM32H7x_2048)
    FLASH->KEYR1 = FLASH_KEY1;                     /* Unlcock FLASH A Registers access */
    FLASH->KEYR1 = FLASH_KEY2;
    FLASH->CCR1  = FLASH_PGERR;                    /* Clear status register  */

    FLASH->KEYR2 = FLASH_KEY1;                     /* Unlcock FLASH B Registers access */
    FLASH->KEYR2 = FLASH_KEY2;
    FLASH->CCR2  = FLASH_PGERR;                    /* Clear status register  */
    #endif	
    return (0);
}

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t UnInit(uint32_t fnc)
{
    #if defined(STM32F4xx_1024) || defined(STM32F4xx_2048) || defined(STM32F4xx_1536) || defined(STM32F4xx_1024dual)
    FLASH->CR |=  FLASH_LOCK;                             // Lock Flash
    #elif defined(STM32G4xx_512)
    FLASH->CR |= FLASH_CR_LOCK;                            /* Lock Flash operation */
    DSB();
    #elif defined(STM32F10x_128)||defined(STM32F10x_512)||defined(STM32F10x_1024)
    FLASH->CR  |=  FLASH_LOCK;
    #ifdef STM32F10x_1024
    FLASH->CR2 |=  FLASH_LOCK;                    // Flash bank 2
    #endif
    #elif defined(STM32F2xx_1024)
    FLASH->CR |=  FLASH_LOCK;                             // Lock Flash
    #elif defined (STM32H7x_2048)
    FLASH->CR1 |=  FLASH_CR_LOCK;                  /* Lock FLASH A Registers access */
    FLASH->CR2 |=  FLASH_CR_LOCK;                  /* Lock FLASH B Registers access */
    #endif	
    return (0);
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

static int32_t EraseChip(void)
{
    #if defined(STM32F4xx_1024) || defined(STM32F4xx_2048) || defined(STM32F4xx_1536) || defined(STM32F4xx_1024dual)
    FLASH->CR |=  FLASH_MER;                              // Mass Erase Enabled (sectors  0..11)
    #ifdef STM32F4xx_2048
    FLASH->CR |=  FLASH_MER1;                             // Mass Erase Enabled (sectors 12..23)
    #endif

    FLASH->CR |=  FLASH_STRT;                             // Start Erase

    while (FLASH->SR & FLASH_BSY) {
        IWDG->KR = 0xAAAA;                                  // Reload IWDG
    }

    FLASH->CR &= ~FLASH_MER;                              // Mass Erase Disabled
    #ifdef STM32F4xx_2048
    FLASH->CR &= ~FLASH_MER1;                             // Mass Erase Disabled
    #endif

    #elif defined(STM32G4xx_512)
    FLASH->SR  = FLASH_PGERR;                              /* Reset Error Flags */

    FLASH->CR  = (FLASH_CR_MER1 | FLASH_CR_MER2);          /* Bank A/B mass erase enabled */
    FLASH->CR |=  FLASH_CR_STRT;                           /* Start erase */
    DSB();

    while (FLASH->SR & FLASH_SR_BSY);

    #elif defined (STM32F103xB) || defined (STM32F103xE) || defined(STM32F105xC)
    FLASH->CR  |=  FLASH_MER;                     // Mass Erase Enabled
    FLASH->CR  |=  FLASH_STRT;                    // Start Erase

    while (FLASH->SR  & FLASH_BSY);

    FLASH->CR  &= ~FLASH_MER;                     // Mass Erase Disabled

    #ifdef STM32F10x_1024                           // Flash bank 2
    FLASH->CR2 |=  FLASH_MER;
    FLASH->CR2 |=  FLASH_STRT;

    while (FLASH->SR2 & FLASH_BSY);

    FLASH->CR2 &= ~FLASH_MER;
    #endif
    #elif defined(STM32F2xx_1024)
    FLASH->CR |=  FLASH_MER;                              // Mass Erase Enabled
    FLASH->CR |=  FLASH_STRT;                             // Start Erase

    while (FLASH->SR & FLASH_BSY);

    FLASH->CR &= ~FLASH_MER;                              // Mass Erase Disabled

    #elif defined(STM32H7x_2048)
    FLASH->CCR1 = FLASH_PGERR;                     /* Clear status register  */
    FLASH->CR1  = FLASH_CR_BER | FLASH_CR_PSIZE_VAL;
    FLASH->CR1 |= FLASH_CR_START;

    while (FLASH->SR1 & FLASH_BSY) __NOP();

    FLASH->CR1  =  0;                              /* Reset command register */


    FLASH->CCR2 = FLASH_PGERR;                     /* Clear status register  */
    FLASH->CR2  = FLASH_CR_BER | FLASH_CR_PSIZE_VAL;
    FLASH->CR2 |= FLASH_CR_START;

    while (FLASH->SR2 & FLASH_BSY) __NOP();

    FLASH->CR2  =  0;                              /* Reset command register */
	
    #endif
    return (0);	
}

/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */
static int32_t EraseSector(uint32_t adr)
{
    int32_t result = 0;
    /*Variable used for Erase procedure*/

    #if  defined(STM32F10x_128)||defined(STM32F10x_512)||defined(STM32F10x_1024)
    #ifdef STM32F10x_1024

    if (adr < (BANK1_SIZE)) {          // Flash bank 2
    #endif
        FLASH->CR  |=  FLASH_PER;                   // Page Erase Enabled
        FLASH->AR   =  adr;                         // Page Address
        FLASH->CR  |=  FLASH_STRT;                  // Start Erase

        while (FLASH->SR  & FLASH_BSY);

        FLASH->CR  &= ~FLASH_PER;                   // Page Erase Disabled
        #ifdef STM32F10x_1024
    } else {                                      // Flash bank 2
        FLASH->CR2 |=  FLASH_PER;
        FLASH->AR2  =  adr;
        FLASH->CR2 |=  FLASH_STRT;

        FLASH->CR2 &= ~FLASH_PER;
    }

    #endif

    #elif defined(STM32F2xx_1024)
	  unsigned long n;

	  n = GetSecNum(adr);                                   // Get Sector Number

	  FLASH->SR |= FLASH_PGERR;                             // Reset Error Flags

	  FLASH->CR  =  FLASH_SER;                              // Sector Erase Enabled 
	  FLASH->CR |=  ((n << FLASH_SNB_POS) & FLASH_SNB_MSK); // Sector Number
	  FLASH->CR |=  FLASH_STRT;                             // Start Erase

      while (FLASH->SR  & FLASH_BSY);

      FLASH->CR &= ~FLASH_SER;                        // Page Erase Disabled
	  if (FLASH->SR & FLASH_PGERR) {                        // Check for Error
		FLASH->SR |= FLASH_PGERR;                           // Reset Error Flags
		result =(1);                                           // Failed
	  }
    #elif defined(STM32G4xx_512)
    u32 b, p;
    b = GetFlashBankNum(adr);                              /* Get Bank Number 0..1  */
    p = get_flash_sector(adr);                              /* Get Page Number 0..127 */
    FLASH->SR  = FLASH_PGERR;                              /* Reset Error Flags */

    FLASH->CR  = (FLASH_CR_PER |                           /* Page Erase Enabled */
                  (p <<  3)    |                           /* page Number. 0 to 127 for each bank */
                  (b << 11)     );
    FLASH->CR |=  FLASH_CR_STRT;                           /* Start Erase */
    DSB();

    while (FLASH->SR & FLASH_SR_BSY);

    if (FLASH->SR & FLASH_PGERR) {                         /* Check for Error */
        FLASH->SR  = FLASH_PGERR;                            /* Reset Error Flags */
        result =(1);                                          /* Failed */
    }

    #elif defined(STM32F4xx_1024) || defined(STM32F4xx_2048) || defined(STM32F4xx_1536) || defined(STM32F4xx_1024dual)

    unsigned long n;

    n = get_flash_sector(adr);//GetSecNum(adr);//                                   // Get Sector Number
    FLASH->SR |= FLASH_PGERR;                             // Reset Error Flags

    FLASH->CR  =  FLASH_SER;                              // Sector Erase Enabled
    FLASH->CR |=  ((n << FLASH_SNB_POS) & FLASH_SNB_MSK); // Sector Number
    FLASH->CR |=  FLASH_STRT;                             // Start Erase

    while (FLASH->SR & FLASH_SR_BSY);

    FLASH->CR &= ~FLASH_SER;                              // Page Erase Disabled

    if (FLASH->SR & FLASH_PGERR) {                        // Check for Error
        FLASH->SR |= FLASH_PGERR;                           // Reset Error Flags
        result =(1);                                          // Failed
    }

    #elif defined(STM32H7x_2048)
    unsigned long b, s;

    b = GetBankNum(adr);                           /* get Flash Bank number */
    s = get_flash_sector(adr);                            /* get Flash sector number */

    if (b == 0) {
        FLASH->CCR1 = FLASH_PGERR;                   /* Clear status register  */
        FLASH->CR1  = ((s << 8) | FLASH_CR_PSIZE_VAL | FLASH_CR_SER);
        FLASH->CR1 |= FLASH_CR_START;
        __ISB();
        __DSB();

        while (FLASH->SR1 & FLASH_BSY) __NOP();

        FLASH->CR1 = 0;                               /* Reset command register */

        if (FLASH->SR1 & FLASH_PGERR)
            result =(1);  
    } else {
        FLASH->CCR2 = FLASH_PGERR;                    /* Clear status register  */
        FLASH->CR2  = ((s << 8) | FLASH_CR_PSIZE_VAL | FLASH_CR_SER);
        FLASH->CR2 |= FLASH_CR_START;
        __ISB();
        __DSB();

        while (FLASH->SR2 & FLASH_BSY) __NOP();

        FLASH->CR2 = 0;                              /* Reset command register */

        if (FLASH->SR2 & FLASH_PGERR)
            result =(1);  
    }
    #endif
    return (result);	
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
    #if defined(STM32F10x_128)||defined(STM32F10x_512)||defined(STM32F10x_1024)
    uint32_t end_addr   = addr + sz;

    sz = (sz + 1) & ~1;                           // Adjust size for Half Words

    #ifdef STM32F10x_1024

    if (adr < (BANK1_SIZE)) {          // Flash bank 2
    #endif
        while (sz) {

            FLASH->CR  |=  FLASH_PG;                  // Programming Enabled

            M16(addr) = *((unsigned short *)buf);      // Program Half Word

            while (FLASH->SR  & FLASH_BSY);

            FLASH->CR  &= ~FLASH_PG;                  // Programming Disabled

            // Check for Errors
            if (FLASH->SR  & (FLASH_PGERR | FLASH_WRPRTERR)) {
                FLASH->SR  |= FLASH_PGERR | FLASH_WRPRTERR;
                result =(1);                             // Failed
				break;
            }

            // Go to next Half Word
            addr += 2;
            buf += 2;
            sz  -= 2;
        }

        #ifdef STM32F10x_1024
    } else {                                      // Flash bank 2
        while (sz) {

            FLASH->CR2 |=  FLASH_PG;

            M16(adr) = *((unsigned short *)buf);

            while (FLASH->SR2 & FLASH_BSY);

            FLASH->CR2 &= ~FLASH_PG;

            // Check for Errors
            if (FLASH->SR2 & (FLASH_PGERR | FLASH_WRPRTERR)) {
                FLASH->SR2 |= FLASH_PGERR | FLASH_WRPRTERR;
                result =(1);  
				break;
            }

            // Go to next Half Word
            adr += 2;
            buf += 2;
            sz  -= 2;
        }
    }

    #endif

    #endif
    #if defined(STM32F2xx_1024)	
	  sz = (sz + 3) & ~3;                                   // Adjust size for Words
	  
	  FLASH->SR |= FLASH_PGERR;                             // Reset Error Flags
	  FLASH->CR  =  0;                                      // reset CR 

	  while (sz) {
		FLASH->CR |= (FLASH_PG              |               // Programming Enabled
					  FLASH_PSIZE_Word);                    // Programming Enabled (Word)

		M32(addr) = *((u32 *)buf);                           // Program Double Word
		while (FLASH->SR & FLASH_BSY);

		FLASH->CR &= ~FLASH_PG;                             // Programming Disabled

		if (FLASH->SR & FLASH_PGERR) {                      // Check for Error
		  FLASH->SR |= FLASH_PGERR;                         // Reset Error Flags
            result =(1);                                       // Failed
			break;
		}

		addr += 4;                                           // Go to next Word
		buf += 4;
		sz  -= 4;
	  }
    #endif
	
    #if defined(STM32F4xx_1024) || defined(STM32F4xx_2048) || defined(STM32F4xx_1536) || defined(STM32F4xx_1024dual)
    sz = (sz + 3) & ~3;                                   // Adjust size for Words

    FLASH->SR |= FLASH_PGERR;                             // Reset Error Flags
    FLASH->CR  =  0;                                      // Reset CR

    while (sz) {
        FLASH->CR |= (FLASH_PG              |               // Programming Enabled
                      FLASH_PSIZE_Word);                    // Programming Enabled (Word)

        M32(addr) = *((u32 *)buf);                           // Program Double Word

        while (FLASH->SR & FLASH_BSY);

        FLASH->CR &= ~FLASH_PG;                             // Programming Disabled

        if (FLASH->SR & FLASH_PGERR) {                      // Check for Error
            FLASH->SR |= FLASH_PGERR;                         // Reset Error Flags
            result =(1);                                       // Failed
			break;
        }

        addr += 4;                                           // Go to next Word
        buf += 4;
        sz  -= 4;
    }

    #endif
	
    #if defined(STM32G4xx_512)//FLASH_TYPEPROGRAM_DOUBLEWORD
    sz = (sz + 7) & ~7;                                    /* Adjust size for two words */

    FLASH->SR  = FLASH_PGERR;                              /* Reset Error Flags */

    FLASH->CR = FLASH_CR_PG;                               /* Programming Enabled */

    while (sz) {
        M32(addr    ) = *((u32 *)(buf + 0));                  /* Program the first word of the Double Word */
        M32(addr + 4) = *((u32 *)(buf + 4));                  /* Program the second word of the Double Word */
        DSB();

        while (FLASH->SR & FLASH_SR_BSY);

        if (FLASH->SR & FLASH_PGERR) {                       /* Check for Error */
            FLASH->SR  = FLASH_PGERR;                          /* Reset Error Flags */
            result =(1);                                         /* Failed */
			break;
        }

        addr += 8;                                            /* Go to next DoubleWord */
        buf += 8;
        sz  -= 8;
    }

    FLASH->CR &= ~(FLASH_CR_PG) ;                          /* Reset CR */

    #endif
    #if defined(STM32H7x_2048)
    unsigned long b;
    unsigned long *dest_addr = ( u32 *)addr;
    unsigned long *src_addr  = ( u32 *)buf;
    unsigned long row_index  = 8;

    b = GetBankNum(addr);                           /* get Flash Bank number */
    sz = (sz + 31) & ~31;                          /* Adjust size for 8 words (256 Bit) */

    while (sz) {
        if (b == 0) {
            FLASH->CCR1 = FLASH_PGERR;                 /* Clear status register  */
            FLASH->CR1  = FLASH_CR_PSIZE_VAL | FLASH_CR_PG;
        } else {
            FLASH->CCR2 = FLASH_PGERR;                 /* Clear status register  */
            FLASH->CR2  = FLASH_CR_PSIZE_VAL | FLASH_CR_PG;
        }

        __ISB();
        __DSB();

        /* Program the 256 bits flash word */
        row_index  = 8;

        do {
            *dest_addr = *src_addr;
            dest_addr++;
            src_addr++;
            row_index--;
        } while (row_index != 0U);

        __ISB();
        __DSB();

        if (b == 0) {
            while (FLASH->SR1 & FLASH_BSY) __NOP();

            FLASH->CR1 = 0;                            /* Reset command register */

            if (FLASH->SR1 & FLASH_PGERR){
                result =(1);  
			    break;
			}
        } else {
            while (FLASH->SR2 & FLASH_BSY) __NOP();

            FLASH->CR2 = 0;                            /* Reset command register */

            if (FLASH->SR2 & FLASH_PGERR){
                result =(1); 
                break;				
			}
        }

        sz  -= 32;
    }
    #endif	
    return (result);
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

