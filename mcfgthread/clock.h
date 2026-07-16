/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_CLOCK_
#define __MCFGTHREAD_CLOCK_

#include "fwd.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_CLOCK_IMPORT
#  define __MCF_CLOCK_IMPORT
#  define __MCF_CLOCK_INLINE  __MCF_GNU_INLINE
#endif

/* Gets the system time.
 *
 * This function is fast, but may be less precise than `_MCF_hires_utc_now()`.
 * This value corresponds to `clock_gettime(CLOCK_REALTIME_COARSE, ...)` on
 * Linux.
 *
 * @returns the number of milliseconds since 1970-01-01T00:00:00Z.  */
__MCF_CLOCK_IMPORT
int64_t
_MCF_utc_now(void)
  __MCF_noexcept;

/* Gets a more precise value of the system time.
 *
 * This function is precise, but may be slower than `_MCF_utc_now()`. This
 * value corresponds to `clock_gettime(CLOCK_REALTIME, ...)` on Linux.
 *
 * @returns the number of milliseconds since 1970-01-01T00:00:00Z.  */
__MCF_CLOCK_IMPORT
double
_MCF_hires_utc_now(void)
  __MCF_noexcept;

/* Gets the number of milliseconds since system startup, excluding time when the
 * system is in suspension or hibernation.
 *
 * This function is fast, but may be less precise than `_MCF_hires_steady_now()`.
 * This value corresponds to `clock_gettime(CLOCK_MONOTONIC_COARSE, ...)` on
 * Linux.
 *
 * @returns the number of milliseconds of the steady clock.  */
__MCF_CLOCK_IMPORT
int64_t
_MCF_steady_now(void)
  __MCF_noexcept;

/* Gets the number of milliseconds since system startup, excluding time when the
 * system is in suspension or hibernation.
 *
 * This function is precise, but may be slower than `_MCF_steady_now()`. This
 * value corresponds to `clock_gettime(CLOCK_MONOTONIC, ...)` on Linux.
 *
 * @returns the number of milliseconds of the steady clock.  */
__MCF_CLOCK_IMPORT
double
_MCF_hires_steady_now(void)
  __MCF_noexcept;

/* Gets the number of milliseconds since system startup, including time when the
 * system is in suspension or hibernation.
 *
 * This function is fast, but may be less precise than `_MCF_hires_tick_count()`.
 * This value corresponds to `clock_gettime(CLOCK_BOOTTIME, ...)` on Linux.
 *
 * @returns the number of milliseconds of the steady clock.  */
__MCF_CLOCK_IMPORT
int64_t
_MCF_tick_count(void)
  __MCF_noexcept;

/* Gets the number of milliseconds since system startup, including time when the
 * system is in suspension or hibernation.
 *
 * This function is precise, but may be slower than `_MCF_tick_count()`. This
 * value corresponds to `clock_gettime(CLOCK_BOOTTIME, ...)` on Linux.
 *
 * @returns the number of milliseconds of the steady clock.  */
__MCF_CLOCK_IMPORT
double
_MCF_hires_tick_count(void)
  __MCF_noexcept;

/* Gets the value of the high-resolution performance counter as a number of
 * milliseconds.
 *
 * This is the traditional high-resolution clock for measurement, but it may
 * deviate from `_MCF_hires_tick_count()` by a few milliseconds.
 *
 * @returns the number of milliseconds of the performance counter.  */
__MCF_CLOCK_IMPORT
double
_MCF_perf_counter(void)
  __MCF_noexcept;

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_CLOCK_  */
