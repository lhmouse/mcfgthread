/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_GTHR_
#define __MCFGTHREAD_GTHR_

#include "fwd.h"
#include "gthr_aux.h"
#include <errno.h>

__MCF_CXX(extern "C" {)
#ifndef __MCF_GTHR_IMPORT
#  define __MCF_GTHR_IMPORT
#  define __MCF_GTHR_INLINE  __MCF_GNU_INLINE
#endif

/* Enable full C++11 threading support.  */
#define __GTHREADS  1
#define __GTHREADS_CXX0X  1
#define __GTHREAD_HAS_COND  1

/* Define gthread types. These usually map to our APIs directly, except
 * the recursive mutex.  */
typedef __MCF_timespec __gthread_time_t;
typedef _MCF_thread* __gthread_t;
typedef _MCF_tls_key* __gthread_key_t;

typedef _MCF_once __gthread_once_t;
typedef _MCF_cond __gthread_cond_t;
typedef _MCF_mutex __gthread_mutex_t;
typedef __MCF_gthr_rc_mutex __gthread_recursive_mutex_t;

/* Define macros for static and dynamic initialization.  */
#define __GTHREAD_ONCE_INIT   __MCF_0_INIT
#define __GTHREAD_COND_INIT   __MCF_0_INIT
#define __GTHREAD_COND_INIT_FUNCTION   __MCF_gthr_cond_init
#define __GTHREAD_MUTEX_INIT   __MCF_0_INIT
#define __GTHREAD_MUTEX_INIT_FUNCTION   __MCF_gthr_mutex_init
#define __GTHREAD_RECURSIVE_MUTEX_INIT   __MCF_0_INIT
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION   __MCF_gthr_recursive_mutex_init

/* Informs the runtime that threading support is active.
 * Windows creates new threads for console control handlers, so threading
 * cannot be disabled.  */
__MCF_GTHR_IMPORT __MCF_FN_CONST
int
__MCF_gthr_active_p(void) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_active_p, __gthread_active_p);
#  define __MCF_gthr_active_p  __gthread_active_p
#endif

/* Performs one-time initialization, like `pthread_once()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_once(__gthread_once_t* __once, __MCF_once_callback* __init_proc) __MCF_MAY_THROW;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_once, __gthread_once);
#  define __MCF_gthr_once  __gthread_once
#endif

/* Allocates a thread-specific key, like `pthread_key_create()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_key_create(__gthread_key_t* __keyp, _MCF_tls_dtor* __dtor_opt) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_key_create, __gthread_key_create);
#  define __MCF_gthr_key_create  __gthread_key_create
#endif

/* Destroys a thread-specific key, like `pthread_key_delete()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_key_delete(__gthread_key_t __key) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_key_delete, __gthread_key_delete);
#  define __MCF_gthr_key_delete  __gthread_key_delete
#endif

/* Gets a thread-specific value, like `pthread_getspecific()`.  */
__MCF_GTHR_IMPORT __MCF_FN_PURE
void*
__MCF_gthr_getspecific(__gthread_key_t __key) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_getspecific, __gthread_getspecific);
#  define __MCF_gthr_getspecific  __gthread_getspecific
#endif

