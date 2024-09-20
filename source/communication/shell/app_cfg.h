//! \note do not move this pre-processor statement to other places
#include "..\app_cfg.h"

#ifndef __WL_SHELL_CFG_H__
#define __WL_SHELL_CFG_H__

/*============================ INCLUDES ======================================*/
#include <stdio.h>
/*============================ MACROS ========================================*/
#ifndef SHELL_PRINTF
#define SHELL_PRINTF                   printf
#endif

#ifndef SHELL_DEBUG
#define SHELL_DEBUG                    0
#endif

#ifdef  log_raw
#undef  log_raw
#endif
#define log_raw(...)                   SHELL_PRINTF(__VA_ARGS__);               


#if SHELL_DEBUG
#ifdef assert
#undef assert
#endif
#define assert(EXPR)                                                           \
if (!(EXPR))                                                                   \
{                                                                              \
    FAL_PRINTF("(%s) has assert failed at %s.\n", #EXPR, __FUNCTION__);        \
    while (1);                                                                 \
}
/* debug level log */
#ifdef  log_d
#undef  log_d
#endif
#define log_d(...)                     SHELL_PRINTF("[D/FAL] (%s:%d) ", __FUNCTION__, __LINE__);           SHELL_PRINTF(__VA_ARGS__);FAL_PRINTF("\n")

#else

#ifdef assert
#undef assert
#endif
#define assert(EXPR)                   ((void)0);

/* debug level log */
#ifdef  log_d
#undef  log_d
#endif
#define log_d(...)
#endif 
/* error level log */
#ifdef  log_e
#undef  log_e
#endif
#define log_e(...)                     SHELL_PRINTF("\033[31;22m[E/FAL] (%s:%d) ", __FUNCTION__, __LINE__);SHELL_PRINTF(__VA_ARGS__);SHELL_PRINTF("\033[0m\n")

/* info level log */
#ifdef  log_i
#undef  log_i
#endif
#define log_i(...)                     SHELL_PRINTF("\033[32;22m[I/FAL] ");                                SHELL_PRINTF(__VA_ARGS__);SHELL_PRINTF("\033[0m\n")

/*============================ MACROFIED FUNCTIONS ===========================*/

/*============================ TYPES =========================================*/

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ LOCAL VARIABLES ===============================*/

/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */

