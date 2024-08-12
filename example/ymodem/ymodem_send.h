#ifndef APPLICATIONS_CHECK_AGENT_XMODEM_SEND_H_
#define APPLICATIONS_CHECK_AGENT_XMODEM_SEND_H_
#include "ymodem.h"

typedef struct ymodem_send_t {
    ymodem_t      parent;
    char          chFileName[32];
    char         *pchFileSize;
    uint32_t      wFileSize;
    uint32_t      wOffSet;
} ymodem_send_t;


extern ymodem_send_t *ymodem_send_init(ymodem_send_t *ptObj);

#endif /* APPLICATIONS_CHECK_AGENT_XMODEM_H_ */