/* Sets a thread-specific value, like `pthread_setspecific()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_setspecific(__gthread_key_t __key, const void* __val_opt) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_setspecific, __gthread_setspecific);
#  define __MCF_gthr_setspecific  __gthread_setspecific
#endif

/* Initializes a mutex, like `pthread_mutex_init()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_mutex_init(__gthread_mutex_t* __mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_mutex_init, __gthread_mutex_init);
#  define __MCF_gthr_mutex_init  __gthread_mutex_init
#endif

/* Destroys a mutex. This function does nothing.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_mutex_destroy(__gthread_mutex_t* __mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_mutex_destroy, __gthread_mutex_destroy);
#  define __MCF_gthr_mutex_destroy  __gthread_mutex_destroy
#endif

/* Locks a mutex, like `pthread_mutex_lock()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_mutex_lock(__gthread_mutex_t* __mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_mutex_lock, __gthread_mutex_lock);
#  define __MCF_gthr_mutex_lock  __gthread_mutex_lock
#endif

/* Tries locking a mutex without blocking, like `pthread_mutex_trylock()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_mutex_trylock(__gthread_mutex_t* __mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_mutex_trylock, __gthread_mutex_trylock);
#  define __MCF_gthr_mutex_trylock  __gthread_mutex_trylock
#endif

/* Tries locking a mutex until a time point, like `pthread_mutex_timedlock()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_mutex_timedlock(__gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_mutex_timedlock, __gthread_mutex_timedlock);
#  define __MCF_gthr_mutex_timedlock  __gthread_mutex_timedlock
#endif

/* Unlocks a mutex, like `pthread_mutex_unlock()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_mutex_unlock(__gthread_mutex_t* __mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_mutex_unlock, __gthread_mutex_unlock);
#  define __MCF_gthr_mutex_unlock  __gthread_mutex_unlock
#endif

/* Initializes a recursive mutex, like `pthread_mutex_init()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_recursive_mutex_init(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_recursive_mutex_init, __gthread_recursive_mutex_init);
#  define __MCF_gthr_recursive_mutex_init  __gthread_recursive_mutex_init
#endif

/* Destroys a recursive mutex. This function does nothing.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_recursive_mutex_destroy(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_recursive_mutex_destroy, __gthread_recursive_mutex_destroy);
#  define __MCF_gthr_recursive_mutex_destroy  __gthread_recursive_mutex_destroy
#endif

/* Locks a recursive mutex, like `pthread_mutex_lock()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_recursive_mutex_lock(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_recursive_mutex_lock, __gthread_recursive_mutex_lock);
#  define __MCF_gthr_recursive_mutex_lock  __gthread_recursive_mutex_lock
#endif

/* Tries locking a recursive mutex without blocking, like
 * `pthread_mutex_trylock()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_recursive_mutex_trylock(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_recursive_mutex_trylock, __gthread_recursive_mutex_trylock);
#  define __MCF_gthr_recursive_mutex_trylock  __gthread_recursive_mutex_trylock
#endif

/* Tries locking a recursive mutex until a time point, like
 * `pthread_mutex_timedlock()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_recursive_mutex_timedlock(__gthread_recursive_mutex_t* __rmtx, const __gthread_time_t* __abs_time) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_recursive_mutex_timedlock, __gthread_recursive_mutex_timedlock);
#  define __MCF_gthr_recursive_mutex_timedlock  __gthread_recursive_mutex_timedlock
#endif

/* Unlocks a recursive mutex, like `pthread_mutex_unlock()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_recursive_mutex_unlock(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_recursive_mutex_unlock, __gthread_recursive_mutex_unlock);
#  define __MCF_gthr_recursive_mutex_unlock  __gthread_recursive_mutex_unlock
#endif

/* Initializes a condition variable, like `pthread_cond_init()`.
 * This function exists not in GCC's 'gthr.h' but in 'gthr-posix.h'.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_cond_init(__gthread_cond_t* __cond) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_cond_init, __gthread_cond_init);
#  define __MCF_gthr_cond_init  __gthread_cond_init
#endif

/* Destroys a condition variable. This function does nothing.
 * This function exists not in GCC's 'gthr.h' but in 'gthr-posix.h'.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_cond_destroy(__gthread_cond_t* __cond) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_cond_destroy, __gthread_cond_destroy);
#  define __MCF_gthr_cond_destroy  __gthread_cond_destroy
#endif

/* Waits for a condition variable, like `pthread_cond_wait()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_cond_wait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_cond_wait, __gthread_cond_wait);
#  define __MCF_gthr_cond_wait  __gthread_cond_wait
#endif

/* Waits for a condition variable, like `pthread_cond_wait()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_cond_wait_recursive(__gthread_cond_t* __cond, __gthread_recursive_mutex_t* __rmtx) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_cond_wait_recursive, __gthread_cond_wait_recursive);
#  define __MCF_gthr_cond_wait_recursive  __gthread_cond_wait_recursive
#endif

/* Waits for a condition variable until a time point, like
 * `pthread_cond_timedwait()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_cond_timedwait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_cond_timedwait, __gthread_cond_timedwait);
#  define __MCF_gthr_cond_timedwait  __gthread_cond_timedwait
#endif

/* Signals at most one thread that is waiting on the condition variable, like
 * `pthread_cond_signal()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_cond_signal(__gthread_cond_t* __cond) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_cond_signal, __gthread_cond_signal);
#  define __MCF_gthr_cond_signal  __gthread_cond_signal
#endif

/* Signals all threads that are waiting on the condition variable, like
 * `pthread_cond_broadcast()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_cond_broadcast(__gthread_cond_t* __cond) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_cond_broadcast, __gthread_cond_broadcast);
#  define __MCF_gthr_cond_broadcast  __gthread_cond_broadcast
#endif

/* Creates a thread, like `pthread_create()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_create_v3(__gthread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_create_v3, __gthread_create);
#  define __MCF_gthr_create_v3  __gthread_create
#endif

/* Awaits a thread to terminate and gets its result, like `pthread_join()`.  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_join_v3(__gthread_t __thrd, void** __resp_opt) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_join_v3, __gthread_join);
#  define __MCF_gthr_join_v3  __gthread_join
#endif

/* Detaches a thread, like `pthread_detach()`  */
__MCF_GTHR_IMPORT
int
__MCF_gthr_detach(__gthread_t __thrd) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_detach, __gthread_detach);
#  define __MCF_gthr_detach  __gthread_detach
#endif

