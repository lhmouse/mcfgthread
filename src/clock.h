// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_CLOCK_H_
#define __MCFGTHREAD_CLOCK_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCFGTHREAD_CLOCK_C_
#  define __MCFGTHREAD_CLOCK_INLINE  __MCF_GNU_INLINE
#else
#  define __MCFGTHREAD_CLOCK_INLINE
#endif

// Get the number of milliseconds since 1970-01-01T00:00:00Z.
int64_t
_MCF_utc_now(void) __MCF_NOEXCEPT;

// Get the number of milliseconds since system startup.
// This value is monotonic.
int64_t
_MCF_tick_count(void) __MCF_NOEXCEPT;

// Get the value of the performance counter in milliseconds.
// This value is monotonic.
double
_MCF_perf_counter(void) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_CLOCK_H_
