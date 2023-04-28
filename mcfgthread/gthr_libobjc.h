/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_GTHR_LIBOBJC_
#define __MCFGTHREAD_GTHR_LIBOBJC_

#include "fwd.h"
#include "gthr_aux.h"
#ifdef _LIBOBJC
#  include <objc/thr.h>
#endif

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_GTHR_LIBOBJC_IMPORT
#  define __MCF_GTHR_LIBOBJC_IMPORT
#  define __MCF_GTHR_LIBOBJC_INLINE  __MCF_GNU_INLINE
#endif

/* Copy this declaration from GCC libobjc here.  */
typedef void* objc_thread_t;

/* Initialize the threads subsystem.  */
__MCF_GTHR_LIBOBJC_INLINE __MCF_CXX11(constexpr)
int
__MCF_gthr_objc_init_thread_system(void) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_init_thread_system  __MCF_gthr_objc_init_thread_system
#endif

/* Close the threads subsystem.  */
__MCF_GTHR_LIBOBJC_INLINE __MCF_CXX11(constexpr)
int
__MCF_gthr_objc_close_thread_system(void) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_close_thread_system  __MCF_gthr_objc_close_thread_system
#endif

/* Below are functions that depend on libobjc types and are thus
 * provided static inline only. Nevertheless, it is still possible to
 * compile these functions by defining `_LIBOBJC`.  */
#ifdef _LIBOBJC

/* Allocate a mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_allocate(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_mutex_allocate  __MCF_gthr_objc_mutex_allocate
#endif

/* Deallocate a mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_deallocate(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_mutex_deallocate  __MCF_gthr_objc_mutex_deallocate
#endif

/* Grab a lock on a mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_lock(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_mutex_lock  __MCF_gthr_objc_mutex_lock
#endif

/* Try to grab a lock on a mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_trylock(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_mutex_trylock  __MCF_gthr_objc_mutex_trylock
#endif

/* Unlock the mutex.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_unlock(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_mutex_unlock  __MCF_gthr_objc_mutex_unlock
#endif

/* Allocate a condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_allocate(objc_condition_t __objc_cnd) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_condition_allocate  __MCF_gthr_objc_condition_allocate
#endif

/* Deallocate a condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_deallocate(objc_condition_t __objc_cnd) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_condition_deallocate  __MCF_gthr_objc_condition_deallocate
#endif

/* Wait on the condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_wait(objc_condition_t __objc_cnd, objc_mutex_t __objc_mtx) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_condition_wait  __MCF_gthr_objc_condition_wait
#endif

/* Wake up one thread waiting on this condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_signal(objc_condition_t __objc_cnd) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_condition_signal  __MCF_gthr_objc_condition_signal
#endif

/* Wake up all threads waiting on this condition.  */
__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_broadcast(objc_condition_t __objc_cnd) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_condition_broadcast  __MCF_gthr_objc_condition_broadcast
#endif

#endif  /* _LIBOBJC  */

/* Create a new thread of execution.
 * This function creates a detached thread.  */
