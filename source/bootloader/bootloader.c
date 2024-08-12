/****************************************************************************
*  Copyright 2022 kk (https://github.com/Aladdin-Wang)                                    *
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

#include "./app_cfg.h"
#include <stdint.h>
#include <stdbool.h>
#include "bootloader.h"


#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
/* Avoids the semihosting issue */
__asm("  .global __ARM_use_no_argv\n");
#elif defined(__GNUC__)
/* Disables part of C/C++ runtime startup/teardown */
void __libc_init_array (void) {}
#endif

#if defined(__CC_ARM)
__asm void modify_stack_pointer_and_start_app(uint32_t r0_sp, uint32_t r1_pc)
{
    MOV SP, R0
    BX R1
}
#elif defined(__GNUC__)
void modify_stack_pointer_and_start_app(uint32_t r0_sp, uint32_t r1_pc)
{
    uint32_t z = 0;
    __asm volatile (  "msr    control, %[z]   \n\t"
                      "isb                    \n\t"
                      "mov    sp, %[r0_sp]    \n\t"
                      "bx     %[r1_pc]"
                      :
                      :   [z] "l" (z),
                      [r0_sp] "l" (r0_sp),
                      [r1_pc] "l" (r1_pc)
                   );
}
#else
#error "Unknown compiler!"
#endif

#define MARK_SIZE                 64
/**
 * @brief User data structure.
 * 
 * This structure holds the user data that will be manipulated and stored in flash memory.
 */
user_data_t  tUserData;

/**
 * @brief Array to hold magic values for bootloader operations.
 * 
 * This array holds three sets of magic values used to control the behavior of the bootloader and 
 * application entry processes.
 */
static uint8_t chBootMagic[3][MARK_SIZE];
/**
 * @brief Structure to hold bootloader operations.
 * 
 * This structure holds function pointers for various bootloader operations, including entering 
 * bootloader mode, initializing flash, reading from flash, writing to flash, and erasing flash.
 */
typedef struct {
    void (*fnEnterBootloaderMode)(uint8_t *pchDate, uint16_t hwLength); /**< Function pointer to enter bootloader mode */
    bool (*target_flash_init)(uint32_t addr); /**< Function pointer to initialize flash */
    bool (*target_flash_uninit)(uint32_t addr); /**< Function pointer to uninitialize flash */
    int  (*target_flash_read)(uint32_t addr, uint8_t *buf, size_t size); /**< Function pointer to read from flash */
    int  (*target_flash_write)(uint32_t addr, const uint8_t *buf, size_t size); /**< Function pointer to write to flash */
    int  (*target_flash_erase)(uint32_t addr, size_t size); /**< Function pointer to erase flash */
} boot_ops_t;

/**
 * @brief Bootloader operations instance.
 * 
 * This constant instance of boot_ops_t holds the function pointers for bootloader operations and 
 * is placed at a specific memory address (0x08001000) in the ARM memory section.
 * 
 * The __attribute__((used)) ensures that the compiler does not optimize away this variable, and 
 * the __attribute__((section(".ARM.__at_0x08001000"))) places it at the specified memory address.
 */
__attribute__((used))
static const boot_ops_t tBootOps  __attribute__ ((section(".ARM.__at_0x08001000"))) = {
    .fnEnterBootloaderMode = enter_bootloader,
    .target_flash_init = target_flash_init,
    .target_flash_erase = target_flash_erase,
    .target_flash_write = target_flash_write,
    .target_flash_read = target_flash_read,
    .target_flash_uninit = target_flash_uninit
};


/**********************************************************************************************************

|    Distribution    |    UserData Backup    |       UserData        | Magic1    | Magic2    | Magic3    | 
|--------------------|-----------------------|-----------------------|-----------|-----------|-----------|
|     size           |      192 Byte         |      192 Byte         | 64 Byte   | 64 Byte   | 64 Byte   | 
|++++++++++++++++++++|+++++++++++++++++++++++|+++++++++++++++++++++++|+++++++++++|+++++++++++|+++++++++++|
| enter_bootloader   |      XXXX             |      user_data...     |   XXXX    |   XXXX    |    0x00   |
|--------------------|-----------------------|-----------------------|-----------|-----------|-----------|
| begin_download     |      user_data...     |      0xFF, 0XFF...    |   0XFF    |   0X00    |    0xFF   |
|--------------------|-----------------------|-----------------------|-----------|-----------|-----------|
| finalize_download  |      user_data...     |      0xFF, 0XFF...    |   0X00    |   0X00    |    0xFF   |  

***********************************************************************************************************/

/**
 * @brief Prepare the system to enter bootloader mode.
 * 
 * This function initializes the flash memory, writes the provided user data to a specific location, 
 * and sets the Magic3 value to 0x00 to indicate that the system should enter bootloader mode on the next reset.
 * 
 * Memory Layout After Execution:
 * |    Distribution    |      UserData Backup  |      UserData         | Magic1    | Magic2    | Magic3    |
 * |--------------------|-----------------------|-----------------------|-----------|-----------|-----------|
 * | enter_bootloader   |      XXXX             |      user_data...     |   XXXX    |   XXXX    |    0x00   |
 * 
 * @param pchDate Pointer to the data to be written to flash.
 * @param hwLength Length of the data to be written.
 */
