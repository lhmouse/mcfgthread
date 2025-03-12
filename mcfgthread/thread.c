/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_THREAD_IMPORT  __MCF_DLLEXPORT
#define __MCF_THREAD_INLINE  __MCF_DLLEXPORT
#include "thread.h"
#include "event.h"
#include "xglobals.h"

enum initialization_status
  {
    initialization_null       = 0,
    initialization_running    = 1,
    initialization_succeeded  = 2,
    initialization_orphaned   = 3,
  };

struct thread_initializer
  {
    _MCF_event status[1];
    _MCF_thread* thrd;
    ULONG win32_error;
  };

static __MCF_REALIGN_SP
ULONG
__stdcall
do_win32_thread_thunk(LPVOID param)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    struct thread_initializer* init = param;
    _MCF_event_await_change(init->status, initialization_null, __MCF_nullptr);
    _MCF_thread* thrd = init->thrd;

    /* Attach the thread.  */
    if(!TlsSetValue(__MCF_g->__tls_index, thrd)) {
      init->win32_error = GetLastError();
      _MCF_event_set(init->status, initialization_orphaned);
      return 0;
    }

    /* Let the creator go, which invalidates `*init`.  */
    __MCF_ASSERT(thrd->__tid == _MCF_thread_self_tid());
    _MCF_event_set(init->status, initialization_succeeded);
    init = __MCF_BAD_PTR;

#if defined __i386__ || (defined __amd64__ && !defined __arm64ec__)
    /* Set x87 precision to 64-bit mantissa (GNU `long double` format).  */
    __asm__ volatile ("fninit");
#endif

    (* thrd->__proc) (thrd);
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

    if(align > __MCF_THREAD_MAX_DATA_ALIGNMENT)
      return __MCF_win32_error_p(ERROR_NOT_SUPPORTED, __MCF_nullptr);

    if(size > (INT32_MAX & -__MCF_THREAD_MAX_DATA_ALIGNMENT))
      return __MCF_win32_error_p(ERROR_ARITHMETIC_OVERFLOW, __MCF_nullptr);

    /* Allocate and initialize the thread control structure.  */
    struct thread_initializer init;
    _MCF_event_init(init.status, initialization_null);

    size_t real_alignment = _MCF_maxz(__MCF_THREAD_DATA_ALIGNMENT, align);
    size_t size_need = sizeof(_MCF_thread) + size;
    size_t size_request = size_need + real_alignment - MEMORY_ALLOCATION_ALIGNMENT;
    __MCF_ASSERT(size_need <= size_request);

    init.thrd = __MCF_malloc_0(size_request);
    if(!init.thrd)
      return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, __MCF_nullptr);

    _MCF_atomic_store_32_rlx(init.thrd->__nref, 2);
    init.thrd->__proc = proc;

    if(size != 0) {
      init.thrd->__data_opt = init.thrd->__data_storage;

      /* Adjust `__data_opt` for over-aligned types. If we have over-allocated
       * memory, give back some. Errors are ignored.  */
      if(size_need != size_request) {
        init.thrd->__data_opt = (void*) ((((uintptr_t) init.thrd->__data_opt - 1) | (real_alignment - 1)) + 1);

        size_request = (uintptr_t) init.thrd->__data_opt + size - (uintptr_t) init.thrd;
        __MCF_ASSERT(size_need <= size_request);
        __MCF_mresize_0(init.thrd, size_request);
      }

      /* Copy user-defined data. If this doesn't happen, they are implicit zeroes.  */
      if(data_opt)
        __MCF_mcopy(init.thrd->__data_opt, data_opt, size);
    }

    /* Create a thread and wait for its initialization to finish.  */
    init.thrd->__handle = CreateThread(__MCF_nullptr, 0, do_win32_thread_thunk, &init, 0, (ULONG*) &(init.thrd->__tid));
    if(init.thrd->__handle == NULL) {
      __MCF_mfree_nonnull(init.thrd);
      return __MCF_nullptr;
    }

    _MCF_event_set(init.status, initialization_running);
    int result = _MCF_event_await_change_slow(init.status, initialization_running, __MCF_nullptr);
    if(result == initialization_orphaned) {
      __MCF_close_handle(init.thrd->__handle);
      __MCF_mfree_nonnull(init.thrd);
      return __MCF_win32_error_p(init.win32_error, __MCF_nullptr);
    }

    /* Return the initialized thread.  */
    __MCF_ASSERT(result == initialization_succeeded);
    return init.thrd;
  }

__MCF_DLLEXPORT
_MCF_thread*
__MCF_thread_attach_foreign(_MCF_thread* thrd)
  {
    __MCF_ASSERT(thrd->__nref[0] == 0);
    __MCF_ASSERT(thrd->__tid == 0);
    __MCF_ASSERT(thrd->__handle == __MCF_nullptr);

    /* Ensure the thread has not been attached.  */
    __MCF_CHECK(__MCF_crt_TlsGetValue(__MCF_g->__tls_index) == __MCF_nullptr);

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
    __MCF_mfree_nonnull(thrd);
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

static __MCF_NEVER_INLINE
_MCF_thread*
do_thread_self_slow(void)
  {
    _MCF_thread* self = __MCF_crt_TlsGetValue(__MCF_g->__tls_index);
    if(self)
      return self;

    /* Allocate a new thread object with no user-defined data. When out of memory,
     * use the pre-allocated backup. If it is in use, this thread shall block
     * until the other thread terminates.  */
    self = __MCF_malloc_0(sizeof(_MCF_thread));
    if(!self) {
      self = __MCF_G_FIELD_OPT(__thread_oom_self_st);
      __MCF_CHECK(self);
      _MCF_mutex_lock(__MCF_g->__thread_oom_mtx, __MCF_nullptr);
    }

    /* Attach the new structure. This will be deallocated in
     * `_MCF_thread_drop_ref_nonnull()`.  */
    return __MCF_thread_attach_foreign(self);
  }

__MCF_DLLEXPORT
_MCF_thread*
_MCF_thread_self(void)
  {
    if(__MCF_g->__tls_index < 64) {
      _MCF_thread* self;
      __MCF_TEB_LOAD_PTR_INDEXED(&self, __MCF_64_32(0x1480, 0x0E10), __MCF_g->__tls_index);
      if(self)
        return self;
    }

    /* Go on the slow path with a tail call.  */
    return do_thread_self_slow();
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
do_sleep_interrupt(ULONG type)
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
