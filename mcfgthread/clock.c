/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_CLOCK_IMPORT  __MCF_DLLEXPORT
#define __MCF_CLOCK_INLINE  __MCF_DLLEXPORT
#include "clock.h"
#include "xglobals.h"

static inline
uint64_t
do_divide_by_10000(uint64_t value)
  {
#ifdef __SIZEOF_INT128__
    return (uint64_t) ((unsigned __int128) value * 0x68DB8BAC710CBULL >> 64);
#else
    uint64_t temp = (uint32_t) value * 0xBAC710CBULL >> 32;
    uint64_t middle = (uint32_t) value * 0x68DB8ULL;
    temp = ((value >> 32) * 0xBAC710CBULL + (uint32_t) middle + temp) >> 32;
    return temp + (middle >> 32) + (value >> 32) * 0x68DB8ULL;
#endif
  }

__MCF_DLLEXPORT
int64_t
_MCF_utc_now(void)
  {
    ULARGE_INTEGER uli;

    /* `ULARGE_INTEGER` has a more strict alignment requirement than `FILETIME`,
     * so the cast should be safe.  */
    GetSystemTimeAsFileTime((FILETIME*) &uli);

    /* 11644473600 is number of seconds from 1601-01-01T00:00:00Z (the NT epoch)
     * to 1970-01-01T00:00:00Z (the Unix Epoch).  */
    return (int64_t) do_divide_by_10000(uli.QuadPart - 116444736000000000);
  }

__MCF_DLLEXPORT
double
_MCF_hires_utc_now(void)
  {
    __MCF_LAZY_DECLARE(GetSystemTimePreciseAsFileTime);
    ULARGE_INTEGER uli;

    /* This is available since Windows 8. `ULARGE_INTEGER` has a more strict
     * alignment requirement than `FILETIME`, so the cast should be safe.  */
    if(__MCF_LAZY_GET(GetSystemTimePreciseAsFileTime))
      __MCF_LAZY_DEREF(GetSystemTimePreciseAsFileTime) ((FILETIME*) &uli);
    else
      GetSystemTimeAsFileTime((FILETIME*) &uli);

    /* 11644473600 is number of seconds from 1601-01-01T00:00:00Z (the NT epoch)
     * to 1970-01-01T00:00:00Z (the Unix Epoch).  */
    return (double) ((int64_t) uli.QuadPart - 116444736000000000) * 0.0001;
  }

__MCF_DLLEXPORT
int64_t
_MCF_tick_count(void)
  {
    __MCF_LAZY_DECLARE(QueryInterruptTime);
    ULONGLONG ull;

    /* This is available since Windows Vista. Its result is calculated by
     * multiplying the count of ticks by the interval of each tick (15.625 ms),
     * hence the poor accuracy.  */
    if(!__MCF_LAZY_GET(QueryInterruptTime))
      return (int64_t) GetTickCount64();

    /* This is available since Windows 10.  */
    __MCF_LAZY_DEREF(QueryInterruptTime) (&ull);
    return (int64_t) do_divide_by_10000(ull);
  }

__MCF_DLLEXPORT
double
_MCF_perf_counter(void)
  {
    LARGE_INTEGER li;

    /* This will not fail since Windows XP.  */
    QueryPerformanceCounter(&li);
    return (double) li.QuadPart * __MCF_crt_pf_recip;
  }
