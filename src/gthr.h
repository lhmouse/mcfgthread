/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_GTHR_H_
#define __MCFGTHREAD_GTHR_H_

#include "fwd.h"
#include "tls.h"
#include "once.h"
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include <time.h>  /* struct timespec  */
#include <errno.h>  /* E* macros  */

#ifdef __cplusplus
extern "C" {
#endif

/* Define wrappers as required by 'gthr-default.h'.  */
#ifndef __MCF_GTHR_EXTERN_INLINE
#  define __MCF_GTHR_EXTERN_INLINE  __MCF_GNU_INLINE
#endif

/* Enable full C++11 threading support.  */
#define __GTHREADS  1
#define __GTHREADS_CXX0X  1
#define __GTHREAD_HAS_COND  1

/* Define gthread types. These usually map to our APIs directly, except
 * the recursive mutex.  */
typedef struct timespec __gthread_time_t;
typedef _MCF_thread* __gthread_t;
typedef _MCF_tls_key* __gthread_key_t;

typedef _MCF_once __gthread_once_t;
typedef _MCF_cond __gthread_cond_t;
typedef _MCF_mutex __gthread_mutex_t;

typedef struct __MCF_gthr_recursive_mutex_t __gthread_recursive_mutex_t;
typedef struct __MCF_gthr_thread_record __MCF_gthr_thread_record;

typedef void* __MCF_gthr_thread_procedure(void* __arg);

struct __MCF_gthr_recursive_mutex_t
  {
    uint32_t __owner;  /* owner thread ID  */
    int __depth;  /* recursion depth  */
    _MCF_mutex __mutex;
  };

struct __MCF_gthr_thread_record
  {
    void* __result;
    __MCF_gthr_thread_procedure* __proc;
    void* __arg;
  };

/* Define macros for static and dynamic initialization.  */
#define __GTHREAD_ONCE_INIT  {0}
#define __GTHREAD_COND_INIT  {0}
#define __GTHREAD_MUTEX_INIT  {0}
#define __GTHREAD_RECURSIVE_MUTEX_INIT  {0}

/* Informs the runtime that threading support is active.
 * Windows creates new threads for console control handlers, so threading
 * cannot be disabled.  */
int
__MCF_gthr_active_p(void) __MCF_NOEXCEPT
  __attribute__((__const__));

#define __gthread_active_p  __MCF_gthr_active_p

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_active_p(void) __MCF_NOEXCEPT
  {
    return 1;
  }

/* This is an auxiliary function for exception handling in `__gthread_once()`.
 * Ideally, if the target function throws exception we would like to allow
 * attempts to retry. Sadly this is not possible in standard C.  */
void
__MCF_gthr_unonce(__gthread_once_t** __oncep) __MCF_NOEXCEPT;

__MCF_GTHR_EXTERN_INLINE
void
__MCF_gthr_unonce(__gthread_once_t** __oncep) __MCF_NOEXCEPT
  {
    if(*__oncep)
      _MCF_once_abort(*__oncep);
  }

/* This is an auxiliary function for converting a `struct timespec` to the
 * number of milliseconds since the Unix epoch, with boundary checking.  */
int64_t
__MCF_gthr_timeout_from_timespec(const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_GTHR_EXTERN_INLINE
int64_t
__MCF_gthr_timeout_from_timespec(const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    double __time_ms = 0.000999;
    __time_ms += (double) __abs_time->tv_nsec * 0.000001;
    __time_ms += (double) __abs_time->tv_sec * 1000;

    if(__time_ms < 0)
      return 0;  /* underflowed  */

    if(__time_ms > 0x7FFFFFFFFFFFFC00)
      return 0x7FFFFFFFFFFFFFFF;  /* overflowed  */

    return (int64_t) __time_ms;
  }

/* These are auxiliary functions for condition variables.  */
intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

void
__MCF_gthr_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

/* This is the actual thread function for a gthread.  */
void
__MCF_gthr_thread_thunk(_MCF_thread* __thrd) __MCF_NOEXCEPT;

/* Performs one-time initialization, like `pthread_once()`.  */
int
__MCF_gthr_once(__gthread_once_t* __once, void __init_proc(void));

#define __gthread_once  __MCF_gthr_once

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_once(__gthread_once_t* __once, void __init_proc(void))
  {
    int __do_init = _MCF_once_wait(__once, NULL);
    if(__do_init == 0)
      return 0;

    _MCF_once* __cl __MCF_USE_DTOR(__MCF_gthr_unonce) = __once;
    __init_proc();
    __cl = NULL;
    _MCF_once_release(__once);
    return 0;
  }

/* Allocates a thread-specific key, like `pthread_key_create()`.  */
int
__MCF_gthr_key_create(__gthread_key_t* __keyp, void __dtor(void*)) __MCF_NOEXCEPT;

#define __gthread_key_create  __MCF_gthr_key_create

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_key_create(__gthread_key_t* __keyp, void __dtor(void*)) __MCF_NOEXCEPT
  {
    _MCF_tls_key* __key = _MCF_tls_key_new(__dtor);
    *__keyp = __key;
    return (__key == NULL) ? ENOMEM : 0;
  }

/* Destroys a thread-specific key, like `pthread_key_delete()`.  */
int
__MCF_gthr_key_delete(__gthread_key_t __key) __MCF_NOEXCEPT;

#define __gthread_key_delete  __MCF_gthr_key_delete

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_key_delete(__gthread_key_t __key) __MCF_NOEXCEPT
  {
    _MCF_tls_key_delete(__key);
    return 0;
  }

/* Gets a thread-specific value, like `pthread_getspecific()`.  */
void*
__MCF_gthr_getspecific(__gthread_key_t __key) __MCF_NOEXCEPT;

#define __gthread_getspecific  __MCF_gthr_getspecific

__MCF_GTHR_EXTERN_INLINE
void*
__MCF_gthr_getspecific(__gthread_key_t __key) __MCF_NOEXCEPT
  {
    return _MCF_tls_get(__key);
  }

/* Sets a thread-specific value, like `pthread_setspecific()`.  */
int
__MCF_gthr_setspecific(__gthread_key_t __key, const void* __ptr) __MCF_NOEXCEPT;

#define __gthread_setspecific  __MCF_gthr_setspecific

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_setspecific(__gthread_key_t __key, const void* __ptr) __MCF_NOEXCEPT
  {
    int __err = _MCF_tls_set(__key, __ptr);
    return (__err != 0) ? ENOMEM : 0;
  }

/* Initializes a mutex, like `pthread_mutex_init()`.  */
int
__MCF_gthr_mutex_init(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_init  __MCF_gthr_mutex_init
#define __GTHREAD_MUTEX_INIT_FUNCTION  __MCF_gthr_mutex_init

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_mutex_init(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_init(__mtx);
    return 0;
  }

/* Destroys a mutex. This function does nothing.  */
int
__MCF_gthr_mutex_destroy(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_destroy  __MCF_gthr_mutex_destroy

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_mutex_destroy(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    (void) __mtx;
    return 0;
  }

/* Locks a mutex, like `pthread_mutex_lock()`.  */
int
__MCF_gthr_mutex_lock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_lock  __MCF_gthr_mutex_lock

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_mutex_lock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_mutex_lock(__mtx, NULL);
    __MCFGTHREAD_ASSERT(__err == 0);
    return 0;
  }

/* Tries locking a mutex without blocking, like `pthread_mutex_trylock()`.  */
int
__MCF_gthr_mutex_trylock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_trylock  __MCF_gthr_mutex_trylock

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_mutex_trylock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return (__err != 0) ? EBUSY : 0;
  }

/* Tries locking a mutex until a time point, like `pthread_mutex_timedlock()`.  */
int
__MCF_gthr_mutex_timedlock(__gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT;

#define __gthread_mutex_timedlock  __MCF_gthr_mutex_timedlock

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_mutex_timedlock(__gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return (__err != 0) ? ETIMEDOUT : 0;
  }

/* Unlocks a mutex, like `pthread_mutex_unlock()`.  */
int
__MCF_gthr_mutex_unlock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_unlock  __MCF_gthr_mutex_unlock

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_mutex_unlock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

/* Initializes a recursive mutex, like `pthread_mutex_init()`.  */
int
__MCF_gthr_recursive_mutex_init(__gthread_recursive_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_recursive_mutex_init  __MCF_gthr_recursive_mutex_init
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION  __MCF_gthr_recursive_mutex_init

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_recursive_mutex_init(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    __rmtx->__owner = 0;
    __rmtx->__depth = 0;
    _MCF_mutex_init(&(__rmtx->__mutex));
    return 0;
  }

/* Destroys a recursive mutex. This function does nothing.  */
int
__MCF_gthr_recursive_mutex_destroy(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

#define __gthread_recursive_mutex_destroy  __MCF_gthr_recursive_mutex_destroy

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_recursive_mutex_destroy(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    (void) __rmtx;
    return 0;
  }

/* Locks a recursive mutex, like `pthread_mutex_lock()`.  */
int
__MCF_gthr_recursive_mutex_lock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

#define __gthread_recursive_mutex_lock  __MCF_gthr_recursive_mutex_lock

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_recursive_mutex_lock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    if(__MCF_ATOMIC_LOAD_RLX(&(__rmtx->__owner)) != __my_tid) {
      /* If the calling thread does not own this mutex, attempt to take ownership.  */
      int __err = _MCF_mutex_lock(&(__rmtx->__mutex), NULL);
      __MCFGTHREAD_ASSERT(__err == 0);

      /* The calling thread owns the mutex now.  */
      __MCFGTHREAD_ASSERT(__rmtx->__owner == 0);
      __MCF_ATOMIC_STORE_RLX(&(__rmtx->__owner), __my_tid);
      __MCFGTHREAD_ASSERT(__rmtx->__depth == 0);
    }

    /* Increment the recursion counter.  */
    __MCFGTHREAD_ASSERT(__rmtx->__depth < INT_MAX);
    __rmtx->__depth ++;
    __MCFGTHREAD_ASSERT(__rmtx->__depth != 0);
    return 0;
  }

/* Tries locking a recursive mutex without blocking, like
 * `pthread_mutex_trylock()`.  */
int
__MCF_gthr_recursive_mutex_trylock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

#define __gthread_recursive_mutex_trylock  __MCF_gthr_recursive_mutex_trylock

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_recursive_mutex_trylock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    if(__MCF_ATOMIC_LOAD_RLX(&(__rmtx->__owner)) != __my_tid) {
      /* If the calling thread does not own this mutex, attempt to take ownership.  */
      int64_t __timeout = 0;
      int __err = _MCF_mutex_lock(&(__rmtx->__mutex), &__timeout);
      if(__err != 0)
        return EBUSY;

      /* The calling thread owns the mutex now.  */
      __MCF_ATOMIC_STORE_RLX(&(__rmtx->__owner), __my_tid);
      __MCFGTHREAD_ASSERT(__rmtx->__depth == 0);
    }

    /* Increment the recursion counter.  */
    __MCFGTHREAD_ASSERT(__rmtx->__depth < INT_MAX);
    __rmtx->__depth ++;
    __MCFGTHREAD_ASSERT(__rmtx->__depth != 0);
    return 0;
  }

/* Tries locking a recursive mutex until a time point, like
 * `pthread_mutex_timedlock()`.  */
int
__MCF_gthr_recursive_mutex_timedlock(__gthread_recursive_mutex_t* __rmtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT;

#define __gthread_recursive_mutex_timedlock  __MCF_gthr_recursive_mutex_timedlock

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_recursive_mutex_timedlock(__gthread_recursive_mutex_t* __rmtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    if(__MCF_ATOMIC_LOAD_RLX(&(__rmtx->__owner)) != __my_tid) {
      /* If the calling thread does not own this mutex, attempt to take ownership.  */
      int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
      int __err = _MCF_mutex_lock(&(__rmtx->__mutex), &__timeout);
      if(__err != 0)
        return ETIMEDOUT;

      /* The calling thread owns the mutex now.  */
      __MCF_ATOMIC_STORE_RLX(&(__rmtx->__owner), __my_tid);
      __MCFGTHREAD_ASSERT(__rmtx->__depth == 0);
    }

    /* Increment the recursion counter.  */
    __MCFGTHREAD_ASSERT(__rmtx->__depth < INT_MAX);
    __rmtx->__depth ++;
    __MCFGTHREAD_ASSERT(__rmtx->__depth > 0);
    return 0;
  }

/* Unlocks a recursive mutex, like `pthread_mutex_unlock()`.  */
int
__MCF_gthr_recursive_mutex_unlock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

#define __gthread_recursive_mutex_unlock  __MCF_gthr_recursive_mutex_unlock

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_recursive_mutex_unlock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    __MCFGTHREAD_CHECK(__MCF_ATOMIC_LOAD_RLX(&(__rmtx->__owner)) == __my_tid);

    /* Decrement the recursion counter.  */
    __MCFGTHREAD_ASSERT(__rmtx->__depth > 0);
    __rmtx->__depth --;

    if(__rmtx->__depth == 0) {
      /* The calling thread shall give up ownership now.  */
      __MCF_ATOMIC_STORE_RLX(&(__rmtx->__owner), 0);
      _MCF_mutex_unlock(&(__rmtx->__mutex));
    }
    return 0;
  }

/* Initializes a condition variable, like `pthread_cond_init()`.
 * This function exists not in GCC's 'gthr.h' but in 'gthr-posix.h'.  */
int
__MCF_gthr_cond_init(__gthread_cond_t* __cond) __MCF_NOEXCEPT;

#define __gthread_cond_init  __MCF_gthr_cond_init
#define __GTHREAD_COND_INIT_FUNCTION  __MCF_gthr_cond_init

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_cond_init(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_init(__cond);
    return 0;
  }

/* Destroys a condition variable. This function does nothing.
 * This function exists not in GCC's 'gthr.h' but in 'gthr-posix.h'.  */
int
__MCF_gthr_cond_destroy(__gthread_cond_t* __cond) __MCF_NOEXCEPT;

#define __gthread_cond_destroy  __MCF_gthr_cond_destroy

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_cond_destroy(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    (void) __cond;
    return 0;
  }

/* Waits for a condition variable, like `pthread_cond_wait()`.  */
int
__MCF_gthr_cond_wait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_cond_wait  __MCF_gthr_cond_wait

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_cond_wait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, NULL);
    __MCFGTHREAD_ASSERT(__err == 0);
    return 0;
  }

/* Waits for a condition variable, like `pthread_cond_wait()`.  */
int
__MCF_gthr_cond_wait_recursive(__gthread_cond_t* __cond, __gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

#define __gthread_cond_wait_recursive  __MCF_gthr_cond_wait_recursive

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_cond_wait_recursive(__gthread_cond_t* __cond, __gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_recursive_mutex_unlock_callback, __MCF_gthr_recursive_mutex_relock_callback, (intptr_t) __rmtx, NULL);
    __MCFGTHREAD_ASSERT(__err == 0);
    return 0;
  }

/* Waits for a condition variable until a time point, like
 * `pthread_cond_timedwait()`.  */
int
__MCF_gthr_cond_timedwait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT;

#define __gthread_cond_timedwait  __MCF_gthr_cond_timedwait

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_cond_timedwait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, &__timeout);
    return (__err != 0) ? ETIMEDOUT : 0;
  }

