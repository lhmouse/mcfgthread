/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_C11(...)  __MCF_DLLEXPORT
#include "c11.h"
#include "clock.h"

__MCF_DLLEXPORT
intptr_t
__MCF_c11_mutex_unlock_callback(intptr_t arg)
  {
    mtx_t* mtx = (mtx_t*) arg;

    /* Clear the depth counter and return it.  */
    intptr_t unlocked = mtx->__depth;
    mtx->__depth = 0;
    _MCF_atomic_store_32_rlx(&(mtx->__owner), 0);
    _MCF_mutex_unlock(&(mtx->__mutex));

    __MCF_ASSERT(unlocked > 0);
    return unlocked;
  }

__MCF_DLLEXPORT
void
__MCF_c11_mutex_relock_callback(intptr_t arg, intptr_t unlocked)
  {
    __MCF_ASSERT(unlocked > 0);
    mtx_t* mtx = (mtx_t*) arg;

    /* Relock the mutex and restore the depth counter.  */
    _MCF_mutex_lock(&(mtx->__mutex), NULL);
    __MCF_ASSERT(mtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(mtx->__owner), (int32_t) _MCF_thread_self_tid());
    mtx->__depth = (int32_t) unlocked;
  }

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
__MCF_c11_thrd_sleep(const struct timespec* dur, struct timespec* rem_opt)
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
    _MCF_sleep(&timeout);

    /* Windows does not support signals, so this function never fails.  */
    if(rem_opt)
      *rem_opt = (struct timespec) __MCF_0_INIT;

    /* The C11 standard doesn't say we should return `thrd_success.  */
    return 0;
  }