void enter_bootloader(uint8_t *pchDate, uint16_t hwLength)
{
	  uint32_t wData = 0;
    target_flash_init(APP_PART_ADDR);
    target_flash_write((APP_PART_ADDR + APP_PART_SIZE - (3*MARK_SIZE) - (USER_DATA_SIZE)), pchDate, USER_DATA_SIZE);
    target_flash_write((APP_PART_ADDR + APP_PART_SIZE - MARK_SIZE), (const uint8_t *)&wData, sizeof(wData));
    target_flash_uninit(APP_PART_ADDR);
}


void reset_bootloader(void)
{
    target_flash_erase(APP_PART_ADDR + APP_PART_SIZE - (3*MARK_SIZE), 3*MARK_SIZE);
}

/**
 * @brief Initialize flash memory for a new download process.
 * 
 * This function clears a buffer, erases a section of flash memory, and writes initial data to specific locations
 * to prepare for a new firmware download. Magic2 is set to 0x00 to indicate that a download is in progress.
 * 
 * Memory Layout After Execution:
 * |    Distribution    |      UserData Backup  |      UserData         | Magic1    | Magic2    | Magic3    |
 * |--------------------|-----------------------|-----------------------|-----------|-----------|-----------|
 * | begin_download     |      user_data...     |      0xFF, 0XFF...    |   0XFF    |   0X00    |    0xFF   |
 */
void begin_download(void)
{
    memset(chBootMagic, 0, sizeof(chBootMagic));
    target_flash_erase(APP_PART_ADDR + APP_PART_SIZE - (3*MARK_SIZE), 3*MARK_SIZE);
    target_flash_write((APP_PART_ADDR + APP_PART_SIZE - (3*MARK_SIZE) - 2 * (USER_DATA_SIZE)), tUserData.msg_data.B, USER_DATA_SIZE);
    target_flash_write((APP_PART_ADDR + APP_PART_SIZE - (2*MARK_SIZE)), chBootMagic[1], MARK_SIZE);
}

/**
 * @brief Finalize the download process by marking it as complete.
 * 
 * This function writes a specific value to flash memory to indicate the end of the download process.
 * Magic1 and Magic2 are set to 0x00 to indicate that the download process is complete.
 * 
 * Memory Layout After Execution:
 * |    Distribution    |      UserData Backup  |      UserData         | Magic1    | Magic2    | Magic3    |
 * |--------------------|-----------------------|-----------------------|-----------|-----------|-----------|
 * | finalize_download  |      user_data...     |      0xFF, 0XFF...    |   0X00    |   0X00    |    0xFF   |
 */
void finalize_download(void)
{
    memset(chBootMagic, 0X00, sizeof(chBootMagic));
    target_flash_write((APP_PART_ADDR + APP_PART_SIZE - 3*MARK_SIZE), chBootMagic[0], MARK_SIZE);
}


/**
 * @brief Entry point for application mode.
 * 
 * This function is executed at the start of the program, reading specific locations in flash memory to determine
 * whether to enter the application or stay in bootloader mode. Depending on the values read from flash, 
 * it may load user data into a specific buffer or modify the stack pointer and start the application.
 */
__attribute__((constructor))
static void enter_application(void)
{
    do {
        // Read the magic values from flash memory to determine the next action
        target_flash_read((APP_PART_ADDR + APP_PART_SIZE - 3 * MARK_SIZE), chBootMagic[0], 3 * MARK_SIZE);

        // Check if Magic3 is 0x00, indicating to read user data from a specific location
        if ((0 == *(uint32_t *)&chBootMagic[2])) {
            target_flash_read((APP_PART_ADDR + APP_PART_SIZE - (3 * MARK_SIZE) - USER_DATA_SIZE), tUserData.msg_data.B, USER_DATA_SIZE);
            break;
        }

        // Check if Magic2 is 0x00 and Magic1 is 0xFFFFFFFF, indicating to read user data from a different location
        if ((0 == *(uint32_t *)&chBootMagic[1]) && (0XFFFFFFFF == *(uint32_t *)&chBootMagic[0])) {
            target_flash_read((APP_PART_ADDR + APP_PART_SIZE - (3 * MARK_SIZE) - 2 * USER_DATA_SIZE), tUserData.msg_data.B, USER_DATA_SIZE);
            break;
        }

        // Check if the value at the address (APP_PART_ADDR + 4) has the expected application identifier
        if (((*(volatile uint32_t *)(APP_PART_ADDR + 4)) & 0xff000000) != 0x08000000) {
            break;
        }

        // Check if the value at the address APP_PART_ADDR has the expected stack pointer identifier
        if (((*(volatile uint32_t *)APP_PART_ADDR) & 0x20000000) != 0x20000000) {
            break;
        }
        // If all checks are passed, modify the stack pointer and start the application
        modify_stack_pointer_and_start_app(*(volatile uint32_t *)APP_PART_ADDR,
                                           (*(volatile uint32_t *)(APP_PART_ADDR + 4)));

    } while(0);
}





