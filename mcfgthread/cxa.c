/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_CXA_IMPORT  __MCF_DLLEXPORT
#define __MCF_CXA_INLINE  __MCF_DLLEXPORT
#include "cxa.h"
#include "once.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "thread.h"
#include "xglobals.h"

__MCF_DLLEXPORT
int
__MCF_cxa_guard_acquire(int64_t* guard)
  {
    /* Reuse the storage of the guard object as a once flag.  */
    return _MCF_once_wait_slow((_MCF_once*) guard, __MCF_nullptr);
  }

__MCF_DLLEXPORT
void
__MCF_cxa_guard_release(int64_t* guard)
  {
    /* Reuse the storage of the guard object as a once flag.  */
    _MCF_once_release((_MCF_once*) guard);
  }

__MCF_DLLEXPORT
void
__MCF_cxa_guard_abort(int64_t* guard)
  {
    /* Reuse the storage of the guard object as a once flag.  */
    _MCF_once_abort((_MCF_once*) guard);
  }

__MCF_DLLEXPORT
int
__MCF_cxa_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    /* Push the element to the global queue.  */
    _MCF_mutex_lock(__MCF_g->__exit_mtx, __MCF_nullptr);
    __MCF_dtor_element elem = { dtor, this, dso };
    int err = __MCF_dtor_queue_push(__MCF_g->__exit_queue, &elem);
    _MCF_mutex_unlock(__MCF_g->__exit_mtx);
    return err;
  }

__MCF_DLLEXPORT
int
__MCF_atexit(__MCF_atexit_callback* func)
  {
    return __MCF_cxa_atexit(func, __MCF_nullptr, __MCF_nullptr);
  }

__MCF_DLLEXPORT
int
__MCF_cxa_at_quick_exit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    /* Push the element to the global queue.  */
    _MCF_mutex_lock(__MCF_g->__quick_exit_mtx, __MCF_nullptr);
    __MCF_dtor_element elem = { dtor, this, dso };
    int err = __MCF_dtor_queue_push(__MCF_g->__quick_exit_queue, &elem);
    _MCF_mutex_unlock(__MCF_g->__quick_exit_mtx);
    return err;
  }

__MCF_DLLEXPORT
int
__MCF_at_quick_exit(__MCF_atexit_callback* func)
  {
    return __MCF_cxa_at_quick_exit(func, __MCF_nullptr, __MCF_nullptr);
  }

__MCF_DLLEXPORT
int
__MCF_cxa_thread_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    /* Push the element to the thread-specific queue.  */
    _MCF_thread* self = _MCF_thread_self();
    __MCF_dtor_element elem = { dtor, this, dso };
    int err = __MCF_dtor_queue_push(self->__atexit_queue, &elem);
    return err;
  }

__MCF_DLLEXPORT
int
__MCF_thread_atexit(__MCF_atexit_callback* func)
  {
    return __MCF_cxa_thread_atexit(func, __MCF_nullptr, __MCF_nullptr);
  }

static
void
do_thread_dtor_queue_finalize(void* dso)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    __MCF_dtor_element elem;

    _MCF_thread* self = __MCF_crt_TlsGetValue(__MCF_g->__tls_index);
    if(!self)
      return;

    while(__MCF_dtor_queue_pop(&elem, self->__atexit_queue, dso) == 0)
      __MCF_invoke_cxa_dtor(elem.__dtor, elem.__this);
  }

__MCF_DLLEXPORT
void
__MCF_cxa_finalize(void* dso)
  {
    /* A null DSO handle indicates that the current process is terminating. A
     * non-null DSO handle indicates that a dynamic library is being unloaded.
     * In either case, destructors for thread-local objects shall be called
     * before static ones, in accordance with the ISO C++ standard. (See
     * [basic.start.term]/2.) Destructors of thread-local keys are not called,
     * according to POSIX.  */
    do_thread_dtor_queue_finalize(dso);
    __MCF_run_static_dtors(__MCF_g->__exit_mtx, __MCF_g->__exit_queue, dso);

    /* Remove quick exit callbacks that will expire.  */
    _MCF_mutex_lock(__MCF_g->__quick_exit_mtx, __MCF_nullptr);
    __MCF_dtor_queue_remove(__MCF_g->__quick_exit_queue, dso);
    _MCF_mutex_unlock(__MCF_g->__quick_exit_mtx);
  }
