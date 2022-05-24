/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_GTHR_
#define __MCFGTHREAD_GTHR_

#include "fwd.h"
#include "tls.h"
#include "once.h"
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include "gthr_aux.h"
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define wrappers as required by 'gthr-default.h'.  */
#ifndef __MCF_DECLSPEC_GTHR
#  define __MCF_DECLSPEC_GTHR(...)  __VA_ARGS__
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
    uint8_t __joinable;
    intptr_t __reserved[2];
  };

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
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_active_p(void) __MCF_NOEXCEPT
  __attribute__((__const__));

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_active_p(void) __MCF_NOEXCEPT
  {
    return 1;
  }

static __inline__
int
__gthread_active_p(void) __MCF_NOEXCEPT
  {
    return __MCF_gthr_active_p();
  }

/* These are auxiliary functions for condition variables. The argument is a
 * pointer to a `__gthread_recursive_mutex_t`.  */
__MCF_DECLSPEC_GTHR()
intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t __arg) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR()
void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked) __MCF_NOEXCEPT;

/* This is the actual thread function for a gthread.  */
__MCF_DECLSPEC_GTHR()
void
__MCF_gthr_thread_thunk_v2(_MCF_thread* __thrd) __MCF_NOEXCEPT;

/* Performs one-time initialization, like `pthread_once()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_once(__gthread_once_t* __once, void __init_proc(void));

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_once(__gthread_once_t* __once, void __init_proc(void))
  {
    _MCF_once* __cleanup __attribute__((__cleanup__(__MCF_gthr_unonce))) = NULL;

    if(_MCF_once_wait(__once, NULL) == 0)
      return 0;

    __cleanup = __once;
    __init_proc();
    __cleanup = NULL;

    _MCF_once_release(__once);
    return 0;
  }

static __inline__
int
__gthread_once(__gthread_once_t* __once, void __init_proc(void))
  {
    return __MCF_gthr_once(__once, __init_proc);
  }

/* Allocates a thread-specific key, like `pthread_key_create()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_key_create(__gthread_key_t* __keyp, void __dtor_opt(void*)) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_key_create(__gthread_key_t* __keyp, void __dtor_opt(void*)) __MCF_NOEXCEPT
  {
    _MCF_tls_key* __key = _MCF_tls_key_new(__dtor_opt);
    *__keyp = __key;
    return (__key == NULL) ? ENOMEM : 0;
  }

static __inline__
int
__gthread_key_create(__gthread_key_t* __keyp, void __dtor_opt(void*)) __MCF_NOEXCEPT
  {
    return __MCF_gthr_key_create(__keyp, __dtor_opt);
  }

/* Destroys a thread-specific key, like `pthread_key_delete()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_key_delete(__gthread_key_t __key) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_key_delete(__gthread_key_t __key) __MCF_NOEXCEPT
  {
    _MCF_tls_key_delete(__key);
    return 0;
  }

static __inline__
int
__gthread_key_delete(__gthread_key_t __key) __MCF_NOEXCEPT
  {
    return __MCF_gthr_key_delete(__key);
  }

/* Gets a thread-specific value, like `pthread_getspecific()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
void*
__MCF_gthr_getspecific(__gthread_key_t __key) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
void*
__MCF_gthr_getspecific(__gthread_key_t __key) __MCF_NOEXCEPT
  {
    return _MCF_tls_get(__key);
  }

static __inline__
void*
__gthread_getspecific(__gthread_key_t __key) __MCF_NOEXCEPT
  {
    return __MCF_gthr_getspecific(__key);
  }

/* Sets a thread-specific value, like `pthread_setspecific()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_setspecific(__gthread_key_t __key, const void* __val_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_setspecific(__gthread_key_t __key, const void* __val_opt) __MCF_NOEXCEPT
  {
    int __err = _MCF_tls_set(__key, __val_opt);
    return (__err != 0) ? ENOMEM : 0;
  }

static __inline__
int
__gthread_setspecific(__gthread_key_t __key, const void* __val_opt) __MCF_NOEXCEPT
  {
    return __MCF_gthr_setspecific(__key, __val_opt);
  }

/* Initializes a mutex, like `pthread_mutex_init()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_init(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_init(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_init(__mtx);
    return 0;
  }

static __inline__
int
__gthread_mutex_init(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_mutex_init(__mtx);
  }

/* Destroys a mutex. This function does nothing.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_destroy(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_destroy(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    (void) __mtx;
    return 0;
  }

static __inline__
int
__gthread_mutex_destroy(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_mutex_destroy(__mtx);
  }

/* Locks a mutex, like `pthread_mutex_lock()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_lock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_lock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_mutex_lock(__mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

static __inline__
int
__gthread_mutex_lock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_mutex_lock(__mtx);
  }

/* Tries locking a mutex without blocking, like `pthread_mutex_trylock()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_trylock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_trylock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return (__err != 0) ? EBUSY : 0;
  }

static __inline__
int
__gthread_mutex_trylock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_mutex_trylock(__mtx);
  }

/* Tries locking a mutex until a time point, like `pthread_mutex_timedlock()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_timedlock(__gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_timedlock(__gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return (__err != 0) ? ETIMEDOUT : 0;
  }

static __inline__
int
__gthread_mutex_timedlock(__gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    return __MCF_gthr_mutex_timedlock(__mtx, __abs_time);
  }

/* Unlocks a mutex, like `pthread_mutex_unlock()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_unlock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_mutex_unlock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

static __inline__
int
__gthread_mutex_unlock(__gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_mutex_unlock(__mtx);
  }

/* Initializes a recursive mutex, like `pthread_mutex_init()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_init(__gthread_recursive_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_init(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    __rmtx->__owner = 0;
    __rmtx->__depth = 0;
    _MCF_mutex_init(&(__rmtx->__mutex));
    return 0;
  }

static __inline__
int
__gthread_recursive_mutex_init(__gthread_recursive_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_recursive_mutex_init(__mtx);
  }

/* Destroys a recursive mutex. This function does nothing.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_destroy(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_destroy(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    (void) __rmtx;
    return 0;
  }

static __inline__
int
__gthread_recursive_mutex_destroy(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_recursive_mutex_destroy(__rmtx);
  }

/* Locks a recursive mutex, like `pthread_mutex_lock()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_lock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_lock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    int __err;

    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(&(__rmtx->__owner)) == (int32_t) __my_tid) {
      __MCF_ASSERT(__rmtx->__depth < __INT32_MAX__);
      __rmtx->__depth ++;
      return 0;
    }

    /* Attempt to take ownership.  */
    __err = _MCF_mutex_lock(&(__rmtx->__mutex), NULL);
    __MCF_ASSERT(__err == 0);

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__rmtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(__rmtx->__owner), (int32_t) __my_tid);
    __MCF_ASSERT(__rmtx->__depth == 0);
    __rmtx->__depth = 1;
    return 0;
  }

