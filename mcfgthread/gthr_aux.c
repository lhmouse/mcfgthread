/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_GTHR_AUX_IMPORT  __MCF_DLLEXPORT
#define __MCF_GTHR_AUX_INLINE  __MCF_DLLEXPORT
#include "gthr_aux.h"
#include "xglobals.h"

void
__cdecl
__MCF_gthr_do_call_once_seh_take_over(_MCF_once* once, __MCF_cxa_dtor_any_ init_proc, void* arg)
; __asm__ (
#if defined __i386__ || defined __amd64__
/* This is required by Clang, where `-masm=intel` doesn't affect basic asm.  */
".intel_syntax noprefix  \n"
#endif
#if defined __i386__
/* On x86, SEH is stack-based.  */
".globl ___MCF_gthr_do_call_once_seh_take_over  \n"
".def ___MCF_gthr_do_call_once_seh_take_over; .scl 2; .type 32; .endef  \n"
"___MCF_gthr_do_call_once_seh_take_over:  \n"
#  ifdef _MSC_VER
".safeseh _do_call_once_seh_uhandler  \n"
#  endif
/* The stack is used as follows:
 *
 *    -20: argument to subroutines
 *    -16: unused
 *    -12: unused
 *     -8: establisher frame; pointer to previous frame
 *     -4: `do_call_once_seh_uhandler`
 * EBP  0: saved frame pointer
 *      4: saved ESI
 *      8: return address
 * ENT 12: `once`
 *     16: `init_proc`
 *     20: `arg`
 */
#  define __MCF_SEH_ONCE_PTR_DISPLACEMENT   20
"  push esi  \n"
"  push ebp  \n"
"  mov ebp, esp  \n"
"  sub esp, 20  \n"
/* Initialize the constant zero.  */
"  xor esi, esi  \n"
/* Install an SEH handler.  */
"  mov eax, fs:[esi]  \n"
"  lea ecx, [ebp - 8]  \n"
"  mov [ecx], eax  \n"
"  mov eax, OFFSET _do_call_once_seh_uhandler  \n"
"  mov [ecx + 4], eax  \n"
"  mov fs:[esi], ecx  \n"
/* Make the call `(*init_proc) (arg)`. The argument is passed
 * both via the ECX register and on the stack, to allow both
 * `__cdecl` and `__thiscall` functions to work properly.  */
"  mov ecx, [ebp + 20]  \n"
"  mov [ebp - 20], ecx  \n"
"  call [ebp + 16]  \n"
/* Dismantle the SEH handler.  */
"  mov eax, [ebp - 8]  \n"
"  mov fs:[esi], eax  \n"
/* Disarm the once flag with a tail call.  */
"  leave  \n"
"  pop esi  \n"
"  jmp __MCF_once_release  \n"
#else
/* Otherwise, SEH is table-based. `@unwind` without `@except`
 * works only on x86-64 and not on ARM, so let's keep both for
 * simplicity.  */
".globl __MCF_gthr_do_call_once_seh_take_over  \n"
".def __MCF_gthr_do_call_once_seh_take_over; .scl 2; .type 32; .endef  \n"
"__MCF_gthr_do_call_once_seh_take_over:  \n"
".seh_proc __MCF_gthr_do_call_once_seh_take_over  \n"
".seh_handler do_call_once_seh_uhandler, @except, @unwind  \n"
#  if defined __amd64__ && !defined __arm64ec__
/* The stack is used as follows:
 *
 *    -32: shadow slot for subroutines
 *    -24: ditto
 *    -16: ditto
 *     -8: ditto
 * RBP  0: establisher frame; saved frame pointer
 *      8: return address
 * ENT 16: shadow slot for `once` from RCX
 *     24: shadow slot for `init_proc` from RDX
 *     32: shadow slot for `arg` from R8
 *     40: unused
 */
#  define __MCF_SEH_ONCE_PTR_DISPLACEMENT   16
"  push rbp  \n"
".seh_pushreg rbp  \n"
"  mov rbp, rsp  \n"
".seh_setframe rbp, 0  \n"
"  sub rsp, 32  \n"
".seh_stackalloc 32  \n"
".seh_endprologue  \n"
/* Stash `once` for the handler.  */
"  mov [rbp + 16], rcx  \n"
/* Make the call `(*init_proc) (arg)`.  */
"  mov rcx, r8  \n"
"  call rdx  \n"
/* Disarm the once flag with a tail call. The x64 stack unwinder
 * recognizes `add rsp, SIZE` as the start of the epilogue.  */
"  mov rcx, [rbp + 16]  \n"
"  add rsp, 32  \n"
"  pop rbp  \n"
"  jmp _MCF_once_release  \n"
#  elif defined __aarch64__ || defined __arm64ec__
/* The stack is used as follows:
 *
 *  SP  0: saved FP
 *      8: saved LR
 *     16: `once` from X0
 *     24: unused
 * ENT 32: establisher frame
 */
#  define __MCF_SEH_ONCE_PTR_DISPLACEMENT   -16
"  stp fp, lr, [sp, #-32]!  \n"
".seh_save_fplr_x 32  \n"
"  mov fp, sp  \n"
".seh_set_fp  \n"
".seh_endprologue  \n"
/* Stash `once` for the handler.  */
"  str x0, [sp, #16]  \n"
/* Make the call `(*init_proc) (arg)`.  */
"  mov x0, x2  \n"
"  blr x1  \n"
/* Disarm the once flag with a tail call.  */
"  ldr x0, [sp, #16]  \n"
".seh_startepilogue  \n"
"  ldp fp, lr, [sp], #32  \n"
".seh_save_fplr_x 32  \n"
".seh_endepilogue  \n"
"  b _MCF_once_release  \n"
#  endif
".seh_endproc  \n"
#endif
);

