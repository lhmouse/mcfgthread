/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_GTHR_LIBOBJC_
#define __MCFGTHREAD_GTHR_LIBOBJC_

#include "fwd.h"
#include "gthr_aux.h"
#ifdef _LIBOBJC
#  include <objc/thr.h>
#endif

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_DECLSPEC_GTHR_LIBOBJC_IMPORT
#  define __MCF_DECLSPEC_GTHR_LIBOBJC_IMPORT
#  define __MCF_DECLSPEC_GTHR_LIBOBJC_INLINE  __MCF_GNU_INLINE
#endif

/* Copy this declaration from GCC libobjc here.  */
typedef void* objc_thread_t;

/* Initialize the threads subsystem.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE __MCF_CXX11(constexpr)
int
__gthread_objc_init_thread_system(void) __MCF_NOEXCEPT;

/* Close the threads subsystem.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE __MCF_CXX11(constexpr)
int
__gthread_objc_close_thread_system(void) __MCF_NOEXCEPT;

/* Below are functions that depend on libobjc types and are thus
 * provided static inline only. Nevertheless, it is still possible to
 * compile these functions by defining `_LIBOBJC`.  */
#ifdef _LIBOBJC

/* Allocate a mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_allocate(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT;

/* Deallocate a mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_deallocate(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT;

/* Grab a lock on a mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_lock(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT;

/* Try to grab a lock on a mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_trylock(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT;

/* Unlock the mutex.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_unlock(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT;

/* Allocate a condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_allocate(objc_condition_t __objc_cond) __MCF_NOEXCEPT;

/* Deallocate a condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_deallocate(objc_condition_t __objc_cond) __MCF_NOEXCEPT;

/* Wait on the condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_wait(objc_condition_t __objc_cond, objc_mutex_t __objc_mutex) __MCF_NOEXCEPT;

/* Wake up all threads waiting on this condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_broadcast(objc_condition_t __objc_cond) __MCF_NOEXCEPT;

/* Wake up one thread waiting on this condition.  */
__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_signal(objc_condition_t __objc_cond) __MCF_NOEXCEPT;

#endif  /* _LIBOBJC  */

/* Create a new thread of execution.
 * This function creates a detached thread.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
objc_thread_t
__gthread_objc_thread_detach(__MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT;

/* Set the current thread's priority.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
int
__gthread_objc_thread_set_priority(int __priority) __MCF_NOEXCEPT;

/* Return the current thread's priority.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
int
__gthread_objc_thread_get_priority(void) __MCF_NOEXCEPT
  __attribute__((__pure__));

/* Yield our process time to another thread.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
void
__gthread_objc_thread_yield(void) __MCF_NOEXCEPT;

/* Terminate the current thread.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
int
__gthread_objc_thread_exit(void) __MCF_NOEXCEPT
  __attribute__((__noreturn__));

/* Returns an integer value which uniquely describes a thread.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
objc_thread_t
__gthread_objc_thread_id(void) __MCF_NOEXCEPT
  __attribute__((__const__));

/* Returns the thread's local storage pointer.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
void*
__gthread_objc_thread_get_data(void) __MCF_NOEXCEPT
  __attribute__((__pure__));

/* Sets the thread's local storage pointer.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
int
__gthread_objc_thread_set_data(void* __value) __MCF_NOEXCEPT;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE __MCF_CXX11(constexpr)
int
__gthread_objc_init_thread_system(void) __MCF_NOEXCEPT
  {
    return 0;
  }

__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE __MCF_CXX11(constexpr)
int
__gthread_objc_close_thread_system(void) __MCF_NOEXCEPT
  {
    return 0;
  }

#ifdef _LIBOBJC

__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_allocate(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    _MCF_mutex_init(__mtx);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_deallocate(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    /* libobjc calls this function only when the mutex is being held by
     * the current thread. Proceed anyway without any cleanup.  */
    (void) __objc_mutex;
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_lock(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    int __err = _MCF_mutex_lock(__mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_trylock(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return __err;
  }

__MCF_ALWAYS_INLINE
int
__gthread_objc_mutex_unlock(objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_allocate(objc_condition_t __objc_cond) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cond->backend);
    _MCF_cond_init(__cond);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_deallocate(objc_condition_t __objc_cond) __MCF_NOEXCEPT
  {
    (void) __objc_cond;
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_wait(objc_condition_t __objc_cond, objc_mutex_t __objc_mutex) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cond->backend);
    _MCF_mutex* __mtx = (_MCF_mutex*)(void*) &(__objc_mutex->backend);
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_broadcast(objc_condition_t __objc_cond) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cond->backend);
    _MCF_cond_signal_all(__cond);
    return 0;
  }

__MCF_ALWAYS_INLINE
int
__gthread_objc_condition_signal(objc_condition_t __objc_cond) __MCF_NOEXCEPT
  {
    _MCF_cond* __cond = (_MCF_cond*)(void*) &(__objc_cond->backend);
    _MCF_cond_signal(__cond);
    return 0;
  }

#endif  /* _LIBOBJC  */

__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
objc_thread_t
__gthread_objc_thread_detach(__MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT
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

__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
int
__gthread_objc_thread_set_priority(int __priority) __MCF_NOEXCEPT
  {
    int __wprio = (__priority < 0) ? -2 : (__priority - 2);
    int __err = _MCF_thread_set_priority(NULL, (_MCF_thread_priority) __wprio);
    return __err;
  }

__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
int
__gthread_objc_thread_get_priority(void) __MCF_NOEXCEPT
  {
    int __wprio = (int) _MCF_thread_get_priority(NULL);
    return (__wprio < -2) ? 0 : (__wprio + 2);
  }

__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
void
__gthread_objc_thread_yield(void) __MCF_NOEXCEPT
  {
    _MCF_yield();
  }

__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
int
__gthread_objc_thread_exit(void) __MCF_NOEXCEPT
  {
    _MCF_thread_exit();
  }

__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
objc_thread_t
__gthread_objc_thread_id(void) __MCF_NOEXCEPT
  {
    return (objc_thread_t)(uintptr_t) _MCF_thread_self_tid();
  }

__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
void*
__gthread_objc_thread_get_data(void) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    if(!__self)
      return NULL;

    return __self->__libobjc_tls_data;
  }

__MCF_DECLSPEC_GTHR_LIBOBJC_INLINE
int
__gthread_objc_thread_set_data(void* __value) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    if(!__self)
      return -1;

    __self->__libobjc_tls_data = __value;
    return 0;
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_GTHR_LIBOBJC_  */