static __inline__
int
__gthread_recursive_mutex_lock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_recursive_mutex_lock(__rmtx);
  }

/* Tries locking a recursive mutex without blocking, like
 * `pthread_mutex_trylock()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_trylock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_trylock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    int64_t __timeout = 0;
    int __err;

    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(&(__rmtx->__owner)) == (int32_t) __my_tid) {
      __MCF_ASSERT(__rmtx->__depth < __INT32_MAX__);
      __rmtx->__depth ++;
      return 0;
    }

    /* Attempt to take ownership.  */
    __err = _MCF_mutex_lock(&(__rmtx->__mutex), &__timeout);
    if(__err != 0)
      return EBUSY;

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__rmtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(__rmtx->__owner), (int32_t) __my_tid);
    __MCF_ASSERT(__rmtx->__depth == 0);
    __rmtx->__depth = 1;
    return 0;
  }

static __inline__
int
__gthread_recursive_mutex_trylock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_recursive_mutex_trylock(__rmtx);
  }

/* Tries locking a recursive mutex until a time point, like
 * `pthread_mutex_timedlock()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_timedlock(__gthread_recursive_mutex_t* __rmtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_timedlock(__gthread_recursive_mutex_t* __rmtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();
    int64_t __timeout = 0;
    int __err;

    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(&(__rmtx->__owner)) == (int32_t) __my_tid) {
      __MCF_ASSERT(__rmtx->__depth < __INT32_MAX__);
      __rmtx->__depth ++;
      return 0;
    }

    /* Attempt to take ownership.  */
    __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    __err = _MCF_mutex_lock(&(__rmtx->__mutex), &__timeout);
    if(__err != 0)
      return ETIMEDOUT;

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__rmtx->__owner == 0);
    _MCF_atomic_store_32_rlx(&(__rmtx->__owner), (int32_t) __my_tid);
    __MCF_ASSERT(__rmtx->__depth == 0);
    __rmtx->__depth = 1;
    return 0;
  }

