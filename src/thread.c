/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#define __MCF_THREAD_EXTERN_INLINE
#include "thread.h"
#include "memory.h"
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
    if(!proc) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return NULL;
    }

    if(size > SIZE_MAX / 4 - sizeof(_MCF_thread)) {
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return NULL;
    }

    _MCF_thread* thrd = _MCF_malloc0(sizeof(_MCF_thread) + size);
    if(!thrd) {
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return NULL;
    }

    /* Initialize the thread control structure.  */
    __MCF_ATOMIC_STORE_RLX(thrd->__nref, 2);
    thrd->__proc = proc;

    if(data_opt)
      _MCF_mmove(thrd->__data, data_opt, size);

    /* Create the thread.
     * The new thread must not begin execution before the `__handle` field is
     * initialized, after `CreateThread()` returns, so suspend it first.  */
    thrd->__handle = CreateThread(NULL, 0, do_win32_thread_thunk, thrd, CREATE_SUSPENDED, (DWORD*) &(thrd->__tid));
    if(thrd->__handle == NULL) {
      _MCF_mfree_nonnull(thrd);
      return NULL;
    }

    __MCFGTHREAD_CHECK(ResumeThread(thrd->__handle));
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

    __MCFGTHREAD_CHECK(CloseHandle(thrd->__handle));
    _MCF_mfree_nonnull(thrd);
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
    LARGE_INTEGER timeout = { 0 };
    LARGE_INTEGER* use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);

    NTSTATUS status = NtWaitForSingleObject(thrd->__handle, FALSE, use_timeout);
    __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
    return (status == STATUS_SUCCESS) ? 0 : -1;
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
    LARGE_INTEGER timeout = { 0 };
    LARGE_INTEGER* use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);

    NTSTATUS status = NtDelayExecution(FALSE, use_timeout);
    __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
  }

void*
_MCF_tls_get(const _MCF_tls_key* key)
  {
    _MCF_thread* self = TlsGetValue(__MCF_win32_tls_index);
    if(!self)
      return NULL;

    return __MCF_tls_table_get(&(self->__tls_table), key);
  }

int
_MCF_tls_set(_MCF_tls_key* key, const void* value_opt)
  {
    _MCF_thread* self = TlsGetValue(__MCF_win32_tls_index);
    if(!self)
      return -1;

    return __MCF_tls_table_set(&(self->__tls_table), key, value_opt);
  }
