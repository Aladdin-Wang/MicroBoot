#include "../msg_map/msg_map.h"
/**
 * @ingroup msh
 *
 * This macro exports a command to module shell.
 *
 * @param command is the name of the command.
 * @param desc is the description of the command, which will show in help list.
 */
#define MSH_FUNCTION_EXPORT_CMD(name, cmd, desc)                      \
    const char __fsym_##cmd##_name[] __section(".rodata.name") = #cmd;    \
    const char __fsym_##cmd##_desc[] __section(".rodata.name") = #desc;   \
    const __used struct _msg_t __fsym_##cmd __section("FSymTab")={  \
          __fsym_##cmd##_name,    \
          (msg_hanlder_t *)&name, \
          __fsym_##cmd##_desc,    \
    };                            


#define MSH_CMD_EXPORT(command, desc)   \
    MSH_FUNCTION_EXPORT_CMD(command, command, desc)

/**
 * @ingroup msh
 *
 * This macro exports a command with alias to module shell.
 *
 * @param command is the name of the command.
 * @param alias is the alias of the command.
 * @param desc is the description of the command, which will show in help list.
 */
#define MSH_CMD_EXPORT_ALIAS(command, alias, desc)  \
    MSH_FUNCTION_EXPORT_CMD(command, alias, desc)
	
/**
 * @ingroup finsh
 *
 * This macro exports a system function to finsh shell.
 *
 * @param name the name of function.
 * @param desc the description of function, which will show in help.
 */
#define FINSH_FUNCTION_EXPORT(name, desc)	

/**
 * @ingroup finsh
 *
 * This macro exports a system function with an alias name to finsh shell.
 *
 * @param name the name of function.
 * @param alias the alias name of function.
 * @param desc the description of function, which will show in help.
 */
#define FINSH_FUNCTION_EXPORT_ALIAS(name, alias, desc)