static __attribute__((__used__, __unused__))  /* this must be good code!  */
EXCEPTION_DISPOSITION
__cdecl
do_call_once_seh_uhandler(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx)
  {
    (void) rec;
    (void) ctx;
    (void) disp_ctx;

    /* If we are unwinding the stack, restore the once flag.  */
    if(rec->ExceptionFlags & EXCEPTION_UNWINDING)
      _MCF_once_abort(*(void**) ((char*) estab_frame + __MCF_SEH_ONCE_PTR_DISPLACEMENT));

    /* Continue unwinding.  */
    return ExceptionContinueSearch;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_call_once_seh_take_over(_MCF_once* once, __MCF_cxa_dtor_any_ init_proc, void* arg)
  {
    __MCF_gthr_do_call_once_seh_take_over(once, init_proc, arg);
  }

__MCF_DLLEXPORT
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
__MCF_gthr_cond_mutex_wait(_MCF_cond* cond, _MCF_mutex* mtx, int64_t* timeout_opt)
  {
    return _MCF_cond_wait(cond, __MCF_gthr_mutex_unlock_callback,
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
__MCF_gthr_cond_shared_mutex_wait_shared(_MCF_cond* cond, _MCF_shared_mutex* smtx, int64_t* timeout_opt)
  {
    return _MCF_cond_wait(cond, __MCF_gthr_shared_mutex_unlock_callback,
                          __MCF_gthr_shared_mutex_relock_shared_callback,
                          (intptr_t) smtx, timeout_opt);
  }

__MCF_DLLEXPORT
int
__MCF_gthr_cond_shared_mutex_wait_exclusive(_MCF_cond* cond, _MCF_shared_mutex* smtx, int64_t* timeout_opt)
  {
    return _MCF_cond_wait(cond, __MCF_gthr_shared_mutex_unlock_callback,
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
__MCF_gthr_cond_recursive_mutex_wait(_MCF_cond* cond, __MCF_gthr_rc_mutex* rmtx, int64_t* timeout_opt)
  {
    return _MCF_cond_wait(cond, __MCF_gthr_recursive_mutex_unlock_callback,
                          __MCF_gthr_recursive_mutex_relock_callback,
                          (intptr_t) rmtx, timeout_opt);
  }

static
void
do_gthr_thread_thunk_v3(_MCF_thread* thrd)
  {
    __MCF_gthr_thread_record* rec = _MCF_thread_get_data(thrd);
    rec->__arg_or_result = (*(rec->__proc)) (rec->__arg_or_result);
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
    if(!__MCF_mequal(rec->__magic_guid, &__MCF_crt_gthread_guid, 16))
      return __MCF_nullptr;

    /* Assume so. `do_gthr_thread_thunk_v3()` is not shared across modules,
     * so we should not check it for uniqueness.  */
    return rec;
  }

__MCF_DLLEXPORT
_MCF_thread*
__MCF_gthr_thread_create_v3(__MCF_gthr_thread_procedure* proc, void* arg)
  {
    __MCF_gthr_thread_record record;
    __MCF_mcopy(record.__magic_guid, &__MCF_crt_gthread_guid, 16);
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
