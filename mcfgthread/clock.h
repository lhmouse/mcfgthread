/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_CLOCK_
#define __MCFGTHREAD_CLOCK_

#include "fwd.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_CLOCK_IMPORT
#  define __MCF_CLOCK_IMPORT
#  define __MCF_CLOCK_INLINE  __MCF_GNU_INLINE
#endif

/* Get the number of milliseconds since 1970-01-01T00:00:00Z.  */
__MCF_CLOCK_IMPORT
int64_t
_MCF_utc_now(void) __MCF_noexcept;

__MCF_CLOCK_IMPORT
double
_MCF_hires_utc_now(void) __MCF_noexcept;

/* Get the number of milliseconds since system startup.
 * This value is monotonic.  */
__MCF_CLOCK_IMPORT
int64_t
_MCF_tick_count(void) __MCF_noexcept;

/* Get the value of the performance counter in milliseconds.
 * This value is monotonic.  */
__MCF_CLOCK_IMPORT
double
_MCF_perf_counter(void) __MCF_noexcept;

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_CLOCK_  */
