#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_
#include ".\app_cfg.h"
#include <stdint.h>
#include <stdbool.h>
#include "../flash_algo/flash_algo.h"
// <o>The user data size
//  <i>Default: 192
#define USER_DATA_SIZE            192

typedef struct {
    char chProjectName[16];
    char chHardWareVersion[16];
    char chSoftBootVersion[16];
    char chSoftAppVersion[16];
    char chReceive[128];	
} msgSig_t;
typedef struct {
    union {
        msgSig_t sig;
        uint8_t B[USER_DATA_SIZE];
    } msg_data;
} user_data_t;

extern user_data_t tUserData;
extern void enter_bootloader(uint8_t *pchDate, uint16_t hwLength);
extern void begin_download(void);
extern void finalize_download(void);

#endif