__MCF_GTHR_LIBOBJC_INLINE
objc_thread_t
__MCF_gthr_objc_thread_detach(__MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_thread_detach  __MCF_gthr_objc_thread_detach
#endif

/* Set the current thread's priority.  */
__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_set_priority(int __priority) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_thread_set_priority  __MCF_gthr_objc_thread_set_priority
#endif

/* Return the current thread's priority.  */
__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_get_priority(void) __MCF_NOEXCEPT __attribute__((__pure__));

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_thread_get_priority  __MCF_gthr_objc_thread_get_priority
#endif

/* Yield our process time to another thread.  */
__MCF_GTHR_LIBOBJC_INLINE
void
__MCF_gthr_objc_thread_yield(void) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_thread_yield  __MCF_gthr_objc_thread_yield
#endif

/* Terminate the current thread.  */
__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_exit(void) __MCF_NOEXCEPT __attribute__((__noreturn__));

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_thread_exit  __MCF_gthr_objc_thread_exit
#endif

/* Returns an integer value which uniquely describes a thread.  */
__MCF_GTHR_LIBOBJC_INLINE
objc_thread_t
__MCF_gthr_objc_thread_id(void) __MCF_NOEXCEPT __attribute__((__const__));

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_thread_id  __MCF_gthr_objc_thread_id
#endif

/* Returns the thread's local storage pointer.  */
__MCF_GTHR_LIBOBJC_INLINE
void*
__MCF_gthr_objc_thread_get_data(void) __MCF_NOEXCEPT __attribute__((__pure__));

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_thread_get_data  __MCF_gthr_objc_thread_get_data
#endif

/* Sets the thread's local storage pointer.  */
__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_set_data(void* __value) __MCF_NOEXCEPT;

#ifndef __MCF_GTHR_LIBOBJC_NO_ALIASES
#  define __gthread_objc_thread_set_data  __MCF_gthr_objc_thread_set_data
#endif

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_GTHR_LIBOBJC_INLINE __MCF_CXX11(constexpr)
int
__MCF_gthr_objc_init_thread_system(void) __MCF_NOEXCEPT
  {
    return 0;
  }

__MCF_GTHR_LIBOBJC_INLINE __MCF_CXX11(constexpr)
int
__MCF_gthr_objc_close_thread_system(void) __MCF_NOEXCEPT
  {
    return 0;
  }

#ifdef _LIBOBJC

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_allocate(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    _MCF_mutex_init(__mtx);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_deallocate(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT
  {
    /* libobjc calls this function only when the mutex is being held by
     * the current thread. Proceed anyway without any cleanup.  */
    (void) __objc_mtx;
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_lock(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    int __err = _MCF_mutex_lock(__mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_trylock(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return __err;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_mutex_unlock(objc_mutex_t __objc_mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_allocate(objc_condition_t __objc_cnd) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cnd->backend);
    _MCF_cond_init(__cond);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_deallocate(objc_condition_t __objc_cnd) __MCF_NOEXCEPT
  {
    (void) __objc_cnd;
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_wait(objc_condition_t __objc_cnd, objc_mutex_t __objc_mtx) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cnd->backend);
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mtx->backend);
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_signal(objc_condition_t __objc_cnd) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cnd->backend);
    _MCF_cond_signal(__cond);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__MCF_gthr_objc_condition_broadcast(objc_condition_t __objc_cnd) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cnd->backend);
    _MCF_cond_signal_all(__cond);
    return 0;
  }

#endif  /* _LIBOBJC  */

__MCF_GTHR_LIBOBJC_INLINE
objc_thread_t
__MCF_gthr_objc_thread_detach(__MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record __rec[1] = __MCF_0_INIT;
    _MCF_thread* __thrd;
    uint32_t __tid;

    __rec->__proc = __proc;
    __rec->__arg = __arg;

    __thrd = _MCF_thread_new(__MCF_gthr_thread_thunk_v2, __rec, sizeof(*__rec));
    if(!__thrd)
      return NULL;

    __tid = __thrd->__tid;
    _MCF_thread_drop_ref_nonnull(__thrd);
    return (objc_thread_t)(uintptr_t) __tid;
  }

__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_set_priority(int __priority) __MCF_NOEXCEPT
  {
    int __wprio = (__priority < 0) ? -2 : (__priority - 2);
    int __err = _MCF_thread_set_priority(NULL, (_MCF_thread_priority) __wprio);
    return __err;
  }

__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_get_priority(void) __MCF_NOEXCEPT
  {
    int __wprio = (int) _MCF_thread_get_priority(NULL);
    int __priority = (__wprio < -2) ? 0 : (__wprio + 2);
    return __priority;
  }

__MCF_GTHR_LIBOBJC_INLINE
void
__MCF_gthr_objc_thread_yield(void) __MCF_NOEXCEPT
  {
    _MCF_yield();
  }

__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_exit(void) __MCF_NOEXCEPT
  {
    _MCF_thread_exit();
  }

__MCF_GTHR_LIBOBJC_INLINE
objc_thread_t
__MCF_gthr_objc_thread_id(void) __MCF_NOEXCEPT
  {
    return (objc_thread_t)(uintptr_t) _MCF_thread_self_tid();
  }

__MCF_GTHR_LIBOBJC_INLINE
void*
__MCF_gthr_objc_thread_get_data(void) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    if(!__self)
      return NULL;

    return __self->__libobjc_tls_data;
  }

__MCF_GTHR_LIBOBJC_INLINE
int
__MCF_gthr_objc_thread_set_data(void* __value) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    if(!__self)
      return -1;

    __self->__libobjc_tls_data = __value;
    return 0;
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_GTHR_LIBOBJC_  */
