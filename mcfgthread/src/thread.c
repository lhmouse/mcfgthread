/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_THREAD_IMPORT  __MCF_DLLEXPORT
#define __MCF_THREAD_INLINE  __MCF_DLLEXPORT
#include "../thread.h"
#include "xglobals.h"

static __MCF_REALIGN_SP
void
do_thread_startup(_MCF_thread* thrd)
  {
    __MCF_USING_SEH_TERMINUS;
    thrd->__tid = __MCF_tid();
    __MCF_CHECK(TlsSetValue(__MCF_G(tls_index), thrd));
#if defined __MCF_M_X86_ASM
    /* Set x87 precision to 64-bit mantissa (GNU `long double` format).  */
    __asm__ volatile ("fninit");
#endif
    (* thrd->__proc) (thrd);
  }

static
ULONG
__stdcall
do_win32_thread_routine(LPVOID param)
  {
    do_thread_startup(param);
    __MCF_gthread_on_thread_exit();
    return 0;
  }

__MCF_DLLEXPORT
_MCF_thread*
_MCF_thread_new_aligned(_MCF_thread_procedure* proc, size_t align, const void* data_opt,
                        size_t size)
  {
    if(!proc)
      return __MCF_win32_error_p(ERROR_INVALID_PARAMETER, nullptr);

    if(align & (align - 1))  /* power of 2, or 0 */
      return __MCF_win32_error_p(ERROR_INVALID_PARAMETER, nullptr);
    else if(align > __MCF_THREAD_MAX_DATA_ALIGNMENT)
      return __MCF_win32_error_p(ERROR_NOT_SUPPORTED, nullptr);

    if(size > 0x8000000U - __MCF_THREAD_MAX_DATA_ALIGNMENT)
      return __MCF_win32_error_p(ERROR_ARITHMETIC_OVERFLOW, nullptr);

    /* Calculate the number of bytes to allocate for the thread control structure
     * and user-defined data. If the user-defined data should be over-aligned,
     * over-allocate some, which will be given back later.  */
    size_t size_need = sizeof(__MCF_thread_base) + size;
    size_t real_align = 0;
    size_t size_request = size_need;

    if(size != 0) {
      __MCF_ASSERT(MEMORY_ALLOCATION_ALIGNMENT <= __MCF_THREAD_DATA_ALIGNMENT);
      real_align = _MCF_maxz(__MCF_THREAD_DATA_ALIGNMENT, align);
      size_request = size_need + real_align - MEMORY_ALLOCATION_ALIGNMENT;
      __MCF_ASSERT(size_need <= size_request);
    }

    /* Allocate and initialize the thread control structure.  */
    _MCF_thread* thrd = __MCF_malloc_0(size_request);
    if(!thrd)
      return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, nullptr);

    if(size != 0) {
      uintptr_t data_addr = (uintptr_t) thrd + sizeof(__MCF_thread_base);
      thrd->__data_opt = (void*) data_addr;
      if(size_need != size_request) {
        /* Round `__data_opt` for over-aligned types.  */
        data_addr --;
        data_addr |= real_align - 1;
        data_addr ++;
        __MCF_ASSERT(data_addr % real_align == 0);
        thrd->__data_opt = (void*) data_addr;

        /* If we have over-allocated memory, give back some. Errors are ignored.  */
        size_request = data_addr + size - (uintptr_t) thrd;
        __MCF_ASSERT(size_need <= size_request);
        if(size_request != size_need)
          __MCF_mresize_0(thrd, size_request);
      }

      /* Copy user-defined data. If this doesn't happen, they are implicit zeroes.  */
      if(data_opt)
        __MCF_mcopy(thrd->__data_opt, data_opt, size);
    }

    /* Create a thread and wait for its initialization to finish.  */
    _MCF_atomic_store_32_rlx(thrd->__nref, 2);
    thrd->__proc = proc;
    thrd->__handle = CreateThread(nullptr, 0, do_win32_thread_routine, thrd, 0,
                                  (DWORD*) &(thrd->__tid));
    if(!thrd->__handle) {
      __MCF_mfree_nonnull(thrd);
      return nullptr;
    }

    /* Return the initialized thread.  */
    return thrd;
  }

