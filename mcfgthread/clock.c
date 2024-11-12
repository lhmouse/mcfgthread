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
    if(__MCF_G_HAS_LAZY(GetSystemTimePreciseAsFileTime)) {
      /* This is available since Windows 8.  */
      ULONGLONG ull;
      __MCF_G_LAZY(GetSystemTimePreciseAsFileTime) ((FILETIME*) &ull);
      return (double) ((int64_t) ull - 116444736000000000) * 0.0001;
    }

    /* `ULONGLONG` has a more strict alignment requirement than `FILETIME`,
     * so the cast should be safe. `116444736000000000` is number of 1/100
     * seconds from 1601-01-01T00:00:00Z (the NT epoch) to 1970-01-01T00:00:00Z
     * (the Unix Epoch).  */
    ULONGLONG ull;
    GetSystemTimeAsFileTime((FILETIME*) &ull);
    return (double) ((int64_t) ull - 116444736000000000) * 0.0001;
  }

__MCF_DLLEXPORT
int64_t
_MCF_tick_count(void)
  {
    if(__MCF_G_HAS_LAZY(QueryInterruptTime)) {
      /* This is available since Windows 10.  */
      ULONGLONG ull;
      __MCF_G_LAZY(QueryInterruptTime) (&ull);
      return (int64_t) do_divide_by_10000(ull);
    }

    /* This is available since Windows Vista and has a resolution of
     * 15.625 ms.  */
    return (int64_t) GetTickCount64();
  }

__MCF_DLLEXPORT
double
_MCF_perf_counter(void)
  {
    /* This will not fail since Windows XP.  */
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double) li.QuadPart * __MCF_crt_pf_recip;
  }
