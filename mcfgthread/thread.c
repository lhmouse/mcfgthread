/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_THREAD_IMPORT  __MCF_DLLEXPORT
#define __MCF_THREAD_INLINE  __MCF_DLLEXPORT
#include "thread.h"
#include "xglobals.h"

static __MCF_REALIGN_SP
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

    if(align >= 0x100000U)
      return __MCF_win32_error_p(ERROR_NOT_SUPPORTED, __MCF_nullptr);

    if(size >= 0x7FF00000U)
      return __MCF_win32_error_p(ERROR_ARITHMETIC_OVERFLOW, __MCF_nullptr);

    /* Calculate the number of bytes to allocate.  */
    size_t real_alignment = _MCF_maxz(__MCF_THREAD_DATA_ALIGNMENT, align);
    size_t size_need = sizeof(_MCF_thread) + size;
    size_t size_request = size_need + real_alignment - MEMORY_ALLOCATION_ALIGNMENT;
    __MCF_ASSERT(size_need <= size_request);

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
        __MCF_ASSERT(size_need <= size_request);
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

    /* Ensure the thread has not been attached.  */
    __MCF_CHECK(TlsGetValue(__MCF_g->__tls_index) == __MCF_nullptr);

    /* Initialize thread identity fields.  */
    thrd->__tid = _MCF_thread_self_tid();
    thrd->__handle = __MCF_duplicate_handle(GetCurrentThread());
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

    /* If this is the backup one, clear it for reuse.  */
    _MCF_thread* oom_self = __MCF_G_FIELD_OPT(__thread_oom_self_st);
    if(thrd == oom_self) {
      __MCF_close_handle(thrd->__handle);
      __MCF_mzero(thrd, sizeof(_MCF_thread));
      _MCF_mutex_unlock(__MCF_g->__thread_oom_mtx);
      return;
    }

    /* Deallocate all associated resources.  */
    __MCF_close_handle(thrd->__handle);
    __MCF_mfree(thrd);
  }

__MCF_DLLEXPORT
void
_MCF_thread_exit(void)
  {
    ExitThread(0);
    __builtin_unreachable();
  }

__MCF_DLLEXPORT
int
_MCF_thread_wait(const _MCF_thread* thrd_opt, const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);

    /* Translate the thread handle for the sake of consistency. It is probably
     * not useful to pass a null thread pointer, as the operation will time out
     * or deadlock anyway.  */
    HANDLE handle = thrd_opt ? thrd_opt->__handle : GetCurrentThread();
    return __MCF_wait_for_single_object(handle, &nt_timeout);
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
    BOOL succ = SetThreadPriority(handle, priority);
    return !succ ? -1 : 0;
  }

__MCF_DLLEXPORT
_MCF_thread*
_MCF_thread_self(void)
  {
    _MCF_thread* self;
    const uintptr_t tls_index = __MCF_g->__tls_index;
    if(tls_index < 64) {
#if defined __amd64__
      __asm__ ("mov %0, gs:[0x1480 + %1 * 8]" : "=r"(self) : "r"(tls_index));
#elif defined __i386__
      __asm__ ("mov %0, fs:[0xE10 + %1 * 4]" : "=r"(self) : "r"(tls_index));
#elif defined __aarch64__
      __asm__ ("ldr %0, [x18, %1, lsl 3]" : "=r"(self) : "r"(0x1480 / 8 + tls_index));
#else
#  error Unsupported architecture
#endif
      if(self)
        return self;
    }

    self = TlsGetValue(__MCF_g->__tls_index);
    if(self)
      return self;

    /* Allocate a new thread object with no user-defined data. When out of memory,
     * use the pre-allocated backup. */
    self = __MCF_malloc_0(sizeof(_MCF_thread));
    if(!self) {
      self = __MCF_G_FIELD_OPT(__thread_oom_self_st);
      __MCF_CHECK(self);

      /* If the backup is in use, this thread shall block until the other thread
       * terminates.  */
      _MCF_mutex_lock(__MCF_g->__thread_oom_mtx, __MCF_nullptr);
    }

    return __MCF_thread_attach_foreign(self);
  }

__MCF_DLLEXPORT
void
_MCF_yield(void)
  {
    SwitchToThread();
  }

static __MCF_REALIGN_SP
BOOL
__stdcall
do_sleep_interrupt(DWORD type)
  {
    (void) type;

    /* Notify all threads that are sleeping.  */
    _MCF_cond_signal_all(__MCF_g->__interrupt_cond);
    return false;
  }

static
intptr_t
do_sleep_unlock(intptr_t arg)
  {
    (void) arg;

    /* Add a Ctrl-C handler. The return value indicates whether it has been
     * added successfully.  */
    return SetConsoleCtrlHandler(do_sleep_interrupt, true);
  }

static
void
do_sleep_relock(intptr_t arg, intptr_t added)
  {
    (void) arg;

    /* If a Ctrl-C handler has been added, remove it now.  */
    if(added != 0)
      SetConsoleCtrlHandler(do_sleep_interrupt, false);
  }

__MCF_DLLEXPORT
int
_MCF_sleep(const int64_t* timeout_opt)
  {
    int err = _MCF_cond_wait(__MCF_g->__interrupt_cond, do_sleep_unlock, do_sleep_relock, 0, timeout_opt);
    return err ^ -1;
  }

__MCF_DLLEXPORT
void
_MCF_sleep_noninterruptible(const int64_t* timeout_opt)
  {
    __MCF_winnt_timeout nt_timeout;
    __MCF_initialize_winnt_timeout_v3(&nt_timeout, timeout_opt);
    __MCF_sleep(&nt_timeout);
  }
