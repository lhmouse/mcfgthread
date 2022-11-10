/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_C11_IMPORT  __MCF_DLLEXPORT
#define __MCF_C11_INLINE  __MCF_DLLEXPORT
#include "c11.h"
#include "clock.h"

__MCF_DLLEXPORT
void
__MCF_c11_thread_thunk_v2(_MCF_thread* thrd)
  {
    __MCF_c11_thread_record* rec = _MCF_thread_get_data(thrd);

    /* Invoke the user-defined procedure and save its result in the record.  */
    rec->__result = rec->__proc(rec->__arg);
  }

__MCF_DLLEXPORT
int
__MCF_c11_thrd_sleep(const __MCF_timespec* dur, __MCF_timespec* rem_opt)
  {
    double value = 0.0009999;
    value += (double) dur->tv_nsec * 0.000001;
    value += (double) dur->tv_sec * 1000;

    /* The C11 standard says this should be measured by the system clock.
     * Don't look at me. It was not I who wrote the standard.  */
    value += _MCF_hires_utc_now();

    /* Clamp the timestamp.  */
    value = __builtin_fmax(value, 0);
    value = __builtin_fmin(value, 0x1p63 - 0x1p10);

    int64_t timeout = (int64_t) value;
    int err = _MCF_sleep(&timeout);

    if(rem_opt) {
      *rem_opt = (__MCF_timespec) __MCF_0_INIT;

      if(err) {
        /* Calculate the remaining time if the operation was interrupted.  */
        double rem = __builtin_fmax(value - _MCF_hires_utc_now(), 0) * 0.001;
        rem_opt->tv_sec = (time_t) rem;
        rem_opt->tv_nsec = (long) ((rem - (double) rem_opt->tv_sec) * 1000000000);
      }
    }

    /* Return 0 in case of timeouts, and -1 in case of interrupts.  */
    return err;
  }
