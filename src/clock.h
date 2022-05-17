/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_CLOCK_
#define __MCFGTHREAD_CLOCK_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_DECLSPEC_CLOCK
#  define __MCF_DECLSPEC_CLOCK(...)  __VA_ARGS__
#endif

/* Get the number of milliseconds since 1970-01-01T00:00:00Z.  */
__MCF_DECLSPEC_CLOCK()
int64_t
_MCF_utc_now(void) __MCF_NOEXCEPT;

__MCF_DECLSPEC_CLOCK()
double
_MCF_hires_utc_now(void) __MCF_NOEXCEPT;

/* Get the number of milliseconds since system startup.
 * This value is monotonic.  */
__MCF_DECLSPEC_CLOCK()
int64_t
_MCF_tick_count(void) __MCF_NOEXCEPT;

/* Get the value of the performance counter in milliseconds.
 * This value is monotonic.  */
__MCF_DECLSPEC_CLOCK()
double
_MCF_perf_counter(void) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_CLOCK_  */
