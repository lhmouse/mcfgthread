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

__asm__ (
#if defined __MCF_IN_DLL
"\n .section .drectve, \"yn\""
"\n .ascii \" -export:" __MCF_USYM "__MCF_gthr_call_once_seh_take_over\""
#endif
"\n .section .text$" __MCF_USYM "__MCF_gthr_call_once_seh_take_over, \"x\""
"\n .def " __MCF_USYM "__MCF_gthr_call_once_seh_take_over; .scl 2; .type 32; .endef"
"\n .globl " __MCF_USYM "__MCF_gthr_call_once_seh_take_over"
"\n " __MCF_USYM "__MCF_gthr_call_once_seh_take_over:"
#if defined __i386__
/* On x86-32, SEH is stack-based. The stack is used as follows:
 *
 *    -24: argument to subroutines
 *    -20: unused
 *    -16: establisher frame; pointer to previous frame
 *    -12: `_do_i386_call_once_on_except`
 *     -8: unused
 *     -4: saved ESI
 * EBP  0: saved frame pointer
 *      4: return address
 * CFA  8: `once`
 *     12: `init_proc`
 *     16: `arg`
 */
"\n   push ebp"
"\n   mov ebp, esp"
"\n   push esi"
"\n   sub esp, 20"
/* Initialize the constant zero.  */
"\n   xor esi, esi"
/* Install an SEH handler.  */
"\n   mov eax, DWORD PTR fs:[esi]"
"\n   lea ecx, [ebp - 16]"
"\n   mov DWORD PTR [ecx], eax"
"\n   mov DWORD PTR [ecx + 4], OFFSET _do_i386_call_once_on_except"
"\n   mov DWORD PTR fs:[esi], ecx"
/* Make the call `(*init_proc) (arg)`. The argument is passed both via the
 * ECX register and on the stack, to allow both `__cdecl` and `__thiscall`
 * functions to work properly.  */
"\n   mov ecx, DWORD PTR [ebp + 16]"
"\n   mov DWORD PTR [ebp - 24], ecx"
"\n   call DWORD PTR [ebp + 12]"
/* Dismantle the SEH handler.  */
"\n   mov eax, DWORD PTR [ebp - 16]"
"\n   mov DWORD PTR fs:[esi], eax"
/* Disarm the once flag with a tail call.  */
"\n   lea esp, [ebp - 4]"
"\n   pop esi"
"\n   pop ebp"
"\n   jmp __MCF_once_release"
/* Define the exception handler, which is called either when an exception is
 * raised, or the stack is being unwound.  */
"\n .def _do_i386_call_once_on_except; .scl 3; .type 32; .endef"
"\n _do_i386_call_once_on_except:"
/* Check whether `ExceptionFlags` contains `EXCEPTION_UNWINDING`.  */
"\n   mov ecx, DWORD PTR [esp + 4]"
"\n   test BYTE PTR [ecx + 4], 2"
"\n   jz 1001f"
/* Locate the once flag from `EstablisherFrame`, and reset it.  */
"\n   mov eax, DWORD PTR [esp + 8]"
"\n   push DWORD PTR [eax + 24]"
"\n   call __MCF_once_abort"
"\n   pop eax"
"\n 1001:"
/* Return `ExceptionContinueSearch`.  */
"\n   mov eax, 1"
"\n   ret"
#  if defined __MCF_IN_DLL
"\n .globl ___MCF_gthr_do_i386_call_once_on_except"
"\n .equiv ___MCF_gthr_do_i386_call_once_on_except, _do_i386_call_once_on_except"
#  elif defined _MSC_VER
"\n .safeseh _do_i386_call_once_on_except"
#  endif
#elif defined __amd64__ && !defined __arm64ec__
/* On x86-64, SEH is table-based. We register an unwind handler which is not
 * called when an exception is raised, but is called when the stack is being
 * unwound. The stack is used as follows:
 *
 *    -32: shadow slot for subroutines
 *    -24: ditto
 *    -16: ditto
 *     -8: ditto
 * RBP  0: establisher frame; saved frame pointer
 *      8: return address
 * CFA 16: shadow slot for `once` from RCX
 *     24: shadow slot for `init_proc` from RDX
 *     32: shadow slot for `arg` from R8
 *     40: unused
 */
"\n .seh_proc __MCF_gthr_call_once_seh_take_over"
"\n .seh_handler do_amd64_call_once_on_unwind, @unwind"
"\n   push rbp"
"\n .seh_pushreg rbp"
"\n   mov rbp, rsp"
"\n .seh_setframe rbp, 0"
"\n   sub rsp, 32"
"\n .seh_stackalloc 32"
"\n .seh_endprologue"
/* Stash `once` for the handler.  */
"\n   mov QWORD PTR [rbp + 16], rcx"
/* Make the call `(*init_proc) (arg)`.  */
"\n   mov rcx, r8"
"\n   call rdx"
/* Disarm the once flag with a tail call. The x64 stack unwinder recognizes
 * `add rsp, SIZE` as the start of the epilogue.  */
"\n   mov rcx, QWORD PTR [rbp + 16]"
"\n   add rsp, 32"
"\n   pop rbp"
"\n   jmp _MCF_once_release"
"\n .seh_endproc"
/* Define the unwind handler, which is called the stack is being unwound.  */
"\n .def do_amd64_call_once_on_unwind; .scl 3; .type 32; .endef"
"\n do_amd64_call_once_on_unwind:"
"\n   sub rsp, 40"
/* Locate the once flag from `EstablisherFrame`, and reset it.  */
"\n   mov rcx, QWORD PTR [rdx + 16]"
"\n   call _MCF_once_abort"
/* Return `ExceptionContinueSearch`.  */
"\n   mov eax, 1"
"\n   add rsp, 40"
"\n   ret"
#elif defined __aarch64__ || defined __arm64ec__
/* On ARM64, SEH is table-based. Unlike x86-64 but like x86-32, there is only
 * one kind of handler which is called in either case. The stack is used as
 * follows:
 *
 *  SP  0: saved FP
 *      8: saved LR
 *     16: `once` from X0
 *     24: unused
 * CFA 32: establisher frame
 */
"\n .seh_proc __MCF_gthr_call_once_seh_take_over"
"\n .seh_handler do_arm64_call_once_on_except, @except"
"\n   stp fp, lr, [sp, -32]!"
"\n .seh_save_fplr_x 32"
"\n   mov fp, sp"
"\n .seh_set_fp"
"\n .seh_endprologue"
/* Stash `once` for the handler.  */
"\n   str x0, [sp, 16]"
/* Make the call `(*init_proc) (arg)`.  */
"\n   mov x0, x2"
#  if defined __arm64ec__
"\n   mov x11, x1"
"\n   bl __MCF_arm64ec_icall_helper_p"
#  else
"\n   blr x1"
#  endif
/* Disarm the once flag with a tail call.  */
"\n   ldr x0, [sp, 16]"
"\n .seh_startepilogue"
"\n   ldp fp, lr, [sp], 32"
"\n .seh_save_fplr_x 32"
"\n .seh_endepilogue"
"\n   b _MCF_once_release"
"\n .seh_endproc"
/* Define the exception handler, which is called either when an exception is
 * raised, or the stack is being unwound.  */
"\n .def do_arm64_call_once_on_except; .scl 3; .type 32; .endef"
"\n do_arm64_call_once_on_except:"
"\n   stp fp, lr, [sp, -16]!"
/* Check whether `ExceptionFlags` contains `EXCEPTION_UNWINDING`.  */
"\n   ldr w8, [x0, 4]"
"\n   tbz w8, 1, 3001f"
/* Locate the once flag from `EstablisherFrame`, and reset it.  */
"\n   ldur x0, [x1, -16]"
"\n   bl _MCF_once_abort"
"\n 3001:"
/* Return `ExceptionContinueSearch`.  */
"\n   mov w0, 1"
"\n   ldp fp, lr, [sp], 16"
"\n   ret"
#endif
"\n .text"
);

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
    __MCF_gthr_thread_record record;
    __MCF_mcopy(record.__magic_guid, __MCF_crt_gthread_guid, 16);
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
