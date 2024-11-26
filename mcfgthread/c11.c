/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_C11_IMPORT  __MCF_DLLEXPORT
#define __MCF_C11_INLINE  __MCF_DLLEXPORT
#define __MCF_C11_NO_ALIASES  1
#include "c11.h"
#include "clock.h"

__MCF_DLLEXPORT
int
__MCF_c11_thrd_sleep(const __MCF_timespec* dur, __MCF_timespec* rem_opt)
  {
    double end_time = 0.9999999;
    end_time += (double) dur->tv_nsec * 0.000001;
    end_time += (double) dur->tv_sec * 1000;

    /* The C11 standard says this should be measured by the system clock.
     * Don't look at me. It was not I who wrote the standard.  */
    end_time += _MCF_hires_utc_now();

    /* Clamp the timestamp.  */
    end_time = __builtin_fmax(end_time, 0);
    end_time = __builtin_fmin(end_time, 0x1p63 - 0x1p10);

    if(rem_opt)
      *rem_opt = (__MCF_timespec) __MCF_0_INIT;

    int64_t timeout = (int64_t) end_time;
    int err = _MCF_sleep(&timeout);

    if(rem_opt && err) {
      /* Calculate the remaining time if the operation was interrupted.  */
      double rem = __builtin_fmax(end_time - _MCF_hires_utc_now(), 0) * 0.001;
      rem_opt->tv_sec = (time_t) rem;
      rem_opt->tv_nsec = (long) ((rem - (double) rem_opt->tv_sec) * 1000000000);
    }

    /* Return 0 in case of timeouts, and -1 in case of interrupts.  */
    return err;
  }

__MCF_DLLEXPORT
int
__MCF_c11__thrd_sleep_until(const __MCF_timespec* ts)
  {
    double end_time = 0.9999999;
    end_time += (double) ts->tv_nsec * 0.000001;
    end_time += (double) ts->tv_sec * 1000;

    /* Clamp the timestamp.  */
    end_time = __builtin_fmax(end_time, 0);
    end_time = __builtin_fmin(end_time, 0x1p63 - 0x1p10);

    int64_t timeout = (int64_t) end_time;
    int err = _MCF_sleep(&timeout);

    /* Return 0 in case of timeouts, and -1 in case of interrupts.  */
    return err;
  }