__MCF_DLLEXPORT
_MCF_thread*
__MCF_thread_attach_foreign(_MCF_thread* thrd)
  {
    __MCF_ASSERT(thrd->__nref[0] == 0);
    __MCF_ASSERT(thrd->__tid == 0);
    __MCF_ASSERT(thrd->__handle == NULL);

    /* Initialize thread identity fields.  */
    thrd->__tid = __MCF_tid();
    thrd->__handle = __MCF_duplicate_handle(NtCurrentThread());
    __MCF_CHECK(thrd->__handle);

    /* Initialize the reference count to detached state.  */
    _MCF_atomic_store_32_rel(thrd->__nref, 1);

    /* Attach the thread now.  */
    __MCF_CHECK(TlsSetValue(__MCF_G(tls_index), thrd));
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
    if(thrd == __MCF_G(main_thread))
      return;

    /* Detach the thread structure. The thread-local object table and the exit
     * callback queue shall have been cleared upon termination of the associated
     * thread, so they are empty now.  */
    __MCF_close_handle(thrd->__handle);
    __MCF_ASSERT(thrd->__atexit_queue[0].__prev == nullptr);
    __MCF_ASSERT(thrd->__tls_table[0].__begin == nullptr);

    if(thrd == __MCF_G_OPT(thread_oom_self_st)) {
      /* If this is the OOM backup, clear it for reuse.  */
      __MCF_mzero(thrd, sizeof(__MCF_thread_base));
      _MCF_mutex_unlock_slow(__MCF_G(thread_oom_mtx));
    } else
      __MCF_mfree_nonnull(thrd);
  }

__MCF_DLLEXPORT
void
_MCF_thread_exit(void)
  {
    __MCF_gthread_on_thread_exit();
    ExitThread(0);
    __MCF_UNREACHABLE;
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
    HANDLE handle = thrd_opt ? thrd_opt->__handle : NtCurrentThread();
    return __MCF_wait_for_single_object(handle, &nt_timeout);
  }

__MCF_DLLEXPORT
_MCF_thread_priority
_MCF_thread_get_priority(const _MCF_thread* thrd_opt)
  {
    HANDLE handle = thrd_opt ? thrd_opt->__handle : NtCurrentThread();
    return (_MCF_thread_priority) GetThreadPriority(handle);
  }

__MCF_DLLEXPORT
int
_MCF_thread_set_priority(_MCF_thread* thrd_opt, _MCF_thread_priority priority)
  {
    HANDLE handle = thrd_opt ? thrd_opt->__handle : NtCurrentThread();
    BOOL succ = SetThreadPriority(handle, priority);
    return !succ ? -1 : 0;
  }

static __MCF_NEVER_INLINE
_MCF_thread*
do_thread_self_slow(void)
  {
    _MCF_thread* self = __MCF_crt_TlsGetValue(__MCF_G(tls_index));
    if(self)
      return self;

    self = __MCF_malloc_0(sizeof(__MCF_thread_base));
    if(!self) {
      /* When out of memory, use the static backup. If it is in use, this
       * thread shall block until the other thread frees it.  */
      self = __MCF_G_OPT(thread_oom_self_st);
      __MCF_CHECK(self);
      _MCF_mutex_lock_slow(__MCF_G(thread_oom_mtx), nullptr);
      __MCF_ASSERT(self->__handle == NULL);
    }

    /* Attach the new thread structure. This will be deallocated in
     * `_MCF_thread_drop_ref_nonnull()`.  */
    return __MCF_thread_attach_foreign(self);
  }

__MCF_DLLEXPORT __MCF_FN_CONST
_MCF_thread*
_MCF_thread_self(void)
  {
    uintptr_t tls_index = __MCF_G(tls_index);
    if(tls_index < 64) {
      uintptr_t tls_addr = __MCF_64_32(0x1480, 0x0E10) + tls_index * sizeof(void*);
      _MCF_thread* self = (void*) __MCF_teb_load_ptr((uint32_t) tls_addr);
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

static
BOOL
__stdcall
do_sleep_interrupt(ULONG type)
  {
    (void) type;

    /* Notify all threads that are sleeping.  */
    _MCF_cond_signal_some_slow(__MCF_G(interrupt_cond), SIZE_MAX);
    return false;
  }

static
intptr_t
do_register_ctrl_c_handler(intptr_t arg)
  {
    (void) arg;

    /* Add a Ctrl-C handler. The return value indicates whether it has been
     * added successfully.  */
    return SetConsoleCtrlHandler(do_sleep_interrupt, true);
  }

static
void
do_remove_ctrl_c_handler(intptr_t arg, intptr_t added)
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
    int err = _MCF_cond_wait(__MCF_G(interrupt_cond), do_register_ctrl_c_handler,
                             do_remove_ctrl_c_handler, 0, timeout_opt);
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
