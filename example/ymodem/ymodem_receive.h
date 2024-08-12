#ifndef APPLICATIONS_CHECK_AGENT_XMODEM_RECEIVE_H_
#define APPLICATIONS_CHECK_AGENT_XMODEM_RECEIVE_H_
#include "ymodem.h"

typedef struct ymodem_receive_t {
    ymodem_t      parent;
    char          chFileName[32];
    char         *pchFileSize;
    uint32_t      wFileSize;
    uint32_t      wOffSet;
} ymodem_receive_t;

extern ymodem_receive_t *ymodem_receive_init(ymodem_receive_t *ptObj);

#endif /* APPLICATIONS_CHECK_AGENT_XMODEM_H_ */
