#include "micro_shell.h"
#include <stdarg.h>


void shell_printf(const char *format, ...)
{
	int result = 0;
	wl_shell_t *ptShell = shell_console_get();
    va_list args;
    va_start(args, format);
    result = vsnprintf(ptShell->chWriteLineBuf,sizeof(ptShell->chWriteLineBuf),format,args); 
    __shell_write_data(ptShell,ptShell->chWriteLineBuf,result);
    va_end(args);
}

static int msh_help(int argc, char **argv)
{
    #ifdef __ARMCC_VERSION
    extern const int FSymTab$$Base;
    extern const int FSymTab$$Limit;
    shell_printf("\r\nshell commands:\r\n");
    {
        msg_t *ptMsgTableBase = (msg_t *)&FSymTab$$Base;
        msg_t *ptMsgTableLimit = (msg_t *)&FSymTab$$Limit;

        for (uint32_t i = 0; &ptMsgTableBase[i] != ptMsgTableLimit; i++) {
            shell_printf("%-16s - %s\r\n", ptMsgTableBase[i].pchMessage, ptMsgTableBase[i].pchDesc);
        }
    }
    #elif defined (__GNUC__) || defined(__TI_COMPILER_VERSION__) || defined(__TASKING__)
    /* GNU GCC Compiler and TI CCS */
    extern const int __fsymtab_start;
    extern const int __fsymtab_end;
    shell_printf("\r\nshell commands:\r\n");
    {
        msg_t *ptMsgTableBase = (msg_t *)&__fsymtab_start;
        msg_t *ptMsgTableLimit = (msg_t *)&__fsymtab_end;

        for (uint32_t i = 0; &ptMsgTableBase[i] != ptMsgTableLimit; i++) {
            shell_printf("%-16s - %s\r\n", ptMsgTableBase[i].pchMessage, ptMsgTableBase[i].pchDesc);
        }
    }
    #endif

    return 0;
}
MSH_FUNCTION_EXPORT_CMD(msh_help, help, shell help);
