/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_GTHR_AUX_IMPORT  __MCF_DLLEXPORT
#define __MCF_GTHR_AUX_INLINE  __MCF_DLLEXPORT
#include "../gthr_aux.h"
#include "xglobals.h"

__MCF_DLLEXPORT __MCF_FN_PURE
int64_t
__MCF_gthr_timeout_from_timespec(const __MCF_timespec* abs_time)
  {
    double value = 0.9999999;
    value += (double) abs_time->tv_nsec * 0.000001;
    value += (double) abs_time->tv_sec * 1000;

    /* Clamp the timestamp.  */
    value = __builtin_fmax(value, 0);
    value = __builtin_fmin(value, 0x1p63 - 0x1p10);
    return (int64_t) value;
  }

__MCF_DLLEXPORT
intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t arg)
  {
    _MCF_mutex* mtx = (_MCF_mutex*) arg;
    _MCF_mutex_unlock_slow(mtx);
    return 0;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_mutex_relock_callback(intptr_t arg, intptr_t unlocked)
  {
    _MCF_mutex* mtx = (_MCF_mutex*) arg;
    __MCF_ASSERT(unlocked == 0);
    _MCF_mutex_lock_slow(mtx, nullptr);
  }

__MCF_DLLEXPORT
int
__MCF_gthr_cond_mutex_wait(_MCF_cond* cnd, _MCF_mutex* mtx, const int64_t* timeout_opt)
  {
    return _MCF_cond_wait(cnd, __MCF_gthr_mutex_unlock_callback,
                          __MCF_gthr_mutex_relock_callback, (intptr_t) mtx,
                          timeout_opt);
  }

__MCF_DLLEXPORT
intptr_t
__MCF_gthr_shared_mutex_unlock_callback(intptr_t arg)
  {
    _MCF_shared_mutex* smtx = (_MCF_shared_mutex*) arg;
    _MCF_shared_mutex_unlock_slow(smtx);
    return 0;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_shared_mutex_relock_shared_callback(intptr_t arg, intptr_t unlocked)
  {
    _MCF_shared_mutex* smtx = (_MCF_shared_mutex*) arg;
    __MCF_ASSERT(unlocked == 0);
    _MCF_shared_mutex_lock_shared_slow(smtx, nullptr);
  }

__MCF_DLLEXPORT
void
__MCF_gthr_shared_mutex_relock_exclusive_callback(intptr_t arg, intptr_t unlocked)
  {
    _MCF_shared_mutex* smtx = (_MCF_shared_mutex*) arg;
    __MCF_ASSERT(unlocked == 0);
    _MCF_shared_mutex_lock_exclusive_slow(smtx, nullptr);
  }

__MCF_DLLEXPORT
int
__MCF_gthr_cond_shared_mutex_wait_shared(_MCF_cond* cnd, _MCF_shared_mutex* smtx,
                                         const int64_t* timeout_opt)
  {
    return _MCF_cond_wait(cnd, __MCF_gthr_shared_mutex_unlock_callback,
                          __MCF_gthr_shared_mutex_relock_shared_callback,
                          (intptr_t) smtx, timeout_opt);
  }

__MCF_DLLEXPORT
int
__MCF_gthr_cond_shared_mutex_wait_exclusive(_MCF_cond* cnd, _MCF_shared_mutex* smtx,
                                            const int64_t* timeout_opt)
  {
    return _MCF_cond_wait(cnd, __MCF_gthr_shared_mutex_unlock_callback,
                          __MCF_gthr_shared_mutex_relock_exclusive_callback,
                          (intptr_t) smtx, timeout_opt);
  }

__MCF_DLLEXPORT
intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t arg)
  {
    __MCF_gthr_rc_mutex* rmtx = (__MCF_gthr_rc_mutex*) arg;
    intptr_t unlocked = rmtx->__depth;
    __MCF_ASSERT(unlocked > 0);
    rmtx->__depth = 0;
    _MCF_atomic_store_32_rlx(rmtx->__owner, 0);
    _MCF_mutex_unlock_slow(rmtx->__mutex);
    return unlocked;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t arg, intptr_t unlocked)
  {
    __MCF_gthr_rc_mutex* rmtx = (__MCF_gthr_rc_mutex*) arg;
    _MCF_mutex_lock_slow(rmtx->__mutex, nullptr);
    __MCF_ASSERT(rmtx->__owner[0] == 0);
    _MCF_atomic_store_32_rlx(rmtx->__owner, __MCF_tid());
    __MCF_ASSERT(unlocked > 0);
    rmtx->__depth = (int) unlocked;
  }

__MCF_DLLEXPORT
int
__MCF_gthr_cond_recursive_mutex_wait(_MCF_cond* cnd, __MCF_gthr_rc_mutex* rmtx,
                                     const int64_t* timeout_opt)
  {
    return _MCF_cond_wait(cnd, __MCF_gthr_recursive_mutex_unlock_callback,
                          __MCF_gthr_recursive_mutex_relock_callback,
                          (intptr_t) rmtx, timeout_opt);
  }

static __MCF_REALIGN_SP
void
do_gthread_routine(_MCF_thread* thrd)
  {
    __MCF_gthr_thread_record* rec = _MCF_thread_get_data(thrd);
    rec->__arg_or_result = (* rec->__proc) (rec->__arg_or_result);
  }

static inline
__MCF_gthr_thread_record*
do_get_record(_MCF_thread* thrd)
  {
    __MCF_gthr_thread_record* rec = _MCF_thread_get_data(thrd);
    if(!rec)
      return nullptr;

    /* Check the GUID. As user-defined data are aligned to 16-byte boundaries,
     * there must be at least 16 bytes available.  */
    if(!__MCF_mequal(rec->__magic_guid, __MCF_crt_gthread_guid, 16))
      return nullptr;

    /* Assume so. `do_gthread_routine()` is not shared across modules,
     * so we should not check it for uniqueness.  */
    return rec;
  }

__MCF_DLLEXPORT
_MCF_thread*
__MCF_gthr_thread_create_v3(__MCF_gthr_thread_procedure* proc, void* arg)
  {
    __MCF_ALIGNED(8) __MCF_gthr_thread_record record;
    __builtin_memcpy(record.__magic_guid, __MCF_crt_gthread_guid, 16);
    record.__proc = proc;
    record.__arg_or_result = arg;
    return _MCF_thread_new(do_gthread_routine, &record, sizeof(record));
  }

__MCF_DLLEXPORT
void
__MCF_gthr_thread_join_v3(_MCF_thread* thrd, void** resp_opt)
  {
    /* Wait for the thread to terminate.  */
    __MCF_ASSERT(thrd->__tid != __MCF_tid());
    _MCF_thread_wait(thrd, nullptr);

    if(resp_opt) {
      *resp_opt = nullptr;

      /* Get the exit code.  */
      __MCF_gthr_thread_record* rec = do_get_record(thrd);
      if(rec)
        *resp_opt = rec->__arg_or_result;
    }

    /* Free the thread.  */
    _MCF_thread_drop_ref(thrd);
  }

__MCF_DLLEXPORT
void
__MCF_gthr_thread_exit_v3(void* resp)
  {
    /* Set the exit code.  */
    __MCF_gthr_thread_record* rec = do_get_record(_MCF_thread_self());
    if(rec)
      rec->__arg_or_result = resp;

    /* Terminate the current thread.  */
    _MCF_thread_exit();
  }