static __inline__
int
__gthread_recursive_mutex_timedlock(__gthread_recursive_mutex_t* __rmtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    return __MCF_gthr_recursive_mutex_timedlock(__rmtx, __abs_time);
  }

/* Unlocks a recursive mutex, like `pthread_mutex_unlock()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_unlock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_recursive_mutex_unlock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    uint32_t __my_tid = _MCF_thread_self_tid();

    /* Reduce a level of recursion.  */
    __MCF_ASSERT(__rmtx->__owner == __my_tid);
    __MCF_ASSERT(__rmtx->__depth > 0);
    __rmtx->__depth --;
    if(__rmtx->__depth != 0)
      return 0;

    /* Give up ownership now.  */
    _MCF_atomic_store_32_rlx(&(__rmtx->__owner), 0);
    _MCF_mutex_unlock(&(__rmtx->__mutex));
    return 0;
  }

static __inline__
int
__gthread_recursive_mutex_unlock(__gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_recursive_mutex_unlock(__rmtx);
  }

/* Initializes a condition variable, like `pthread_cond_init()`.
 * This function exists not in GCC's 'gthr.h' but in 'gthr-posix.h'.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_init(__gthread_cond_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_init(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_init(__cond);
    return 0;
  }

static __inline__
int
__gthread_cond_init(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_gthr_cond_init(__cond);
  }

/* Destroys a condition variable. This function does nothing.
 * This function exists not in GCC's 'gthr.h' but in 'gthr-posix.h'.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_destroy(__gthread_cond_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_destroy(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    (void) __cond;
    return 0;
  }

static __inline__
int
__gthread_cond_destroy(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_gthr_cond_destroy(__cond);
  }

/* Waits for a condition variable, like `pthread_cond_wait()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_wait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_wait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

static __inline__
int
__gthread_cond_wait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_cond_wait(__cond, __mtx);
  }

/* Waits for a condition variable, like `pthread_cond_wait()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_wait_recursive(__gthread_cond_t* __cond, __gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_wait_recursive(__gthread_cond_t* __cond, __gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_recursive_mutex_unlock_callback, __MCF_gthr_recursive_mutex_relock_callback, (intptr_t) __rmtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

static __inline__
int
__gthread_cond_wait_recursive(__gthread_cond_t* __cond, __gthread_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    return __MCF_gthr_cond_wait_recursive(__cond, __rmtx);
  }

/* Waits for a condition variable until a time point, like
 * `pthread_cond_timedwait()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_timedwait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_timedwait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, &__timeout);
    return (__err != 0) ? ETIMEDOUT : 0;
  }

static __inline__
int
__gthread_cond_timedwait(__gthread_cond_t* __cond, __gthread_mutex_t* __mtx, const __gthread_time_t* __abs_time) __MCF_NOEXCEPT
  {
    return __MCF_gthr_cond_timedwait(__cond, __mtx, __abs_time);
  }

/* Signals at most one thread that is waiting on the condition variable, like
 * `pthread_cond_signal()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_signal(__gthread_cond_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_signal(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal(__cond);
    return 0;
  }

static __inline__
int
__gthread_cond_signal(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_gthr_cond_signal(__cond);
  }

/* Signals all threads that are waiting on the condition variable, like
 * `pthread_cond_broadcast()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_broadcast(__gthread_cond_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_cond_broadcast(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal_all(__cond);
    return 0;
  }

static __inline__
int
__gthread_cond_broadcast(__gthread_cond_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_gthr_cond_broadcast(__cond);
  }

/* Creates a thread, like `pthread_create()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_create_v2(__gthread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_create_v2(__gthread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record __rec[1];
    __rec->__proc = __proc;
    __rec->__arg = __arg;
    __rec->__joinable = 1;

    _MCF_thread* __thrd = _MCF_thread_new(__MCF_gthr_thread_thunk_v2, __rec, sizeof(*__rec));
    *__thrdp = __thrd;
    return (__thrd == NULL) ? EAGAIN : 0;  /* as specified by POSIX  */
  }

