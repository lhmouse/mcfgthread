/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_GTHR_LIBOBJC_
#define __MCFGTHREAD_GTHR_LIBOBJC_

#include "fwd.h"
#include "gthr_aux.h"
#ifdef _LIBOBJC
#  include <objc/thr.h>
#endif

__MCF_CXX(extern "C" {)
#ifndef __MCF_GTHR_LIBOBJC_IMPORT
#  define __MCF_GTHR_LIBOBJC_IMPORT
#  define __MCF_GTHR_LIBOBJC_INLINE  __MCF_GNU_INLINE
#endif

/* Copy this declaration from GCC libobjc here.  */
typedef void* objc_thread_t;

/* Initialize the threads subsystem.  */
__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_init_thread_system(void) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_init_thread_system, __gthread_objc_init_thread_system);
#  define __MCF_gthr_objc_init_thread_system  __gthread_objc_init_thread_system
#endif

/* Close the threads subsystem.  */
__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_close_thread_system(void) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_close_thread_system, __gthread_objc_close_thread_system);
#  define __MCF_gthr_objc_close_thread_system  __gthread_objc_close_thread_system
#endif

/* Below are functions that depend on libobjc types and are thus
 * provided static inline only. Nevertheless, it is still possible to
 * compile these functions by defining `_LIBOBJC`.  */
#ifdef _LIBOBJC

/* Allocate a mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_allocate(objc_mutex_t __objc_mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_mutex_allocate, __gthread_objc_mutex_allocate);
#  define __MCF_gthr_objc_mutex_allocate  __gthread_objc_mutex_allocate
#endif

/* Deallocate a mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_deallocate(objc_mutex_t __objc_mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_mutex_deallocate, __gthread_objc_mutex_deallocate);
#  define __MCF_gthr_objc_mutex_deallocate  __gthread_objc_mutex_deallocate
#endif

/* Grab a lock on a mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_lock(objc_mutex_t __objc_mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_mutex_lock, __gthread_objc_mutex_lock);
#  define __MCF_gthr_objc_mutex_lock  __gthread_objc_mutex_lock
#endif

/* Try to grab a lock on a mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_trylock(objc_mutex_t __objc_mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_mutex_trylock, __gthread_objc_mutex_trylock);
#  define __MCF_gthr_objc_mutex_trylock  __gthread_objc_mutex_trylock
#endif

/* Unlock the mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_unlock(objc_mutex_t __objc_mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_mutex_unlock, __gthread_objc_mutex_unlock);
#  define __MCF_gthr_objc_mutex_unlock  __gthread_objc_mutex_unlock
#endif

/* Allocate a condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_allocate(objc_condition_t __objc_cnd) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_condition_allocate, __gthread_objc_condition_allocate);
#  define __MCF_gthr_objc_condition_allocate  __gthread_objc_condition_allocate
#endif

/* Deallocate a condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_deallocate(objc_condition_t __objc_cnd) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_condition_deallocate, __gthread_objc_condition_deallocate);
#  define __MCF_gthr_objc_condition_deallocate  __gthread_objc_condition_deallocate
#endif

/* Wait on the condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_wait(objc_condition_t __objc_cnd, objc_mutex_t __objc_mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_condition_wait, __gthread_objc_condition_wait);
#  define __MCF_gthr_objc_condition_wait  __gthread_objc_condition_wait
#endif

/* Wake up one thread waiting on this condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_signal(objc_condition_t __objc_cnd) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_condition_signal, __gthread_objc_condition_signal);
#  define __MCF_gthr_objc_condition_signal  __gthread_objc_condition_signal
#endif

/* Wake up all threads waiting on this condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_broadcast(objc_condition_t __objc_cnd) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_condition_broadcast, __gthread_objc_condition_broadcast);
#  define __MCF_gthr_objc_condition_broadcast  __gthread_objc_condition_broadcast
#endif

#endif  /* _LIBOBJC  */

/* Create a new thread of execution.
 * This function creates a detached thread.  */
__MCF_GTHR_LIBOBJC_INLINE
objc_thread_t
__MCF_gthr_objc_thread_detach(__MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_thread_detach, __gthread_objc_thread_detach);
#  define __MCF_gthr_objc_thread_detach  __gthread_objc_thread_detach
#endif

/* Set the current thread's priority.  */
__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_set_priority(int __priority) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_thread_set_priority, __gthread_objc_thread_set_priority);
#  define __MCF_gthr_objc_thread_set_priority  __gthread_objc_thread_set_priority
#endif

/* Return the current thread's priority.  */
__MCF_GTHR_LIBOBJC_INLINE __MCF_FN_PURE
int
__MCF_gthr_objc_thread_get_priority(void) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_thread_get_priority, __gthread_objc_thread_get_priority);
#  define __MCF_gthr_objc_thread_get_priority  __gthread_objc_thread_get_priority
#endif

/* Yield our process time to another thread.  */
__MCF_GTHR_LIBOBJC_INLINE
void
__MCF_gthr_objc_thread_yield(void) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_thread_yield, __gthread_objc_thread_yield);
#  define __MCF_gthr_objc_thread_yield  __gthread_objc_thread_yield
#endif