/* Signals at most one thread that is waiting on the condition variable, like
 * `pthread_cond_signal()`.  */
int
__MCF_gthr_cond_signal(__gthread_cond_t* __cond) __MCF_NOEXCEPT;

#define __gthread_cond_signal  __MCF_gthr_cond_signal

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_cond_signal(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal_some(__cond, 1);
    return 0;
  }

/* Signals all threads that are waiting on the condition variable, like
 * `pthread_cond_broadcast()`.  */
int
__MCF_gthr_cond_broadcast(__gthread_cond_t* __cond) __MCF_NOEXCEPT;

#define __gthread_cond_broadcast  __MCF_gthr_cond_broadcast

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_cond_broadcast(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal_all(__cond);
    return 0;
  }

/* Creates a thread, like `pthread_create()`.  */
int
__MCF_gthr_create(__gthread_t* __thrdp, void* __proc(void*), void* __arg) __MCF_NOEXCEPT;

#define __gthread_create  __MCF_gthr_create

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_create(__gthread_t* __thrdp, void* __proc(void*), void* __arg) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record __rec = { 0 };
    __rec.__proc = __proc;
    __rec.__arg = __arg;

    _MCF_thread* __thrd = _MCF_thread_new(__MCF_gthr_thread_thunk, &__rec, sizeof(__rec));
    *__thrdp = __thrd;
    return (__thrd == NULL) ? EAGAIN : 0;  /* as specified by POSIX  */
  }

