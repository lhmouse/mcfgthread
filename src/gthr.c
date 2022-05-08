/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_GTHR_EXTERN_INLINE
#include "gthr.h"

__MCF_DLLEXPORT
intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t arg)
  {
    __gthread_recursive_mutex_t* rmtx = (__gthread_recursive_mutex_t*) arg;

    /* Clear the depth counter and return it.  */
    intptr_t unlocked = rmtx->__depth;
    rmtx->__depth = 0;
    _MCF_atomic_store_32_rlx(&(rmtx->__owner), 0);
    _MCF_mutex_unlock(&(rmtx->__mutex));

    __MCFGTHREAD_ASSERT(unlocked > 0);
    return unlocked;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t arg, intptr_t unlocked)
  {
    __MCFGTHREAD_ASSERT(unlocked > 0);
    __gthread_recursive_mutex_t* rmtx = (__gthread_recursive_mutex_t*) arg;

    /* Relock the mutex and restore the depth counter.  */
    _MCF_mutex_lock(&(rmtx->__mutex), NULL);
    __MCFGTHREAD_ASSERT(rmtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(rmtx->__owner), (int32_t) _MCF_thread_self_tid());
    rmtx->__depth = (int32_t) unlocked;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_thread_thunk(_MCF_thread* thrd)
  {
    __MCF_gthr_thread_record* rec = (__MCF_gthr_thread_record*) thrd->__data;

    /* Invoke the user-defined procedure and save its result in the record.  */
    rec->__result = rec->__proc(rec->__arg);
  }
