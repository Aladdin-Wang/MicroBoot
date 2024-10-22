#include "print_str.h"
#include <assert.h>
#include "usart.h"
#include <string.h>
DEF_EPOOL(print_string_heap_t, fsm(print_string))

#if defined(PRINT_STR_TYPEDEF_FN) && defined(PRINT_STR_EXTERN_DECLARE)
PRINT_STR_TYPEDEF_FN();
PRINT_STR_EXTERN_DECLARE();
#else
#error  No defined macro WRITE_CHAR and PRINT_STR_EXTERN_DECLARE for print_char
#endif

def_simple_fsm( print_string,
    def_params(
        send_byte_t *ptSendByte;
        const char *pchStr;        
        uint16_t hwIndex;          
    )
)

fsm_initialiser( print_string,
    args(
        const char *pchString,
        send_byte_t *ptSendByte
    ))

    init_body (
        if (NULL == pchString || NULL == ptSendByte ) {
            abort_init();
        }
        this.pchStr = pchString;
        this.ptSendByte = ptSendByte;
        this.hwIndex = 0;
    )

fsm_implementation(  print_string )
{
    def_states( IS_END_OF_STR, OUTPUT_CHAR )

    body_begin();

    on_start(
        this.hwIndex = 0;        
        update_state_to(IS_END_OF_STR);
    )

    state(IS_END_OF_STR) {
        if(this.pchStr[this.hwIndex] == '\0') {
            fsm_cpl();
        }
        update_state_to(OUTPUT_CHAR);
    }
    state(OUTPUT_CHAR) {
        if(WRITE_CHAR(this.ptSendByte,this.pchStr[this.hwIndex])) {
            this.hwIndex++;
            transfer_to (IS_END_OF_STR);
        }
    }
    body_end();
}