/* Gets a thread itself, like `pthread_self()`.
 * The thread shall be the main thread, or shall have been created by
 * `__gthread_create()`. Otherwise the behavior is undefined.  */
__MCF_GTHR_IMPORT __MCF_FN_CONST
__gthread_t
__MCF_gthr_self(void) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_self, __gthread_self);
#  define __MCF_gthr_self  __gthread_self
#endif

/* Checks whether two thread IDs compare equal, like `pthread_equal()`.  */
__MCF_GTHR_IMPORT __MCF_FN_CONST
int
__MCF_gthr_equal(__gthread_t __t1, __gthread_t __t2) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_equal, __gthread_equal);
#  define __MCF_gthr_equal  __gthread_equal
#endif

/* Gives up the current time slice, like `sched_yield()`.  */
__MCF_GTHR_IMPORT
void
__MCF_gthr_yield(void) __MCF_noexcept;

#ifndef __MCF_GTHR_NO_ALIASES
__MCF_FNA(__MCF_gthr_yield, __gthread_yield);
#  define __MCF_gthr_yield  __gthread_yield
#endif

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_GTHR_INLINE __MCF_FN_CONST
int
__MCF_gthr_active_p(void) __MCF_noexcept
  {
    return 1;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_once(__gthread_once_t* __once, __MCF_once_callback* __init_proc) __MCF_MAY_THROW
  {
    __MCF_GTHR_CALL_ONCE_SEH(__once, __init_proc, __MCF_nullptr);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_key_create(__gthread_key_t* __keyp, _MCF_tls_dtor* __dtor_opt) __MCF_noexcept
  {
    _MCF_tls_key* __key = _MCF_tls_key_new(__dtor_opt);
    *__keyp = __key;
    return (__key == __MCF_nullptr) ? ENOMEM : 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_key_delete(__gthread_key_t __key) __MCF_noexcept
  {
    _MCF_tls_key_delete(__key);
    return 0;
  }

__MCF_GTHR_INLINE __MCF_FN_PURE
void*
__MCF_gthr_getspecific(__gthread_key_t __key) __MCF_noexcept
  {
    return _MCF_tls_get(__key);
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_setspecific(__gthread_key_t __key, const void* __val_opt) __MCF_noexcept
  {
    int __err = _MCF_tls_set(__key, __val_opt);
    return (__err != 0) ? EINVAL : 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_mutex_init(__gthread_mutex_t* __mtx) __MCF_noexcept
  {
    _MCF_mutex_init(__mtx);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_mutex_destroy(__gthread_mutex_t* __mtx) __MCF_noexcept
  {
    (void) __mtx;
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_mutex_lock(__gthread_mutex_t* __mtx) __MCF_noexcept
  {
    int __err = _MCF_mutex_lock(__mtx, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_mutex_trylock(__gthread_mutex_t* __mtx) __MCF_noexcept
  {
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return (__err != 0) ? EBUSY : 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_mutex_timedlock(__gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_noexcept
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return (__err != 0) ? ETIMEDOUT : 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_mutex_unlock(__gthread_mutex_t* __mtx) __MCF_noexcept
  {
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_recursive_mutex_init(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept
  {
    __MCF_gthr_rc_mutex_init(__rmtx);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_recursive_mutex_destroy(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept
  {
    (void) __rmtx;
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_recursive_mutex_lock(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept
  {
    int __err = __MCF_gthr_rc_mutex_recurse(__rmtx);
    if(__err == 0)
      return 0;

    __err = __MCF_gthr_rc_mutex_wait(__rmtx, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_recursive_mutex_trylock(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept
  {
    int64_t __timeout;
    int __err = __MCF_gthr_rc_mutex_recurse(__rmtx);
    if(__err == 0)
      return 0;

    __timeout = 0;
    __err = __MCF_gthr_rc_mutex_wait(__rmtx, &__timeout);
    return (__err != 0) ? EBUSY : 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_recursive_mutex_timedlock(__gthread_recursive_mutex_t* __rmtx, const __gthread_time_t* __abs_time) __MCF_noexcept
  {
    int64_t __timeout;
    int __err = __MCF_gthr_rc_mutex_recurse(__rmtx);
    if(__err == 0)
      return 0;

    __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    __err = __MCF_gthr_rc_mutex_wait(__rmtx, &__timeout);
    return (__err != 0) ? ETIMEDOUT : 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_recursive_mutex_unlock(__gthread_recursive_mutex_t* __rmtx) __MCF_noexcept
  {
    __MCF_gthr_rc_mutex_release(__rmtx);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_cond_init(__gthread_cond_t* __cond) __MCF_noexcept
  {
    _MCF_cond_init(__cond);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_cond_destroy(__gthread_cond_t* __cond) __MCF_noexcept
  {
    (void) __cond;
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_cond_wait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx) __MCF_noexcept
  {
    int __err = __MCF_gthr_cond_mutex_wait(__cond, __mtx, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_cond_wait_recursive(__gthread_cond_t* __cond, __gthread_recursive_mutex_t* __rmtx) __MCF_noexcept
  {
    int __err = __MCF_gthr_cond_recursive_mutex_wait(__cond, __rmtx, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_cond_timedwait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_noexcept
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = __MCF_gthr_cond_mutex_wait(__cond, __mtx, &__timeout);
    return (__err != 0) ? ETIMEDOUT : 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_cond_signal(__gthread_cond_t* __cond) __MCF_noexcept
  {
    _MCF_cond_signal(__cond);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_cond_broadcast(__gthread_cond_t* __cond) __MCF_noexcept
  {
    _MCF_cond_signal_all(__cond);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_create_v3(__gthread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_noexcept
  {
    *__thrdp = __MCF_gthr_thread_create_v3(__proc, __arg);
    return (*__thrdp == __MCF_nullptr) ? EAGAIN : 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_join_v3(__gthread_t __thrd, void** __resp_opt) __MCF_noexcept
  {
    __MCF_gthr_thread_join_v3(__thrd, __resp_opt);
    return 0;
  }

__MCF_GTHR_INLINE
int
__MCF_gthr_detach(__gthread_t __thrd) __MCF_noexcept
  {
    _MCF_thread_drop_ref(__thrd);
    return 0;
  }

__MCF_GTHR_INLINE __MCF_FN_CONST
__gthread_t
__MCF_gthr_self(void) __MCF_noexcept
  {
    return _MCF_thread_self();
  }

__MCF_GTHR_INLINE __MCF_FN_CONST
int
__MCF_gthr_equal(__gthread_t __t1, __gthread_t __t2) __MCF_noexcept
  {
    return __t1 == __t2;
  }

__MCF_GTHR_INLINE
void
__MCF_gthr_yield(void) __MCF_noexcept
  {
    _MCF_yield();
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_GTHR_  */
