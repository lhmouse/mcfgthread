// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_CXA_C_  1
#include "cxa.h"
#include "once.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "thread.h"

int
__cxa_guard_acquire(int64_t* guard)
  __attribute__((__alias__("__MCF_cxa_guard_acquire")));

void
__cxa_guard_release(int64_t* guard)
  __attribute__((__alias__("__MCF_cxa_guard_release")));

void
__cxa_guard_abort(int64_t* guard)
  __attribute__((__alias__("__MCF_cxa_guard_abort")));

int
__cxa_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  __attribute__((__alias__("__MCF_cxa_atexit")));

int
__cxa_thread_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  __attribute__((__alias__("__MCF_cxa_thread_atexit")));

void
__cxa_finalize(void* dso)
  __attribute__((__alias__("__MCF_cxa_finalize")));

int
__MCF_cxa_guard_acquire(int64_t* guard)
  {
    // Reuse the storage of the guard object as a once flag.
    return _MCF_once_wait_slow((_MCF_once*) guard, NULL);
  }

void
__MCF_cxa_guard_release(int64_t* guard)
  {
    // Reuse the storage of the guard object as a once flag.
    _MCF_once_release((_MCF_once*) guard);
  }

void
__MCF_cxa_guard_abort(int64_t* guard)
  {
    // Reuse the storage of the guard object as a once flag.
    _MCF_once_abort((_MCF_once*) guard);
  }

int
__MCF_cxa_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    // Push the element to the global queue.
    _MCF_mutex_lock(&__MCF_cxa_atexit_mutex, NULL);
    __MCF_dtor_element elem = { (__MCF_dtor_generic*) dtor.__cdecl_ptr, this, dso };
    int err = __MCF_dtor_queue_push(&__MCF_cxa_atexit_queue, &elem);
    _MCF_mutex_unlock(&__MCF_cxa_atexit_mutex);
    return err;
  }

int
__MCF_cxa_thread_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    _MCF_thread* self = _MCF_thread_self();
    if(!self)
      return -1;

    // Push the element to the thread-specific queue.
    __MCF_dtor_element elem = { (__MCF_dtor_generic*) dtor.__cdecl_ptr, this, dso };
    int err = __MCF_dtor_queue_push(&(self->__atexit_queue), &elem);
    return err;
  }

void
__MCF_cxa_finalize(void* dso)
  {
    // Call destructors for thread-local objects before static ones.
    // See ISO/IEC C++ [basic.start.term]/2.
    _MCF_thread* self = _MCF_thread_self();
    if(self)
      __MCF_dtor_queue_finalize(&(self->__atexit_queue), NULL, dso);

    // Call destructors for static objects.
    __MCF_dtor_queue_finalize(&__MCF_cxa_atexit_queue, &__MCF_cxa_atexit_mutex, dso);
  }
