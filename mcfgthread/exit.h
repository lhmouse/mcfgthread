/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_EXIT_
#define __MCFGTHREAD_EXIT_

#include "fwd.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_EXIT_IMPORT
#  define __MCF_EXIT_IMPORT
#  define __MCF_EXIT_INLINE  __MCF_GNU_INLINE
#endif

/* Declare 'real' functions here.  */
__MCF_EXIT_IMPORT
void
__MCF__Exit(int __status) __MCF_NOEXCEPT __attribute__((__noreturn__));

__MCF_EXIT_IMPORT
void
__MCF_quick_exit(int __status) __MCF_NOEXCEPT __attribute__((__noreturn__));

__MCF_EXIT_IMPORT
void
__MCF_exit(int __status) __MCF_NOEXCEPT __attribute__((__noreturn__));

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_EXIT_  */
