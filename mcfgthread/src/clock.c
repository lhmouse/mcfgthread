/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_CLOCK_IMPORT  __MCF_DLLEXPORT
#define __MCF_CLOCK_INLINE  __MCF_DLLEXPORT
#include "../clock.h"
#include "xglobals.h"

static inline
uint64_t
do_divide_by_10000(uint64_t value)
  {
#if defined __SIZEOF_INT128__
    return (uint64_t) ((unsigned __int128) value * 0x68DB8BAC710CCULL >> 64);
#elif defined _MSC_VER && __MCF_64_32(1, 0)
    return __umulh(value, 0x68DB8BAC710CCULL);
#else
    uint64_t x0 = (uint32_t) value * 0xBAC710CCULL >> 32;
    uint64_t x1 = (uint32_t) value * 0x68DB8ULL;
    uint64_t x2 = ((value >> 32) * 0xBAC710CCULL + (uint32_t) x1 + x0) >> 32;
    return (value >> 32) * 0x68DB8ULL + x2 + (x1 >> 32);
#endif
  }

__MCF_DLLEXPORT
int64_t
_MCF_utc_now(void)
  {
    /* `ULONGLONG` has a more strict alignment requirement than `FILETIME`,
     * so the cast should be safe. `116444736000000000` is number of 1/100
     * seconds from 1601-01-01T00:00:00Z (the NT epoch) to 1970-01-01T00:00:00Z
     * (the Unix Epoch).  */
    ULONGLONG ull;
    GetSystemTimeAsFileTime((FILETIME*) &ull);
    return (int64_t) do_divide_by_10000(ull - 116444736000000000);
  }

__MCF_DLLEXPORT
double
_MCF_hires_utc_now(void)
  {
    /* `ULONGLONG` has a more strict alignment requirement than `FILETIME`,
     * so the cast should be safe. `116444736000000000` is number of 1/100
     * seconds from 1601-01-01T00:00:00Z (the NT epoch) to 1970-01-01T00:00:00Z
     * (the Unix Epoch).  */
    ULONGLONG ull;
    __MCF_crt_GetSystemTimePreciseAsFileTime((FILETIME*) &ull);
    return (double)(int64_t) (ull - 116444736000000000) * 0.0001;
  }

__MCF_DLLEXPORT
int64_t
_MCF_steady_now(void)
  {
    ULONGLONG ull;
    QueryUnbiasedInterruptTime(&ull);
    return (int64_t) do_divide_by_10000(ull);
  }

__MCF_DLLEXPORT
double
_MCF_hires_steady_now(void)
  {
    ULONGLONG ull;
    __MCF_crt_QueryUnbiasedInterruptTimePrecise(&ull);
    return (double)(int64_t) ull * 0.0001;
  }

__MCF_DLLEXPORT
int64_t
_MCF_tick_count(void)
  {
    ULONGLONG ull;
    ull = __MCF_get_interrupt_time();
    return (int64_t) do_divide_by_10000(ull);
  }

__MCF_DLLEXPORT
double
_MCF_hires_tick_count(void)
  {
    ULONGLONG ull;
    __MCF_crt_QueryInterruptTimePrecise(&ull);
    return (double)(int64_t) ull * 0.0001;
  }

__MCF_DLLEXPORT
double
_MCF_perf_counter(void)
  {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double) li.QuadPart * __MCF_crt_perf_freq_reciprocal;
  }