static __inline__
int
__gthread_create(__gthread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT
  {
    return __MCF_gthr_create_v2(__thrdp, __proc, __arg);
  }

/* Awaits a thread to terminate and gets its result, like `pthread_join()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_join_v2(__gthread_t __thrd, void** __resp_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_join_v2(__gthread_t __thrd, void** __resp_opt) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record* __rec;
    int __err;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if(__thrd->__proc != __MCF_gthr_thread_thunk_v2)
      return EINVAL;

    __rec = (__MCF_gthr_thread_record*) _MCF_thread_get_data(__thrd);

    /* Fail if the thread has already been detached.  */
    if(_MCF_atomic_xchg_8_rlx(&(__rec->__joinable), 0) == 0)
      return EINVAL;

    /* Wait for it.  */
    if(__thrd == _MCF_thread_self())
      return EDEADLK;

    __err = _MCF_thread_wait(__thrd, NULL);
    __MCF_ASSERT(__err == 0);

    if(__resp_opt)
      *__resp_opt = __rec->__result;

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return 0;
  }

static __inline__
int
__gthread_join(__gthread_t __thrd, void** __resp_opt) __MCF_NOEXCEPT
  {
    return __MCF_gthr_join_v2(__thrd, __resp_opt);
  }

/* Detaches a thread, like `pthread_detach()`  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_detach_v2(__gthread_t __thrd) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
int
__MCF_gthr_detach_v2(__gthread_t __thrd) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record* __rec;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if(__thrd->__proc != __MCF_gthr_thread_thunk_v2)
      return EINVAL;

    __rec = (__MCF_gthr_thread_record*) _MCF_thread_get_data(__thrd);

    /* Fail if the thread has already been detached.  */
    if(_MCF_atomic_xchg_8_rlx(&(__rec->__joinable), 0) == 0)
      return EINVAL;

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return 0;
  }

static __inline__
int
__gthread_detach(__gthread_t __thrd) __MCF_NOEXCEPT
  {
    return __MCF_gthr_detach_v2(__thrd);
  }

/* Gets a thread itself, like `pthread_self()`.
 * The thread shall be the main thread, or shall have been created by
 * `__gthr_create()`. Otherwise the behavior is undefined.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
__gthread_t
__MCF_gthr_self(void) __MCF_NOEXCEPT
  __attribute__((__const__, __returns_nonnull__));

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
__gthread_t
__MCF_gthr_self(void) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    __MCF_CHECK(__self);
    return __self;
  }

static __inline__
__gthread_t
__gthread_self(void) __MCF_NOEXCEPT
  {
    return __MCF_gthr_self();
  }

/* Checks whether two thread IDs compare equal, like `pthread_equal()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
__MCF_CXX11(constexpr)
int
__MCF_gthr_equal(__gthread_t __t1, __gthread_t __t2) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
__MCF_CXX11(constexpr)
int
__MCF_gthr_equal(__gthread_t __t1, __gthread_t __t2) __MCF_NOEXCEPT
  {
    return __t1 == __t2;
  }

static __inline__
int
__gthread_equal(__gthread_t __t1, __gthread_t __t2) __MCF_NOEXCEPT
  {
    return __MCF_gthr_equal(__t1, __t2);
  }

/* Gives up the current time slice, like `sched_yield()`.  */
__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
void
__MCF_gthr_yield(void) __MCF_NOEXCEPT;

__MCF_DECLSPEC_GTHR(__MCF_GNU_INLINE)
void
__MCF_gthr_yield(void) __MCF_NOEXCEPT
  {
    _MCF_yield();
  }

static __inline__
void
__gthread_yield(void) __MCF_NOEXCEPT
  {
    return __MCF_gthr_yield();
  }

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_GTHR_  */
