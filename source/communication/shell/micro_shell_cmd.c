#include "micro_shell.h"
#include <stdarg.h>


/**
 * @brief Thread-safe printf function for shell console output.
 * 
 * This function formats a string and writes it to the shell console's 
 * output buffer. It ensures that the output operation is thread-safe 
 * by using a mutex to prevent concurrent access.
 * 
 * @param format The format string, followed by a variable number of arguments
 *               to format the output according to the format string.
 */
void shell_printf(const char *format, ...)
{
    int result = 0; // Variable to store the formatted output result
    bool bEarlyReturn = false; // Flag to indicate if an early return is needed
    wl_shell_t *ptShell = shell_console_get(); // Get the shell console object

    if(ptShell != NULL) { // Check if ptShell is valid
        safe_atom_code() { // Enter safe atomic operation
            if(!ptShell->bMutex) { // If not locked
                ptShell->bMutex  = true; // Lock to prevent concurrent access
            } else {
                bEarlyReturn = true; // Set early return flag if already locked
            }
        }
        
        if(bEarlyReturn) { // If early return is set
            return; // Exit the function
        }

        va_list args; // Declare variable argument list
        va_start(args, format); // Initialize args to point to the variable arguments

        // Format the string and store it in ptShell->chWriteLineBuf
        result = vsnprintf(ptShell->chWriteLineBuf, sizeof(ptShell->chWriteLineBuf), format, args);

        // Write formatted data to the shell console
        __shell_write_data(ptShell, ptShell->chWriteLineBuf, result);
        
        va_end(args); // End usage of variable argument list
        ptShell->bMutex  = false; // Unlock to allow other accesses
    }
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
