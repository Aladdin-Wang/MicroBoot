#ifndef __PRINT_STR_H
#define __PRINT_STR_H
#include ".\app_cfg.h"
#include "simple_fsm.h"
#include "common.h"
#include "mem_pool.h"
#ifdef __cplusplus
extern "C" {
#endif

declare_simple_fsm(print_string);
extern_fsm_implementation(print_string);
extern_fsm_initialiser( print_string,
        args(
            const char *pchString,
            send_byte_t *ptSendByte
        ))
/*! fsm used to output specified string */
extern_simple_fsm(print_string,
    def_params(
        send_byte_t *ptSendByte;
        const char *pchStr;
        uint16_t hwIndex;
    )
)

EXTERN_EPOOL(print_string_heap_t, fsm(print_string))

#ifdef __cplusplus
}
#endif

#endif 