/* Terminate the current thread.  */
__MCF_GTHR_LIBOBJC_INLINE __MCF_NEVER_RETURN
int
__MCF_gthr_objc_thread_exit(void) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_thread_exit, __gthread_objc_thread_exit);
#  define __MCF_gthr_objc_thread_exit  __gthread_objc_thread_exit
#endif

/* Returns an integer value which uniquely describes a thread.  */
__MCF_GTHR_LIBOBJC_INLINE __MCF_FN_CONST
objc_thread_t
__MCF_gthr_objc_thread_id(void) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_thread_id, __gthread_objc_thread_id);
#  define __MCF_gthr_objc_thread_id  __gthread_objc_thread_id
#endif

/* Returns the thread's local storage pointer.  */
__MCF_GTHR_LIBOBJC_INLINE __MCF_FN_PURE
void*
__MCF_gthr_objc_thread_get_data(void) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_thread_get_data, __gthread_objc_thread_get_data);
#  define __MCF_gthr_objc_thread_get_data  __gthread_objc_thread_get_data
#endif

/* Sets the thread's local storage pointer.  */
__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_set_data(void* __value) __MCF_noexcept;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
__MCF_FNA(__MCF_gthr_objc_thread_set_data, __gthread_objc_thread_set_data);
#  define __MCF_gthr_objc_thread_set_data  __gthread_objc_thread_set_data
#endif

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_init_thread_system(void) __MCF_noexcept
  {
    return 0;
  }

__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_close_thread_system(void) __MCF_noexcept
  {
    return 0;
  }

#ifdef _LIBOBJC

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_allocate(objc_mutex_t __objc_mtx) __MCF_noexcept
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    _MCF_mutex_init(__mtx);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_deallocate(objc_mutex_t __objc_mtx) __MCF_noexcept
  {
    /* libobjc calls this function only when the mutex is being held by
     * the current thread. Proceed anyway without any cleanup.  */
    (void) __objc_mtx;
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_lock(objc_mutex_t __objc_mtx) __MCF_noexcept
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    int __err = _MCF_mutex_lock(__mtx, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_trylock(objc_mutex_t __objc_mtx) __MCF_noexcept
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return __err;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_unlock(objc_mutex_t __objc_mtx) __MCF_noexcept
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_allocate(objc_condition_t __objc_cnd) __MCF_noexcept
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cnd->backend);
    _MCF_cond_init(__cond);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_deallocate(objc_condition_t __objc_cnd) __MCF_noexcept
  {
    (void) __objc_cnd;
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_wait(objc_condition_t __objc_cnd, objc_mutex_t __objc_mtx) __MCF_noexcept
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cnd->backend);
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    int __err = __MCF_gthr_cond_mutex_wait(__cond, __mtx, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_signal(objc_condition_t __objc_cnd) __MCF_noexcept
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cnd->backend);
    _MCF_cond_signal(__cond);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_broadcast(objc_condition_t __objc_cnd) __MCF_noexcept
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cnd->backend);
    _MCF_cond_signal_all(__cond);
    return 0;
  }

#endif  /* _LIBOBJC  */

__MCF_GTHR_LIBOBJC_INLINE
objc_thread_t
__MCF_gthr_objc_thread_detach(__MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_noexcept
  {
    __MCF_gthr_thread_record __rec[1] = __MCF_0_INIT;
    _MCF_thread* __thrd;
    uint32_t __tid;

    __rec->__proc = __proc;
    __rec->__arg = __arg;

    __thrd = _MCF_thread_new(__MCF_gthr_thread_thunk_v2, __rec, sizeof(*__rec));
    if(!__thrd)
      return __MCF_nullptr;

    __tid = __thrd->__tid;
    _MCF_thread_drop_ref_nonnull(__thrd);
    return (objc_thread_t)(uintptr_t) __tid;
  }

__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_set_priority(int __priority) __MCF_noexcept
  {
    int __wprio = (__priority < 0) ? -2 : (__priority - 2);
    int __err = _MCF_thread_set_priority(__MCF_nullptr, (_MCF_thread_priority) __wprio);
    return __err;
  }

__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_get_priority(void) __MCF_noexcept
  {
    int __wprio = (int) _MCF_thread_get_priority(__MCF_nullptr);
    int __priority = (__wprio < -2) ? 0 : (__wprio + 2);
    return __priority;
  }

__MCF_GTHR_LIBOBJC_INLINE
void
__MCF_gthr_objc_thread_yield(void) __MCF_noexcept
  {
    _MCF_yield();
  }

__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_exit(void) __MCF_noexcept
  {
    _MCF_thread_exit();
  }

__MCF_GTHR_LIBOBJC_INLINE
objc_thread_t
__MCF_gthr_objc_thread_id(void) __MCF_noexcept
  {
    return (objc_thread_t)(uintptr_t) _MCF_thread_self_tid();
  }

__MCF_GTHR_LIBOBJC_INLINE
void*
__MCF_gthr_objc_thread_get_data(void) __MCF_noexcept
  {
    _MCF_thread* __self = _MCF_thread_self();
    return __self->__libobjc_tls_data;
  }

__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_set_data(void* __value) __MCF_noexcept
  {
    _MCF_thread* __self = _MCF_thread_self();
    __self->__libobjc_tls_data = __value;
    return 0;
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_GTHR_LIBOBJC_  */