/* Awaits a thread to terminate and gets its result, like `pthread_join()`.  */
int
__MCF_gthr_join(__gthread_t __thrd, void** __resultp_opt) __MCF_NOEXCEPT;

#define __gthread_join  __MCF_gthr_join

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_join(__gthread_t __thrd, void** __resultp_opt) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    if(__thrd == __self)
      return EDEADLK;

    /* Wait for it.  */
    int __err = _MCF_thread_wait(__thrd, NULL);
    __MCFGTHREAD_ASSERT(__err == 0);

    if(__resultp_opt) {
      /* As there is no type information, we examine the thread procedure to
       * ensure we don't mistake a thread of a wrong type.  */
      if(__thrd->__proc == __MCF_gthr_thread_thunk) {
        __MCF_gthr_thread_record* __rec = (__MCF_gthr_thread_record*) __thrd->__data;
        *__resultp_opt = __rec->__result;
      }
      else
        *__resultp_opt = NULL;
    }

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return 0;
  }

/* Detaches a thread, like `pthread_detach()`  */
int
__MCF_gthr_detach(__gthread_t __thrd) __MCF_NOEXCEPT;

#define __gthread_detach  __MCF_gthr_detach

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_detach(__gthread_t __thrd) __MCF_NOEXCEPT
  {
    _MCF_thread_drop_ref(__thrd);
    return 0;
  }

