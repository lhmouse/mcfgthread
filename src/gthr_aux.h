/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_GTHR_AUX_
#define __MCFGTHREAD_GTHR_AUX_

#include "fwd.h"
#include "once.h"
#include <time.h>  /* struct timespec  */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_DECLSPEC_GTHR_AUX
#  define __MCF_DECLSPEC_GTHR_AUX(...)  __VA_ARGS__
#endif

/* This is an auxiliary function for exception handling in `__gthread_once()`.
 * Ideally, if the target function throws exception we would like to allow
 * attempts to retry. Sadly this is not possible in standard C.  */
__MCF_DECLSPEC_GTHR_AUX(__MCF_GNU_INLINE)
void
__MCF_gthr_unonce(_MCF_once** __oncep) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR_AUX(__MCF_GNU_INLINE)
void
__MCF_gthr_unonce(_MCF_once** __oncep) __MCF_NOEXCEPT
  {
    if(*__oncep)
      _MCF_once_abort(*__oncep);
  }

/* This is an auxiliary function for converting a `struct timespec` to the
 * number of milliseconds since the Unix epoch, with boundary checking.  */
__MCF_DECLSPEC_GTHR_AUX()
int64_t
__MCF_gthr_timeout_from_timespec(const struct timespec* __abs_time) __MCF_NOEXCEPT
  __attribute__((__pure__));

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a plain `_MCF_mutex`.  */
__MCF_DECLSPEC_GTHR_AUX()
intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR_AUX()
void
__MCF_gthr_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_GTHR_AUX_  */
