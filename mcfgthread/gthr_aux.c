/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_GTHR_AUX_IMPORT  __MCF_DLLEXPORT
#define __MCF_GTHR_AUX_INLINE  __MCF_DLLEXPORT
#include "gthr_aux.h"
#include "xglobals.h"

static
EXCEPTION_DISPOSITION
do_call_once_seh_unwind(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx);

__MCF_DLLEXPORT
void
__MCF_gthr_call_once_seh_take_over(_MCF_once* once, __MCF_cxa_dtor_any_ init_proc, void* arg)
  {
#ifdef __MCF_M_X8632
    __MCF_USING_SEH_HANDLER(do_call_once_seh_unwind, (DWORD) once);
#  define __MCF_UNWINDING_ONCE_PTR(frm, disp)  ((DWORD**) (frm))[2]
#else
    __MCF_USING_SEH_HANDLER(do_call_once_seh_unwind);
#  if defined __MCF_M_X8664_ASM
    register _MCF_once* saved_once __asm__("rsi") = once;
#  elif defined __MCF_M_ARM64_ASM
    register _MCF_once* saved_once __asm__("x25") = once;
#  endif
    __asm__ volatile ("" : "+r"(saved_once));
#  if defined __MCF_M_X8664
#    define __MCF_UNWINDING_ONCE_PTR(frm, disp)  ((DISPATCHER_CONTEXT*) (disp))->ContextRecord->Rsi
#  elif defined __MCF_M_ARM64
#    define __MCF_UNWINDING_ONCE_PTR(frm, disp)  ((DISPATCHER_CONTEXT*) (disp))->ContextRecord->X25
#  endif
#endif

    /* Do initialization. This is the normal path.  */
    __MCF_invoke_cxa_dtor(init_proc, arg);

    /* Disarm the once flag with a tail call.  */
    _MCF_once_release(once);
  }

static
EXCEPTION_DISPOSITION
do_call_once_seh_unwind(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx)
  {
    (void) estab_frame;
    (void) ctx;
    (void) disp_ctx;

    /* If the stack is being unwound, reset the once flag.  */
    if(rec->ExceptionFlags & EXCEPTION_UNWIND)
      _MCF_once_abort((_MCF_once*) __MCF_UNWINDING_ONCE_PTR(estab_frame, disp_ctx));

    /* Continue unwinding.  */
    return ExceptionContinueSearch;
  }

#ifdef __MCF_M_X8632
/* Register the unwind handler. In the DLL we build a handler table by hand
 * which works on all compilers. In the static library we use the `.safeseh`
 * directive but it is only supported by Microsoft LINK, or LLD in LINK mode.  */
#  if !defined __MCF_IN_DLL && defined _MSC_VER
__asm__ (".safeseh _do_call_once_seh_unwind");
#  elif defined __MCF_IN_DLL
__asm__ (".globl ___MCF_gthr_do_call_once_seh_unwind\n"
         ".equiv ___MCF_gthr_do_call_once_seh_unwind, _do_call_once_seh_unwind");
#  endif
#endif

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
    _MCF_mutex_lock(mtx, __MCF_nullptr);
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

    /* Unlock the shared mutex. The return value is unused.  */
    _MCF_shared_mutex_unlock(smtx);
    return 0;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_shared_mutex_relock_shared_callback(intptr_t arg, intptr_t unlocked)
  {
    _MCF_shared_mutex* smtx = (_MCF_shared_mutex*) arg;

    /* Relock the shared mutex. The `unlocked` value is unused.  */
    (void) unlocked;
    _MCF_shared_mutex_lock_shared(smtx, __MCF_nullptr);
  }

__MCF_DLLEXPORT
void
__MCF_gthr_shared_mutex_relock_exclusive_callback(intptr_t arg, intptr_t unlocked)
  {
    _MCF_shared_mutex* smtx = (_MCF_shared_mutex*) arg;

    /* Relock the shared mutex. The `unlocked` value is unused.  */
    (void) unlocked;
    _MCF_shared_mutex_lock_exclusive(smtx, __MCF_nullptr);
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

    /* Clear the depth counter and return it.  */
    intptr_t unlocked = (intptr_t)(unsigned) rmtx->__depth;
    rmtx->__depth = 0;
    _MCF_atomic_store_32_rlx(rmtx->__owner, 0);
    _MCF_mutex_unlock(rmtx->__mutex);

    __MCF_ASSERT(unlocked > 0);
    return unlocked;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t arg, intptr_t unlocked)
  {
    __MCF_ASSERT(unlocked > 0);
    __MCF_gthr_rc_mutex* rmtx = (__MCF_gthr_rc_mutex*) arg;

    /* Relock the mutex and restore the depth counter.  */
    _MCF_mutex_lock(rmtx->__mutex, __MCF_nullptr);
    __MCF_ASSERT(rmtx->__owner[0] == 0);
    _MCF_atomic_store_32_rlx(rmtx->__owner, (int32_t) _MCF_thread_self_tid());
    rmtx->__depth = (int) unlocked;
  }

__MCF_DLLEXPORT
int
__MCF_gthr_cond_recursive_mutex_wait(_MCF_cond* cnd, __MCF_gthr_rc_mutex* rmtx, const int64_t* timeout_opt)
  {
    return _MCF_cond_wait(cnd, __MCF_gthr_recursive_mutex_unlock_callback,
                          __MCF_gthr_recursive_mutex_relock_callback,
                          (intptr_t) rmtx, timeout_opt);
  }

static
void
do_gthr_thread_thunk_v3(_MCF_thread* thrd)
  {
    __MCF_gthr_thread_record* rec = _MCF_thread_get_data(thrd);
    rec->__arg_or_result = (* rec->__proc) (rec->__arg_or_result);
  }

static inline
__MCF_gthr_thread_record*
do_gthr_get_thread_record(_MCF_thread* thrd)
  {
    __MCF_gthr_thread_record* rec = _MCF_thread_get_data(thrd);
    if(!rec)
      return __MCF_nullptr;

    /* Check the GUID. As user-defined data are aligned to 16-byte boundaries,
     * there must be at least 16 bytes available.  */
    if(!__MCF_mequal(rec->__magic_guid, __MCF_crt_gthread_guid, 16))
      return __MCF_nullptr;

    /* Assume so. `do_gthr_thread_thunk_v3()` is not shared across modules,
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
    return _MCF_thread_new(do_gthr_thread_thunk_v3, &record, sizeof(record));
  }

__MCF_DLLEXPORT
void
__MCF_gthr_thread_join_v3(_MCF_thread* thrd, void** resp_opt)
  {
    /* Wait for the thread to terminate.  */
    __MCF_ASSERT(thrd->__tid != _MCF_thread_self_tid());
    _MCF_thread_wait(thrd, __MCF_nullptr);

    if(resp_opt) {
      *resp_opt = __MCF_nullptr;

      /* Get the exit code.  */
      __MCF_gthr_thread_record* rec = do_gthr_get_thread_record(thrd);
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
    __MCF_gthr_thread_record* rec = do_gthr_get_thread_record(_MCF_thread_self());
    if(rec)
      rec->__arg_or_result = resp;

    /* Terminate the current thread.  */
    _MCF_thread_exit();
  }
