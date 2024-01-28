/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

#include "precompiled.h"
#define __MCF_THREAD_IMPORT  __MCF_DLLEXPORT
#define __MCF_THREAD_INLINE  __MCF_DLLEXPORT
#include "thread.h"
#include "xglobals.i"

static __attribute__((__force_align_arg_pointer__))
DWORD
__stdcall
do_win32_thread_thunk(LPVOID param)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    _MCF_thread* const self = param;

    /* `__tid` has to be set in case that this thread begins execution before
     * `CreateThread()` returns from the other thread.  */
    _MCF_atomic_store_32_rlx(&(self->__tid), (int32_t) _MCF_thread_self_tid());

#if defined __i386__ || defined __amd64__
    /* Set x87 precision to 64-bit mantissa (GNU `long double` format).  */
    __asm__ volatile ("fninit");
#endif  /* x86  */

    /* Attach the thread and execute the user-defined procedure.  */
    __MCF_CHECK(TlsSetValue(__MCF_g->__tls_index, self));
    self->__proc(self);
    return 0;
  }

__MCF_DLLEXPORT
_MCF_thread*
_MCF_thread_new_aligned(_MCF_thread_procedure* proc, size_t align, const void* data_opt, size_t size)
  {
    /* Validate arguments.  */
    if(!proc)
      return __MCF_win32_error_p(ERROR_INVALID_PARAMETER, __MCF_nullptr);

    if(align & (align - 1))
      return __MCF_win32_error_p(ERROR_NOT_SUPPORTED, __MCF_nullptr);

    if(align >= 0x10000000U)
      return __MCF_win32_error_p(ERROR_NOT_SUPPORTED, __MCF_nullptr);

    if(size >= 0x7FF00000U)
      return __MCF_win32_error_p(ERROR_ARITHMETIC_OVERFLOW, __MCF_nullptr);

    /* Calculate the number of bytes to allocate.  */
    size_t real_alignment = _MCF_maxz(__MCF_THREAD_DATA_ALIGNMENT, align);
    size_t size_need = sizeof(_MCF_thread) + size;
    size_t size_request = size_need + real_alignment - MEMORY_ALLOCATION_ALIGNMENT;
    __MCF_CHECK(size_need <= size_request);

    /* Allocate and initialize the thread control structure.  */
    _MCF_thread* thrd = __MCF_malloc_0(size_request);
    if(!thrd)
      return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, __MCF_nullptr);

    _MCF_atomic_store_32_rlx(thrd->__nref, 2);
    thrd->__proc = proc;

    if(size != 0) {
      thrd->__data_opt = thrd->__data_storage;

      /* Adjust `__data_opt` for over-aligned types. If we have over-allocated
       * memory, give back some. Errors are ignored.  */
      if(size_need != size_request) {
        thrd->__data_opt = (void*) ((((uintptr_t) thrd->__data_opt - 1) | (real_alignment - 1)) + 1);

        size_request = (uintptr_t) thrd->__data_opt + size - (uintptr_t) thrd;
        __MCF_CHECK(size_need <= size_request);
        HeapReAlloc(__MCF_crt_heap, HEAP_REALLOC_IN_PLACE_ONLY, thrd, size_request);
      }

      /* Copy user-defined data. If this doesn't happen, they are implicit zeroes.  */
      if(data_opt)
        __MCF_mcopy(thrd->__data_opt, data_opt, size);
    }

    /* Create the thread now.  */
    DWORD tid;
    thrd->__handle = CreateThread(__MCF_nullptr, 0, do_win32_thread_thunk, thrd, 0, &tid);
    if(thrd->__handle == __MCF_nullptr) {
      __MCF_mfree(thrd);
      return __MCF_nullptr;
    }

    /* Set `__tid` in case `CreateThread()` returns before the new thread begins
     * execution. It may be overwritten by the new thread with the same value.  */
    _MCF_atomic_store_32_rlx(&(thrd->__tid), (int32_t) tid);
    return thrd;
  }

