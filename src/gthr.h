// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_GTHR_H_
#define __MCFGTHREAD_GTHR_H_

#include "fwd.h"
#include "tls.h"
#include "once.h"
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include <time.h>  // struct timespec
#include <errno.h>  // E* macros

#ifdef __cplusplus
extern "C" {
#endif

// Define wrappers as required by 'gthr-default.h'.
#ifndef __MCFGTHREAD_GTHR_C_
#  define __MCFGTHREAD_GTHR_INLINE  __MCF_GNU_INLINE
#else
#  define __MCFGTHREAD_GTHR_INLINE
#endif

// Enable full C++11 threading support.
#define __GTHREADS  1
#define __GTHREADS_CXX0X  1
#define __GTHREAD_HAS_COND  1

// Define gthread types. These usually map to our APIs directly, except
// the recursive mutex.
typedef struct timespec __gthread_time_t;
typedef _MCF_thread* __gthread_t;
typedef _MCF_tls_key* __gthread_key_t;

typedef _MCF_once __gthread_once_t;
typedef _MCF_cond __gthread_cond_t;
typedef _MCF_mutex __gthread_mutex_t;

struct __MCF_gthr_recursive_mutex_t
  {
    uint32_t __owner;  // owner thread ID
    int32_t __depth;  // recursion depth
    _MCF_mutex __mutex;
  }
  typedef __gthread_recursive_mutex_t;

// Define macros for static and dynamic initialization.
#define __GTHREAD_ONCE_INIT  {0}
#define __GTHREAD_COND_INIT  {0}
#define __GTHREAD_MUTEX_INIT  {0}
#define __GTHREAD_RECURSIVE_MUTEX_INIT  {0}

// Informs the runtime that threading support is active.
// Windows creates new threads for console control handlers, so threading
// cannot be disabled.
int
__MCF_gthr_active_p(void) __MCF_NOEXCEPT
  __attribute__((__const__));

#define __gthread_active_p  __MCF_gthr_active_p

__MCF_CXX11(constexpr)
__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_active_p(void) __MCF_NOEXCEPT
  {
    return 1;
  }

// This is an auxiliary function for exception handling in `__gthread_once()`.
// Ideally, if the target function throws exception we would like to allow
// attempts to retry. Sadly this is not possible in standard C.
void
__MCF_gthr_unonce(__gthread_once_t** __oncep) __MCF_NOEXCEPT;

__MCFGTHREAD_GTHR_INLINE void
__MCF_gthr_unonce(__gthread_once_t** __oncep) __MCF_NOEXCEPT
  {
    if(*__oncep)
      _MCF_once_abort(*__oncep);
  }

// This is an auxiliary function for converting a `struct timespec` to the
// number of milliseconds since the Unix epoch, with boundary checking.
int64_t
__MCF_gthr_timeout_from_timespec(const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCFGTHREAD_GTHR_INLINE int64_t
__MCF_gthr_timeout_from_timespec(const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    double __time_ms = 0.000999;
    __time_ms += (double) __abs_time->tv_sec * 1000;
    __time_ms += (double) __abs_time->tv_nsec * 0.000001;

    if(__time_ms < 0)
      return 0;
    else if(__time_ms > 0x7FFFFFFFFFFFFC00)
      return INT64_MAX;
    else
      return (int64_t) __time_ms;
  }

// Performs one-time initialization, like `pthread_once()`.
int
__MCF_gthr_once(__gthread_once_t* __once, void __init_proc(void));

#define __gthread_once  __MCF_gthr_once

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_once(__gthread_once_t* __once, void __init_proc(void))
  {
    int __do_init = _MCF_once_wait(__once, NULL);
    if(__do_init == 0)
      return 0;

    _MCF_once* __cl __attribute__((__cleanup__(__MCF_gthr_unonce)));
    __cl = __once;
    __init_proc();
    __cl = NULL;
    _MCF_once_release(__once);
    return 0;
  }

// Allocates a thread-specific key, like `pthread_key_create()`.
int
__MCF_gthr_key_create(__gthread_key_t* __keyp, void __dtor(void*)) __MCF_NOEXCEPT;

#define __gthread_key_create  __MCF_gthr_key_create

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_key_create(__gthread_key_t* __keyp, void __dtor(void*)) __MCF_NOEXCEPT
  {
    _MCF_tls_key* __key = _MCF_tls_key_new(__dtor);
    *__keyp = __key;
    return !__key ? ENOMEM : 0;
  }

// Destroys a thread-specific key, like `pthread_key_delete()`.
int
__MCF_gthr_key_delete(__gthread_key_t __key) __MCF_NOEXCEPT;

#define __gthread_key_delete  __MCF_gthr_key_delete

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_key_delete(__gthread_key_t __key) __MCF_NOEXCEPT
  {
    _MCF_tls_key_delete(__key);
    return 0;
  }

// Gets a thread-specific value, like `pthread_getspecific()`.
void*
__MCF_gthr_getspecific(__gthread_key_t __key) __MCF_NOEXCEPT;

#define __gthread_getspecific  __MCF_gthr_getspecific

__MCFGTHREAD_GTHR_INLINE void*
__MCF_gthr_getspecific(__gthread_key_t __key) __MCF_NOEXCEPT
  {
    return _MCF_tls_get(__key);
  }

// Sets a thread-specific value, like `pthread_setspecific()`.
int
__MCF_gthr_setspecific(__gthread_key_t __key, const void* __ptr) __MCF_NOEXCEPT;

#define __gthread_setspecific  __MCF_gthr_setspecific

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_setspecific(__gthread_key_t __key, const void* __ptr) __MCF_NOEXCEPT
  {
    int __err = _MCF_tls_set(__key, __ptr);
    return (__err != 0) ? ENOMEM : 0;
  }

// Initializes a mutex, like `pthread_mutex_init()`.
int
__MCF_gthr_mutex_init(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_init  __MCF_gthr_mutex_init
#define __GTHREAD_MUTEX_INIT_FUNCTION  __MCF_gthr_mutex_init

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_mutex_init(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_init(__mtx);
    return 0;
  }

// Destroys a mutex. This function does nothing.
int
__MCF_gthr_mutex_destroy(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_destroy  __MCF_gthr_mutex_destroy

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_mutex_destroy(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    (void) __mtx;
    return 0;
  }

// Locks a mutex, like `pthread_mutex_lock()`.
int
__MCF_gthr_mutex_lock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_lock  __MCF_gthr_mutex_lock

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_mutex_lock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_mutex_lock(__mtx, NULL);
    __MCFGTHREAD_ASSERT(__err == 0);
    return 0;
  }

// Tries locking a mutex without blocking, like `pthread_mutex_trylock()`.
int
__MCF_gthr_mutex_trylock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_trylock  __MCF_gthr_mutex_trylock

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_mutex_trylock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return (__err != 0) ? EBUSY : 0;
  }

// Tries locking a mutex until a time point, like `pthread_mutex_timedlock()`.
int
__MCF_gthr_mutex_timedlock(__gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT;

#define __gthread_mutex_timedlock  __MCF_gthr_mutex_timedlock

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_mutex_timedlock(__gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return (__err != 0) ? ETIMEDOUT : 0;
  }

// Unlocks a mutex, like `pthread_mutex_unlock()`.
int
__MCF_gthr_mutex_unlock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

#define __gthread_mutex_unlock  __MCF_gthr_mutex_unlock

__MCFGTHREAD_GTHR_INLINE int
__MCF_gthr_mutex_unlock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_GTHR_H_
