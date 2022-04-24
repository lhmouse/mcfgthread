/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_GTHR_AUX_H_
#define __MCFGTHREAD_GTHR_AUX_H_

#include "fwd.h"
#include "once.h"
#include <time.h>  /* struct timespec  */

#ifdef __SSE2__
#  include <emmintrin.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_GTHR_AUX_EXTERN_INLINE
#  define __MCF_GTHR_AUX_EXTERN_INLINE  __MCF_GNU_INLINE
#endif

/* This is an auxiliary function for exception handling in `__gthread_once()`.
 * Ideally, if the target function throws exception we would like to allow
 * attempts to retry. Sadly this is not possible in standard C.  */
void
__MCF_gthr_unonce(_MCF_once** __oncep) __MCF_NOEXCEPT;

__MCF_GTHR_AUX_EXTERN_INLINE
void
__MCF_gthr_unonce(_MCF_once** __oncep) __MCF_NOEXCEPT
  {
    if(*__oncep)
      _MCF_once_abort(*__oncep);
  }

/* This is an auxiliary function for converting a `struct timespec` to the
 * number of milliseconds since the Unix epoch, with boundary checking.  */
int64_t
__MCF_gthr_timeout_from_timespec(const struct timespec* __abs_time) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_GTHR_AUX_EXTERN_INLINE
int64_t
__MCF_gthr_timeout_from_timespec(const struct timespec* __abs_time) __MCF_NOEXCEPT
  {
    double __time_ms = 0.000999;
    __time_ms += (double) __abs_time->tv_nsec * 0.000001;
    __time_ms += (double) __abs_time->tv_sec * 1000;

#ifdef __SSE2__
    /* On x86-64 this both results in smaller code and runs faster.  */
    __m128d __time_d = _mm_set_sd(__time_ms);
    __time_d = _mm_max_pd(__time_d, _mm_setzero_pd());
    __time_d = _mm_min_pd(__time_d, _mm_set_sd(0x7FFFFFFFFFFFFC00));
    return _mm_cvttsd_si64(__time_d);
#else
    /* This is the portable but slower way to do it.  */
    if(__time_ms <= 0)
      return 0;
    else if(__time_ms <= 0x7FFFFFFFFFFFFC00)
      return (int64_t) __time_ms;
    else
      return 0x7FFFFFFFFFFFFFFF;
#endif
  }

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a plain `_MCF_mutex`.  */
intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

void
__MCF_gthr_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_GTHR_AUX_H_  */