__MCF_DLLEXPORT
_MCF_thread*
__MCF_thread_attach_foreign(_MCF_thread* thrd)
  {
    __MCF_ASSERT(thrd->__nref[0] == 0);
    __MCF_ASSERT(thrd->__tid == 0);
    __MCF_ASSERT(thrd->__handle == __MCF_nullptr);
    __MCF_ASSERT(TlsGetValue(__MCF_g->__tls_index) == __MCF_nullptr);

    /* Initialize thread identity fields.  */
    thrd->__tid = _MCF_thread_self_tid();
    __MCF_CHECK_NT(NtDuplicateObject(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &(thrd->__handle), 0, 0, DUPLICATE_SAME_ACCESS));
    __MCF_CHECK(thrd->__handle);

    /* Initialize the reference count to detached state.  */
    _MCF_atomic_store_32_rel(thrd->__nref, 1);

    /* Attach the thread now.  */
    __MCF_CHECK(TlsSetValue(__MCF_g->__tls_index, thrd));
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
    if(thrd == __MCF_g->__main_thread)
      return;

    /* Deallocate all associated resources.  */
    __MCF_close_handle(thrd->__handle);
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
_MCF_thread_wait(const _MCF_thread* thrd_opt, const int64_t* timeout_opt)
  {
    if(!thrd_opt)
      return -1;

    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

    NTSTATUS status = NtWaitForSingleObject(thrd_opt->__handle, false, nt_timeout.__li);
    __MCF_ASSERT_NT(status);
    return (status != STATUS_WAIT_0) ? -1 : 0;
  }

__MCF_DLLEXPORT
_MCF_thread_priority
_MCF_thread_get_priority(const _MCF_thread* thrd_opt)
  {
    HANDLE handle = thrd_opt ? thrd_opt->__handle : GetCurrentThread();
    return (_MCF_thread_priority) GetThreadPriority(handle);
  }

__MCF_DLLEXPORT
int
_MCF_thread_set_priority(_MCF_thread* thrd_opt, _MCF_thread_priority priority)
  {
    HANDLE handle = thrd_opt ? thrd_opt->__handle : GetCurrentThread();
    BOOL succ = SetThreadPriority(handle, (int8_t) priority);
    return !succ ? -1 : 0;
  }

__MCF_DLLEXPORT
_MCF_thread*
_MCF_thread_self(void)
  {
    _MCF_thread* self = TlsGetValue(__MCF_g->__tls_index);
    if(__builtin_expect(self != __MCF_nullptr, 1))
      return self;

    self = __MCF_malloc_0(sizeof(_MCF_thread));
    if(!self)
      return __MCF_nullptr;

    return __MCF_thread_attach_foreign(self);
  }

__MCF_DLLEXPORT
void
_MCF_yield(void)
  {
    NTSTATUS status = NtYieldExecution();
    __MCF_ASSERT_NT(status);
  }

static
BOOL
__stdcall
do_handle_interrupt(DWORD type)
  {
    (void) type;
    uintptr_t old = (uintptr_t) _MCF_atomic_xchg_ptr_rlx(__MCF_g->__sleeping_threads, 0);
    __MCF_batch_release_common(__MCF_g->__sleeping_threads, old / 0x200);
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
    uintptr_t old, new;
    NTSTATUS status;

    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

    /* Set a handler to receive Ctrl-C notifications.  */
    BOOL added __attribute__((__cleanup__(do_handler_cleanup))) = false;
    added = SetConsoleCtrlHandler(do_handle_interrupt, true);

    /* Allocate a count for the current thread. The addend is for backward
     * compatibility, because this used to be an `_MCF_cond`.  */
    _MCF_atomic_xadd_ptr_rlx(__MCF_g->__sleeping_threads, 0x200);

    /* Try waiting.  */
    status = __MCF_keyed_event_wait(__MCF_g->__sleeping_threads, nt_timeout.__li);
    while(status != STATUS_WAIT_0) {
      /* Tell another thread which is going to signal this condition variable
       * that an old waiter has left by decrementing the number of sleeping
       * threads. But see below...  */
      _MCF_atomic_load_pptr_rlx(&old, __MCF_g->__sleeping_threads);
      do {
        if(old == 0)
          break;

        __MCF_ASSERT(old % 0x200 == 0);
        new = old - 0x200;
      }
      while(!_MCF_atomic_cmpxchg_weak_pptr_rlx(__MCF_g->__sleeping_threads, &old, &new));

      if(old != 0)
        return 0;  /* timed out  */

      /* ... It is possible that a second thread has already decremented the
       * counter. If this does take place, it is going to release the keyed
       * event soon. We must still wait, otherwise we get a deadlock in the
       * second thread. However, a third thread could start waiting for this
       * keyed event before us, so we set the timeout to zero. If we time out
       * once more, the third thread will have incremented the number of
       * sleeping threads and we can try decrementing it again.  */
      status = __MCF_keyed_event_wait(__MCF_g->__sleeping_threads, (LARGE_INTEGER[]) { 0 });
    }

    /* We have got interrupted.  */
    return -1;
  }

__MCF_DLLEXPORT
void
_MCF_sleep_noninterruptible(const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

    /* Just sleep.  */
    NTSTATUS status = NtDelayExecution(false, nt_timeout.__li);
    __MCF_ASSERT_NT(status);
  }
