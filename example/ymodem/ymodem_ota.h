#ifndef APPLICATIONS_CHECK_AGENT_YMODEM_RECEIVE_H_
#define APPLICATIONS_CHECK_AGENT_YMODEM_RECEIVE_H_
#include "microboot.h"

typedef struct ymodem_receive_t {
    ymodem_t      parent;
    peek_byte_t   *ptReadByte;
    char          chFileName[32];
    char         *pchFileSize;
    uint32_t      wFileSize;
    uint32_t      wOffSet;
    SIG_SLOT_OBJ;
} ymodem_receive_t;

typedef struct ymodem_ota_recive_t {
    check_agent_t tCheckAgent;
    ymodem_receive_t tYmodemReceive;
} ymodem_ota_recive_t;

signals(ymodem_rec_sig, ymodem_receive_t *ptThis,
        args(
            uint8_t *pchByte,
            uint16_t hwLen
        ));

extern
ymodem_ota_recive_t *ymodem_ota_receive_init(ymodem_ota_recive_t *ptObj, peek_byte_t *ptReadByte);
#endif /* APPLICATIONS_CHECK_AGENT_XMODEM_H_ */
