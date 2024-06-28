/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

#include "xprecompiled.h"
#define __MCF_GTHR_AUX_IMPORT  __MCF_DLLEXPORT
#define __MCF_GTHR_AUX_INLINE  __MCF_DLLEXPORT
#include "gthr_aux.h"
#include "xglobals.h"

void
__cdecl
do_call_once_seh_take_over(_MCF_once* once, __MCF_cxa_dtor_cdecl* init_proc, void* arg);
__asm__ (
".text  \n\t"
#if defined __i386__ || defined __amd64__
/* This is required by Clang, where `-masm=intel` doesn't affect basic asm.  */
".intel_syntax noprefix  \n\t"
#endif
#if defined __i386__
/* On x86, SEH is stack-based.  */
".def _do_call_once_seh_take_over; .scl 2; .type 32; .endef        \n\t"
"_do_call_once_seh_take_over:                                      \n\t"
/* The stack is used as follows:
 *
 *    -24: argument to subroutines
 *    -20: unused
 *    -16: unused
 *    -12: unused
 *     -8: establisher frame; pointer to previous frame
 *     -4: `do_call_once_seh_uhandler`
 * EBP  0: saved frame pointer
 * ENT  4: return address
 *      8: `once`
 *     12: `init_proc`
 *     16: `arg`
 */
#  define __MCF_SEH_ONCE_PTR_DISPLACEMENT   16
"  push ebp                                                        \n\t"
"  mov ebp, esp                                                    \n\t"
"  sub esp, 24                                                     \n\t"
/* Install an SEH handler.  */
"  mov eax, DWORD PTR fs:[0]                                       \n\t"
"  lea ecx, DWORD PTR [ebp - 8]                                    \n\t"
"  mov DWORD PTR [ecx], eax                                        \n\t"
"  mov DWORD PTR [ecx + 4], OFFSET _do_call_once_seh_uhandler      \n\t"
"  mov DWORD PTR fs:[0], ecx                                       \n\t"
/* Make the call `(*init_proc) (arg)`.  */
"  mov eax, DWORD PTR [ebp + 12]                                   \n\t"
"  mov ecx, DWORD PTR [ebp + 16]                                   \n\t"
"  mov DWORD PTR [ebp - 24], ecx                                   \n\t"
"  call eax                                                        \n\t"
/* Dismantle the SEH handler.  */
"  mov ecx, DWORD PTR [ebp - 8]                                    \n\t"
"  mov DWORD PTR fs:[0], ecx                                       \n\t"
/* Disarm the once flag with a tail call.  */
"  leave                                                           \n\t"
"  jmp __MCF_once_release                                          \n\t"
#else
/* Otherwise, SEH is table-based.  */
".def do_call_once_seh_take_over; .scl 2; .type 32; .endef         \n\t"
"do_call_once_seh_take_over:                                       \n\t"
".seh_proc do_call_once_seh_take_over                              \n\t"
".seh_handler do_call_once_seh_uhandler, " __MCF_SEH_FLAG_UNWIND " \n\t"
#  if defined __amd64__
/* The stack is used as follows:
 *
 *    -32: shallow slot for subroutines
 *    -24: ditto
 *    -16: ditto
 *     -8: ditto
 * RBP  0: establisher frame; saved frame pointer
 * ENT  8: return address
 *     16: shallow slot for `once` from RCX
 *     24: shallow slot for `init_proc` from RDX
 *     32: shallow slot for `arg` from R8
 *     40: unused
 */
#  define __MCF_SEH_ONCE_PTR_DISPLACEMENT   16
"  push rbp                                                        \n\t"
".seh_pushreg rbp                                                  \n\t"
"  mov rbp, rsp                                                    \n\t"
".seh_setframe rbp, 0                                              \n\t"
"  sub rsp, 32                                                     \n\t"
".seh_stackalloc 32                                                \n\t"
".seh_endprologue                                                  \n\t"
/* Stash `once` for the handler.  */
"  mov QWORD PTR [rbp + 16], rcx                                   \n\t"
/* Make the call `(*init_proc) (arg)`.  */
"  mov rcx, r8                                                     \n\t"
"  call rdx                                                        \n\t"
/* Disarm the once flag with a tail call.  */
"  mov rcx, QWORD PTR [rbp + 16]                                   \n\t"
"  leave                                                           \n\t"
"  jmp _MCF_once_release                                           \n\t"
#  elif defined __aarch64__
/* The stack is used as follows:
 *
 *  SP  0: saved FP
 *      8: saved LR
 *     16: `once` from X0
 *     24: unused
 * ENT 32: establisher frame
 */
#  define __MCF_SEH_ONCE_PTR_DISPLACEMENT   -16
"  stp fp, lr, [sp, #-32]!                                         \n\t"
".seh_save_fplr_x 32                                               \n\t"
"  mov fp, sp                                                      \n\t"
".seh_set_fp                                                       \n\t"
".seh_endprologue                                                  \n\t"
/* Stash `once` for the handler.  */
"  str x0, [sp, #16]                                               \n\t"
/* Make the call `(*init_proc) (arg)`.  */
"  mov x0, x2                                                      \n\t"
"  blr x1                                                          \n\t"
/* Disarm the once flag with a tail call.  */
"  ldr x0, [sp, #16]                                               \n\t"
".seh_startepilogue                                                \n\t"
"  ldp fp, lr, [sp], #32                                           \n\t"
".seh_save_fplr_x 32                                               \n\t"
".seh_endepilogue                                                  \n\t"
"  b _MCF_once_release                                             \n\t"
#  endif
".seh_endproc                                                      \n\t"
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
__MCF_gthr_call_once_seh_take_over(_MCF_once* once, __MCF_cxa_dtor_cdecl* init_proc, void* arg)
  {
    do_call_once_seh_take_over(once, init_proc, arg);
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
intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t arg)
  {
    __MCF_gthr_rc_mutex* rmtx = (__MCF_gthr_rc_mutex*) arg;

    /* Clear the depth counter and return it.  */
    intptr_t unlocked = rmtx->__depth;
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
    rmtx->__depth = (int32_t) unlocked;
  }

__MCF_DLLEXPORT
void
__MCF_gthr_thread_thunk_v2(_MCF_thread* thrd)
  {
    __MCF_gthr_thread_record* rec = _MCF_thread_get_data(thrd);

    /* Invoke the user-defined procedure and save its result in the record.  */
    rec->__result = rec->__proc(rec->__arg);
  }
