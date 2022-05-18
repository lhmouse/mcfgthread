/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_GTHR(...)  __MCF_DLLEXPORT
#include "gthr.h"

intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t arg)
  {
    __gthread_recursive_mutex_t* rmtx = (__gthread_recursive_mutex_t*) arg;

    /* Clear the depth counter and return it.  */
    intptr_t unlocked = rmtx->__depth;
    rmtx->__depth = 0;
    _MCF_atomic_store_32_rlx(&(rmtx->__owner), 0);
    _MCF_mutex_unlock(&(rmtx->__mutex));

    __MCF_ASSERT(unlocked > 0);
    return unlocked;
  }

void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t arg, intptr_t unlocked)
  {
    __MCF_ASSERT(unlocked > 0);
    __gthread_recursive_mutex_t* rmtx = (__gthread_recursive_mutex_t*) arg;

    /* Relock the mutex and restore the depth counter.  */
    _MCF_mutex_lock(&(rmtx->__mutex), NULL);
    __MCF_ASSERT(rmtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(rmtx->__owner), (int32_t) _MCF_thread_self_tid());
    rmtx->__depth = (int32_t) unlocked;
  }

void
__MCF_gthr_thread_thunk(_MCF_thread* thrd)
  {
    __MCF_gthr_thread_record* rec = _MCF_thread_get_data(thrd);

    /* Invoke the user-defined procedure and save its result in the record.  */
    rec->__result = rec->__proc(rec->__arg);
  }
