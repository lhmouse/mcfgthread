// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_THREAD_C_  1
#include "thread.h"
#include "memory.h"
#include "win32.h"

static DWORD __stdcall
do_win32_thread(LPVOID param)
  __attribute__((__force_align_arg_pointer__));

static DWORD __stdcall
do_win32_thread(LPVOID param)
  {
    __MCF_SEH_TERMINATE_FILTER_BEGIN
    _MCF_thread* const self = param;
    register intptr_t exit_code;

    // Attach the thread.
    (void) TlsSetValue(__MCF_win32_tls_index, self);

    // Execute the user-defined procedure, which should save the exit code
    // into `self->__exit_code`, which is also returned truncated.
    self->__proc(self);
    exit_code = __atomic_load_n(self->__exit_code, __ATOMIC_RELAXED);

    __MCF_SEH_TERMINATE_FILTER_END
    return (DWORD) exit_code;
  }

_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* proc, const void* data_opt, size_t size)
  {
    if(!proc)
      __MCF_SET_ERROR_AND_RETURN(ERROR_INVALID_PARAMETER, NULL);

    // Allocate the thread control structure.
    size_t thrd_size = sizeof(_MCF_thread);
    if(size > PTRDIFF_MAX - thrd_size)
      __MCF_SET_ERROR_AND_RETURN(ERROR_NOT_ENOUGH_MEMORY, NULL);

    thrd_size += size;
    _MCF_thread* thrd = _MCF_malloc0(thrd_size);
    if(!thrd)
      return NULL;

    if(data_opt)
      _MCF_mmove(thrd->__data, data_opt, size);

    // Create the thread.
    // The new thread must not begin execution before the `__handle` field is
    // initialized, after `CreateThread()` returns, so suspend it first.
    thrd->__handle = CreateThread(NULL, 0, do_win32_thread, thrd, CREATE_SUSPENDED, (DWORD*) &(thrd->__tid));
    if(thrd->__handle == NULL) {
      _MCF_mfree_nonnull(thrd);
      return NULL;
    }

    // Initialize the thread control structure.
    thrd->__proc = proc;
    __atomic_store_n(thrd->__nref, 2, __ATOMIC_RELAXED);
    __MCFGTHREAD_CHECK(ResumeThread(thrd->__handle));
    return thrd;
  }

void
_MCF_thread_drop_ref_nonnull(_MCF_thread* thrd)
  {
    int old_ref = __atomic_fetch_sub(thrd->__nref, 1, __ATOMIC_ACQ_REL);
    __MCFGTHREAD_ASSERT(old_ref > 0);
    if(old_ref != 1)
      return;

    // The main thread structure is allocated statically and must not be freed.
    if(thrd == &__MCF_main_thread)
      return;

    __MCFGTHREAD_CHECK(CloseHandle(thrd->__handle));
    _MCF_mfree_nonnull(thrd);
  }

void
_MCF_thread_exit(intptr_t exit_code)
  {
    _MCF_thread* const self = TlsGetValue(__MCF_win32_tls_index);
    if(self)
      __atomic_store_n(self->__exit_code, exit_code, __ATOMIC_RELEASE);

    ExitThread((DWORD) exit_code);
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
    _MCF_thread* const self = TlsGetValue(__MCF_win32_tls_index);
    if(!self)
      return NULL;

    return __MCF_tls_table_get(&(self->__tls_table), key);
  }

int
_MCF_tls_set(_MCF_tls_key* key, const void* value_opt)
  {
    _MCF_thread* const self = TlsGetValue(__MCF_win32_tls_index);
    if(!self)
      return -1;

    return __MCF_tls_table_set(&(self->__tls_table), key, value_opt);
  }

void
__MCF_thread_exit_callback(void)
  {
    _MCF_thread* const self = TlsGetValue(__MCF_win32_tls_index);
    if(!self)
      return;

    // Call destructors for thread-local objects.
    __MCF_dtor_queue_finalize(&(self->__atexit_queue), NULL, NULL);
    __MCF_tls_table_finalize(&(self->__tls_table));

   // Detach the thread.
   (void) TlsSetValue(__MCF_win32_tls_index, NULL);
    _MCF_thread_drop_ref_nonnull(self);
  }
