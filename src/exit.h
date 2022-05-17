/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_EXIT_
#define __MCFGTHREAD_EXIT_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_DECLSPEC_EXIT
#  define __MCF_DECLSPEC_EXIT(...)  __VA_ARGS__
#endif

/* Declare 'real' functions here.  */
__MCF_DECLSPEC_EXIT()
void
__MCF__Exit(int __status) __MCF_NOEXCEPT
  __attribute__((__noreturn__));

__MCF_DECLSPEC_EXIT()
void
__MCF_quick_exit(int __status) __MCF_NOEXCEPT
  __attribute__((__noreturn__));

__MCF_DECLSPEC_EXIT()
void
__MCF_exit(int __status) __MCF_NOEXCEPT
  __attribute__((__noreturn__));

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_EXIT_  */