/* Gets a thread itself, like `pthread_self()`.
 * The thread shall be the main thread, or shall have been created by
 * `__gthr_create()`. Otherwise the behavior is undefined.  */
__gthread_t
__MCF_gthr_self(void) __MCF_NOEXCEPT
  __attribute__((__const__, __returns_nonnull__));

#define __gthread_self  __MCF_gthr_self

__MCF_GTHR_EXTERN_INLINE
__gthread_t
__MCF_gthr_self(void) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    __MCFGTHREAD_CHECK(__self);
    return __self;
  }

/* Checks whether two thread IDs compare equal, like `pthread_equal()`.  */
int
__MCF_gthr_equal(__gthread_t __t1, __gthread_t __t2) __MCF_NOEXCEPT
  __attribute__((__pure__));

#define __gthread_equal  __MCF_thrd_equal

__MCF_GTHR_EXTERN_INLINE
int
__MCF_gthr_equal(__gthread_t __t1, __gthread_t __t2) __MCF_NOEXCEPT
  {
    return __t1 == __t2;
  }

/* Gives up the current time slice, like `sched_yield()`.  */
void
__MCF_gthr_yield(void) __MCF_NOEXCEPT;

#define __gthread_yield  __MCF_gthr_yield

__MCF_GTHR_EXTERN_INLINE
void
__MCF_gthr_yield(void) __MCF_NOEXCEPT
  {
    _MCF_yield();
  }

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_GTHR_H_  */
