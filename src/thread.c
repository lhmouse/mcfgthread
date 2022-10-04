/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_THREAD(...)  __MCF_DLLEXPORT
#include "thread.h"
#include "mutex.h"
#include "cond.h"
#include "xglobals.i"

static __attribute__((__force_align_arg_pointer__))
DWORD
__stdcall
do_win32_thread_thunk(LPVOID param)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    _MCF_thread* self = param;

#if defined(__i386__) || defined(__amd64__)
    /* Set x87 precision to 80 bits.  */
    __asm__ volatile ("fninit");
#endif  /* x86  */

    /* Wait until the structure has been fully initialized.  */
    DWORD cmp = 0;
    if(_MCF_atomic_cmpxchg_32_rlx(&(self->__tid), &cmp, -1))
      __MCF_keyed_event_wait(self, NULL);

    /* Attach the thread and execute the user-defined procedure.  */
    TlsSetValue(__MCF_win32_tls_index, self);
    self->__proc(self);
    return 0;
  }

__MCF_DLLEXPORT
_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* proc, const void* data_opt, size_t size)
  {
    /* Validate arguments.  */
    if(!proc)
      return __MCF_win32_error_p(ERROR_INVALID_PARAMETER, NULL);

    if(size > PTRDIFF_MAX - 0x100000)  /* max offsettable memory - 1 MiB  */
      return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, NULL);

    /* Allocate and initialize the thread control structure.  */
    _MCF_thread* thrd = __MCF_malloc_0(sizeof(_MCF_thread) + size);
    if(!thrd)
      return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, NULL);

    _MCF_atomic_store_32_rlx(thrd->__nref, 2);
    thrd->__proc = proc;
    thrd->__data_ptr = thrd->__data_storage;

    /* Create the thread. The new thread will wait until `__tid` contains a
     * valid thread ID.  */
    DWORD tid;
    thrd->__handle = CreateThread(NULL, 0, do_win32_thread_thunk, thrd, 0, &tid);
    if(thrd->__handle == NULL) {
      __MCF_mfree(thrd);
      return NULL;
    }

    /* Copy user-defined data in, before setting the `__tid` field.  */
    if(data_opt)
      __MCF_mcopy(thrd->__data_storage, data_opt, size);

    /* Set the thread ID. If its old value is not zero, the new thread should
     * have been waiting, so notify it.  */
    if(_MCF_atomic_xchg_32_rlx(&(thrd->__tid), (int32_t) tid) != 0)
      __MCF_keyed_event_signal(thrd, NULL);

    __MCF_ASSERT(thrd->__tid == tid);
    return thrd;
  }

__MCF_DLLEXPORT
void
_MCF_thread_drop_ref_nonnull(_MCF_thread* thrd)
  {
    int32_t old_ref = _MCF_atomic_xsub_32_arl(thrd->__nref, 1);
    __MCF_ASSERT(old_ref > 0);
    if(old_ref != 1)
      return;

    /* The main thread structure is allocated statically and must not be freed.  */
    if(thrd == &__MCF_main_thread)
      return;

    NTSTATUS status = NtClose(thrd->__handle);
    __MCF_ASSERT(NT_SUCCESS(status));
    __MCF_mfree(thrd);
  }

__MCF_DLLEXPORT
void
_MCF_thread_exit(void)
  {
    ExitThread(0);
    __MCF_UNREACHABLE;
  }

__MCF_DLLEXPORT
int
_MCF_thread_wait(const _MCF_thread* thrd, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v2(&nt_timeout, timeout_opt);

    NTSTATUS status = NtWaitForSingleObject(thrd->__handle, false, nt_timeout.__li);
    __MCF_ASSERT(NT_SUCCESS(status));
    return (status != STATUS_WAIT_0) ? -1 : 0;
  }

__MCF_DLLEXPORT
_MCF_thread*
_MCF_thread_self(void)
  {
    return TlsGetValue(__MCF_win32_tls_index);
  }

__MCF_DLLEXPORT
uint32_t
_MCF_thread_self_tid(void)
  {
    return GetCurrentThreadId();
  }

__MCF_DLLEXPORT
void
_MCF_yield(void)
  {
    SwitchToThread();
  }

static
BOOL
__stdcall
do_handle_interrupt(DWORD type)
  {
    (void) type;
    _MCF_cond_signal_all(&__MCF_interrupt_cond);
    return false;
  }

static inline
void
do_handler_cleanup(BOOL* added)
  {
    if(*added)
      SetConsoleCtrlHandler(do_handle_interrupt, false);
  }

__MCF_DLLEXPORT
int
_MCF_sleep(const int64_t* timeout_opt)
  {
    /* Set a handler to receive Ctrl-C notifications.  */
    BOOL added __attribute__((__cleanup__(do_handler_cleanup))) = false;
    added = SetConsoleCtrlHandler(do_handle_interrupt, true);

    int err = _MCF_cond_wait(&__MCF_interrupt_cond, NULL, NULL, 0, timeout_opt);
    return err ^ -1;
  }
