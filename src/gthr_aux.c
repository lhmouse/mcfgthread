/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_GTHR_AUX(...)  __MCF_DLLEXPORT
#include "gthr_aux.h"
#include "mutex.h"

__MCF_DLLEXPORT
int64_t
__MCF_gthr_timeout_from_timespec(const struct timespec* abs_time)
  {
    double value = (double) abs_time->tv_nsec * 0.000001 + (double) abs_time->tv_sec * 1000;
    if(value <= 0)
      return 0;

    if(value >= 0x7FFFFFFFFFFFFC00)
      return __INT64_MAX__;

    return (int64_t) (value + 0.0009999);
  }

__MCF_DLLEXPORT
intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t arg)
  {
    _MCF_mutex* mtx = (_MCF_mutex*) arg;

    /* Unlock the mutex. The return value is unused.  */
    _MCF_mutex_unlock(mtx);
    return 0;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_mutex_relock_callback(intptr_t arg, intptr_t unlocked)
  {
    _MCF_mutex* mtx = (_MCF_mutex*) arg;

    /* Relock the mutex. The `unlocked` value is unused.  */
    (void) unlocked;
    _MCF_mutex_lock(mtx, NULL);
  }
