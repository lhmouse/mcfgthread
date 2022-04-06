// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_THREAD_C_  1
#include "thread.h"
#include "memory.h"
#include "win32.h"

__attribute__((__force_align_arg_pointer__, __noreturn__))
static DWORD __stdcall
do_win32_thread_thunk(LPVOID param)
  {
    // Set up the top SEH dispatcher for uncaughe exceptions.
#ifdef __i386__
    void* __i386_seh_node[2];
    __asm__ volatile (
      "movl %%fs:0, %%eax  \n"  // mov eax, dword fs:[0]
      "movl %%eax, (%0)    \n"  // mov dword [%0], eax
      "movl $___MCF_SEH_top_dispatcher, 4(%0)  \n"
                                // mov dword [%0+8], offset dispatcher
      "movl %0, %%fs:0     \n"  // mov %0, dword fs:[0]
      :  // output
      : "r"(__i386_seh_node)  // input
      : "eax", "memory"  // clobber
    );
#else
    __asm__ volatile (
      ".seh_handler __MCF_SEH_top_dispatcher, @except  \n"
      :  // output
      :  // input
      : "memory"  // clobber
    );
#endif

    // Attach the thread.
    _MCF_thread* const self = param;
    (void) TlsSetValue(__MCF_tls_index, self);

    // Execute the user-defined procedure, which should save the exit code
    // into `self->__exit_code`, which is also returned truncated.
    self->__proc(self);

    // Call `ExitThread()` so we don't have to uninstall the SEH dispatcher.
    ExitThread((DWORD) __atomic_load_n(self->__exit_code, __ATOMIC_RELAXED));
    __MCF_UNREACHABLE;
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

    // Create the thread.
    // The new thread must not begin execution before the `__handle` field is
    // initialized, after `CreateThread()` returns, so suspend it first.
    thrd->__handle = CreateThread(NULL, 0, do_win32_thread_thunk, thrd,
                                    CREATE_SUSPENDED, (DWORD*) &(thrd->__tid));
    if(thrd->__handle == NULL) {
      _MCF_mfree(thrd);
      return NULL;
    }

    // Initialize the thread control structure.
    if(data_opt)
      _MCF_mmove(thrd->__data, data_opt, size);

    __atomic_store_n(thrd->__nref, 2, __ATOMIC_RELAXED);
    thrd->__proc = proc;
    __MCFGTHREAD_CHECK(ResumeThread(thrd->__handle));
    return thrd;
  }

void
_MCF_thread_drop_ref(_MCF_thread* thrd)
  {
    if(thrd == NULL)
      return;

    int old_ref = __atomic_fetch_sub(thrd->__nref, 1, __ATOMIC_ACQ_REL);
    __MCFGTHREAD_ASSERT(old_ref > 0);

    // The main thread structure is allocated statically and must not be freed.
    if(thrd == &__MCF_main_thread)
      return;

    if(old_ref != 1)
      return;

    // Free the thread now.
    __MCFGTHREAD_CHECK(CloseHandle(thrd->__handle));
    _MCF_mfree(thrd);
  }

void
_MCF_thread_exit(intptr_t exit_code)
  {
    _MCF_thread* const self = TlsGetValue(__MCF_tls_index);
    if(self)
      __atomic_store_n(self->__exit_code, exit_code, __ATOMIC_RELEASE);

    ExitThread((DWORD) exit_code);
    __MCF_UNREACHABLE;
  }

int
_MCF_thread_wait(const _MCF_thread* thrd, const int64_t* timeout_opt)
  {
    LARGE_INTEGER timeout = { 0 };
    BOOLEAN use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);
    NTSTATUS status = NtWaitForSingleObject(thrd->__handle, FALSE, use_timeout ? &timeout : NULL);
    __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
    return (status == STATUS_SUCCESS) ? 0 : -1;
  }

_MCF_thread*
_MCF_thread_self(void)
  {
    return TlsGetValue(__MCF_tls_index);
  }

void
_MCF_sleep(const int64_t* timeout_opt)
  {
    LARGE_INTEGER timeout = { 0 };
    BOOLEAN use_timeout = __MCF_initialize_timeout(&timeout, timeout_opt);
    NTSTATUS status = NtDelayExecution(FALSE, use_timeout ? &timeout : NULL);
    __MCFGTHREAD_ASSERT(NT_SUCCESS(status));
  }

void
__MCF_thread_exit_callback(void)
  {
    _MCF_thread* const self = TlsGetValue(__MCF_tls_index);
    if(!self)
      return;

    __MCF_dtorelem elem;
    while(__MCF_dtorque_pop(&elem, &(self->__atexit_queue), NULL) == 0)
      __MCF_dtorelem_execute(&elem);

   // Detach the thread.
   (void) TlsSetValue(__MCF_tls_index, NULL);
    _MCF_thread_drop_ref(self);
  }
