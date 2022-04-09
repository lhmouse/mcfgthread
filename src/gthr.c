// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_GTHR_C_  1
#include "gthr.h"

intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t arg)
  {
    __gthread_mutex_t* mtx = (__gthread_mutex_t*) arg;

    // Unlock the mutex. The return value is unused.
    _MCF_mutex_unlock(mtx);
    return 0;
  }

void
__MCF_gthr_mutex_relock_callback(intptr_t arg, intptr_t unlocked)
  {
    __gthread_mutex_t* mtx = (__gthread_mutex_t*) arg;

    // Relock the mutex. The `unlocked` value is unused.
    (void) unlocked;
    _MCF_mutex_lock(mtx, NULL);
  }

intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t arg)
  {
    __gthread_recursive_mutex_t* rmtx = (__gthread_recursive_mutex_t*) arg;

    // Clear the depth counter and return it.
    intptr_t unlocked = rmtx->__depth;
    __atomic_store_n(&(rmtx->__owner), 0, __ATOMIC_RELAXED);
    _MCF_mutex_unlock(&(rmtx->__mutex));

    __MCFGTHREAD_ASSERT(unlocked > 0);
    return unlocked;
  }

void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t arg, intptr_t unlocked)
  {
    __MCFGTHREAD_ASSERT(unlocked > 0);
    __gthread_recursive_mutex_t* rmtx = (__gthread_recursive_mutex_t*) arg;

    // Relock the mutex and restore the depth counter.
    _MCF_mutex_lock(&(rmtx->__mutex), NULL);
    __atomic_store_n(&(rmtx->__owner), _MCF_thread_self_tid(), __ATOMIC_RELAXED);
    rmtx->__depth = (int32_t) unlocked;
  }

void
__MCF_gthr_thread_thunk(_MCF_thread* thrd)
  {
    __MCF_gthr_thread_record* rec = (__MCF_gthr_thread_record*) thrd->__data;

    // Invoke the user-defined procedure and save its result in the record.
    rec->__result = rec->__proc(rec->__arg);
  }
