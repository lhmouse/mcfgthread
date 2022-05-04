/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#define __MCF_THREAD_EXTERN_INLINE
#include "thread.h"
#include "win32.h"

static
DWORD __stdcall
do_win32_thread_thunk(LPVOID param)
  __attribute__((__force_align_arg_pointer__));

static
DWORD __stdcall
do_win32_thread_thunk(LPVOID param)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    _MCF_thread* self = param;

    /* Wait until the structure has been fully initialized.  */
    while(__MCF_ATOMIC_LOAD_ACQ(&(self->__tid)) == 0) {
      LARGE_INTEGER timeout;
      timeout.QuadPart = -262144;  /* 26.2 ms  */

      NTSTATUS status = NtDelayExecution(false, &timeout);
      __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
    }

    /* Attach the thread.  */
    (void) TlsSetValue(__MCF_win32_tls_index, self);

    /* Execute the user-defined procedure, which has no return value.  */
    self->__proc(self);
    return 0;
  }

_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* proc, const void* data_opt, size_t size)
  {
    /* Validate arguments.  */
    if(!proc)
      return __MCF_win32_error_p(ERROR_INVALID_PARAMETER, NULL);

    if(size > SIZE_MAX / 4 - sizeof(_MCF_thread))
      return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, NULL);

    _MCF_thread* thrd = __MCF_malloc_0(sizeof(_MCF_thread) + size);
    if(!thrd)
      return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, NULL);

    /* Initialize the thread control structure.  */
    thrd->__proc = proc;
    if(data_opt)
      __MCF_mcopy(thrd->__data, data_opt, size);

    /* Create the thread. The new thread will wait until `__nref` contains a
     * non-zero value.  */
    thrd->__handle = CreateThread(NULL, 0, do_win32_thread_thunk, thrd, 0, (DWORD*) &(thrd->__tid));
    if(thrd->__handle == NULL) {
      __MCF_mfree(thrd);
      return NULL;
    }
    __MCF_ATOMIC_STORE_RLX(thrd->__nref, 2);
    return thrd;
  }

void
_MCF_thread_drop_ref_nonnull(_MCF_thread* thrd)
  {
    int old_ref = __MCF_ATOMIC_SUB_ARL(thrd->__nref, 1);
    __MCFGTHREAD_ASSERT(old_ref > 0);
    if(old_ref != 1)
      return;

    /* The main thread structure is allocated statically and must not be freed.  */
    if(thrd == &__MCF_main_thread)
      return;

    __MCFGTHREAD_CHECK(NT_SUCCESS(NtClose(thrd->__handle)));
    __MCF_mfree(thrd);
  }

void
_MCF_thread_exit()
  {
    ExitThread(0);
    __MCF_UNREACHABLE;
  }

int
_MCF_thread_wait(const _MCF_thread* thrd, const int64_t* timeout_opt)
  {
    LARGE_INTEGER timeout = __MCF_0_INIT;
    LARGE_INTEGER* use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);

    NTSTATUS status = NtWaitForSingleObject(thrd->__handle, false, use_timeout);
    __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
    return (status != STATUS_WAIT_0) ? -1 : 0;
  }

_MCF_thread*
_MCF_thread_self(void)
  {
    return TlsGetValue(__MCF_win32_tls_index);
  }

uint32_t
_MCF_thread_self_tid(void)
  {
    return GetCurrentThreadId();
  }

void
_MCF_yield(void)
  {
    SwitchToThread();
  }

void
_MCF_sleep(const int64_t* timeout_opt)
  {
    LARGE_INTEGER timeout = __MCF_0_INIT;
    LARGE_INTEGER* use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);

    NTSTATUS status = NtDelayExecution(false, use_timeout);
    __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
  }
