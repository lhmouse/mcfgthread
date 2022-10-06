/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_CXA_IMPORT  __MCF_DLLEXPORT
#define __MCF_DECLSPEC_CXA_INLINE  __MCF_DLLEXPORT
#include "cxa.h"
#include "once.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "thread.h"
#include "xglobals.i"

__MCF_DLLEXPORT
int
__MCF_cxa_guard_acquire(int64_t* guard)
  {
    /* Reuse the storage of the guard object as a once flag.  */
    return _MCF_once_wait_slow((_MCF_once*) guard, NULL);
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
    _MCF_mutex_lock(&__MCF_cxa_atexit_mutex, NULL);
    __MCF_dtor_element elem = { dtor.__cdecl_ptr, this, dso };
    int err = __MCF_dtor_queue_push(&__MCF_cxa_atexit_queue, &elem);
    _MCF_mutex_unlock(&__MCF_cxa_atexit_mutex);
    return err;
  }

__MCF_DLLEXPORT
int
__MCF_atexit(__MCF_atexit_callback atfn)
  {
    return __MCF_cxa_atexit((__MCF_cxa_dtor_cdecl*)(intptr_t) atfn, NULL, NULL);
  }

__MCF_DLLEXPORT
int
__MCF_cxa_at_quick_exit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    /* Push the element to the global queue.  */
    _MCF_mutex_lock(&__MCF_cxa_at_quick_exit_mutex, NULL);
    __MCF_dtor_element elem = { dtor.__cdecl_ptr, this, dso };
    int err = __MCF_dtor_queue_push(&__MCF_cxa_at_quick_exit_queue, &elem);
    _MCF_mutex_unlock(&__MCF_cxa_at_quick_exit_mutex);
    return err;
  }

__MCF_DLLEXPORT
int
__MCF_at_quick_exit(__MCF_atexit_callback atfn)
  {
    return __MCF_cxa_at_quick_exit((__MCF_cxa_dtor_cdecl*)(intptr_t) atfn, NULL, NULL);
  }

__MCF_DLLEXPORT
int
__MCF_cxa_thread_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    _MCF_thread* self = _MCF_thread_self();
    if(!self)
      return -1;

    /* Push the element to the thread-specific queue.  */
    __MCF_dtor_element elem = { dtor.__cdecl_ptr, this, dso };
    int err = __MCF_dtor_queue_push(self->__atexit_queue, &elem);
    return err;
  }

__MCF_DLLEXPORT
int
__MCF_thread_atexit(__MCF_atexit_callback atfn)
  {
    return __MCF_cxa_thread_atexit((__MCF_cxa_dtor_cdecl*)(intptr_t) atfn, NULL, NULL);
  }

__MCF_DLLEXPORT
void
__MCF_cxa_finalize(void* dso)
  {
    /* A null DSO handle indicates that the current process is terminating.  */
    if(!dso)
      return __MCF_finalize_on_exit();

    /* Remove quick exit callbacks that will expire.  */
    _MCF_mutex_lock(&__MCF_cxa_at_quick_exit_mutex, NULL);
    __MCF_dtor_queue_remove(&__MCF_cxa_at_quick_exit_queue, dso);
    _MCF_mutex_unlock(&__MCF_cxa_at_quick_exit_mutex);

    /* Call destructors for thread-local objects before static ones in
     * accordance with the C++ standard. See [basic.start.term]/2.  */
    _MCF_thread* self = _MCF_thread_self();
    if(self)
      __MCF_dtor_queue_finalize(self->__atexit_queue, NULL, dso);

    /* Call destructors and callbacks registered with `__cxa_atexit()`.  */
    __MCF_dtor_queue_finalize(&__MCF_cxa_atexit_queue, &__MCF_cxa_atexit_mutex, dso);
  }
