/**
 * 此文件用于关闭 semihosting
 */
//! \note for IAR
#ifdef __IS_COMPILER_IAR__
#   undef __IS_COMPILER_IAR__
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__                  1
#endif

//! \note for arm compiler 5
#ifdef __IS_COMPILER_ARM_COMPILER_5__
#   undef __IS_COMPILER_ARM_COMPILER_5__
#endif
#if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#   define __IS_COMPILER_ARM_COMPILER_5__       1
#endif
//! @}

//! \note for arm compiler 6
#ifdef __IS_COMPILER_ARM_COMPILER_6__
#   undef __IS_COMPILER_ARM_COMPILER_6__
#endif
#if ((__ARMCC_VERSION >= 6000000) && (__ARMCC_VERSION < 7000000))
#   define __IS_COMPILER_ARM_COMPILER_6__       1
#endif

#ifdef __IS_COMPILER_ARM_COMPILER__
#   undef __IS_COMPILER_ARM_COMPILER__
#endif
#if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__   \
||  defined(__IS_COMPILER_ARM_COMPILER_6__) && __IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_ARM_COMPILER__         1
#endif

#ifdef __IS_COMPILER_LLVM__
#   undef  __IS_COMPILER_LLVM__
#endif
#if defined(__clang__) && !__IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_LLVM__                 1
#else
//! \note for gcc
#ifdef __IS_COMPILER_GCC__
#   undef __IS_COMPILER_GCC__
#endif
#if defined(__GNUC__) && !(__IS_COMPILER_ARM_COMPILER_6__ || __IS_COMPILER_LLVM__)
#   define __IS_COMPILER_GCC__                  1
#endif
//! @}
#endif
//! @}


/* 告诉编译器若没有使用 MicroLIB ，则 main() 函数不需要入口参数 */
#if __IS_COMPILER_ARM_COMPILER_6__
    #ifndef __MICROLIB
    __asm(".global __ARM_use_no_argv\n\t");
    #endif
#endif

/* 关闭 semihosting */
#if __IS_COMPILER_ARM_COMPILER_6__
__asm(".global __use_no_semihosting");
    
/* AC6 会因为关闭 semihosting 缺这个函数，所以要补上 */
void _sys_exit(int ret)
{
    (void)ret;
    while(1) {}
}
#elif __IS_COMPILER_ARM_COMPILER_5__
#pragma import(__use_no_semihosting)
#endif

/* AC5 和 AC6 都会因为关闭 semihosting 缺这个函数，所以要补上 */
#if __IS_COMPILER_ARM_COMPILER__
void _ttywrch(int ch)
{
    (void)ch;
}
#endif

/* 当使用 AC6 开启 MicroLIB 时，若有使用 assert() 的需求，需要自己实现 __aeabi_assert() */
#if __IS_COMPILER_ARM_COMPILER_6__ && defined(__MICROLIB)
#include "RTE_Components.h "
#if (!defined(RTE_CMSIS_Compiler_STDOUT) && !defined(RTE_Compiler_IO_STDOUT))
void __aeabi_assert(const char *chCond, const char *chLine, int wErrCode) 
{
    (void)chCond;
    (void)chLine;
    (void)wErrCode;
    
    for (;;);
}
#endif
#endif


/* 当用户没有使用printf时，通过添加如下的代码来让编译器避免报错 */
#if __IS_COMPILER_ARM_COMPILER_6__
#include "RTE_Components.h "
#   ifdef __MICROLIB
#include <stdio.h>
#if (!defined(RTE_CMSIS_Compiler_STDOUT) && !defined(RTE_Compiler_IO_STDOUT))
int fputc(int ch, FILE *f)
{
    (void) f;
    (void) ch;
    
    return ch;
}
#endif
#   else 
#include <rt_sys.h>
#if (!defined(RTE_CMSIS_Compiler_STDOUT) && !defined(RTE_Compiler_IO_STDOUT))
FILEHANDLE $Sub$$_sys_open(const char *name, int openmode)
{
    (void) name;
    (void) openmode;
    return 0;
}
#endif
#   endif

#endif

