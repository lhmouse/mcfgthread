/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_LIBCXX_
#define __MCFGTHREAD_LIBCXX_

#include "fwd.h"
#include "gthr_aux.h"
#include <errno.h>
#if defined __cplusplus && (__cplusplus >= 201103L)
#  include <chrono>
#endif

#ifdef _LIBCPP_BEGIN_NAMESPACE_STD
_LIBCPP_BEGIN_NAMESPACE_STD
#endif  /* inside libc++  */
__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_LIBCXX_IMPORT
#  define __MCF_LIBCXX_IMPORT
#  define __MCF_LIBCXX_INLINE  __MCF_GNU_INLINE
#endif

/* Define threading support types for libc++.  */
typedef __MCF_timespec __libcpp_timespec_t;
typedef _MCF_thread* __libcpp_thread_t;
typedef uint32_t __libcpp_thread_id;
typedef _MCF_tls_key* __libcpp_tls_key;

typedef _MCF_once __libcpp_exec_once_flag;
typedef _MCF_cond __libcpp_condvar_t;
typedef _MCF_mutex __libcpp_mutex_t;
typedef __MCF_gthr_rc_mutex __libcpp_recursive_mutex_t;

/* Define macros for initialization.  */
#define _LIBCPP_NULL_THREAD   __MCF_nullptr
#define _LIBCPP_TLS_DESTRUCTOR_CC  /* default calling convention  */
#define _LIBCPP_EXEC_ONCE_INITIALIZER   __MCF_0_INIT
#define _LIBCPP_CONDVAR_INITIALIZER   __MCF_0_INIT
#define _LIBCPP_MUTEX_INITIALIZER   __MCF_0_INIT

/* Performs one-time initialization, like `pthread_once()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_execute_once(__libcpp_exec_once_flag* __once, __MCF_once_callback* __init_proc);

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_execute_once  __MCF_libcxx_execute_once
#endif

/* Allocates a thread-specific key, like `pthread_key_create()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_tls_create(__libcpp_tls_key* __keyp, _MCF_tls_dtor* __dtor_opt) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_tls_create  __MCF_libcxx_tls_create
#endif

/* Destroys a thread-specific key, like `pthread_key_delete()`.
 * This function is currently unused.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_tls_delete(__libcpp_tls_key __key) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_tls_delete  __MCF_libcxx_tls_delete
#endif

/* Gets a thread-specific value, like `pthread_getspecific()`.  */
__MCF_LIBCXX_INLINE __MCF_FN_PURE
void*
__MCF_libcxx_tls_get(__libcpp_tls_key __key) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_tls_get  __MCF_libcxx_tls_get
#endif

/* Sets a thread-specific value, like `pthread_setspecific()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_tls_set(__libcpp_tls_key __key, const void* __val_opt) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_tls_set  __MCF_libcxx_tls_set
#endif

/* Initializes a mutex, like `pthread_mutex_init()`.
 * This function is currently unused.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_mutex_init(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_mutex_init  __MCF_libcxx_mutex_init
#endif

/* Destroys a mutex. This function does nothing.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_mutex_destroy(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_mutex_destroy  __MCF_libcxx_mutex_destroy
#endif

/* Locks a mutex, like `pthread_mutex_lock()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_mutex_lock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_mutex_lock  __MCF_libcxx_mutex_lock
#endif

/* Tries locking a mutex without blocking, like `pthread_mutex_trylock()`.  */
__MCF_LIBCXX_INLINE
bool
__MCF_libcxx_mutex_trylock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_mutex_trylock  __MCF_libcxx_mutex_trylock
#endif

/* Unlocks a mutex, like `pthread_mutex_unlock()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_mutex_unlock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_mutex_unlock  __MCF_libcxx_mutex_unlock
#endif

/* Initializes a recursive mutex, like `pthread_mutex_init()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_init(__libcpp_recursive_mutex_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_recursive_mutex_init  __MCF_libcxx_recursive_mutex_init
#endif

/* Destroys a recursive mutex. This function does nothing.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_destroy(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_recursive_mutex_destroy  __MCF_libcxx_recursive_mutex_destroy
#endif

/* Locks a recursive mutex, like `pthread_mutex_lock()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_lock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_recursive_mutex_lock  __MCF_libcxx_recursive_mutex_lock
#endif

/* Tries locking a recursive mutex without blocking, like
 * `pthread_mutex_trylock()`.  */
__MCF_LIBCXX_INLINE
bool
__MCF_libcxx_recursive_mutex_trylock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_recursive_mutex_trylock  __MCF_libcxx_recursive_mutex_trylock
#endif

/* Unlocks a recursive mutex, like `pthread_mutex_unlock()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_unlock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_recursive_mutex_unlock  __MCF_libcxx_recursive_mutex_unlock
#endif

/* Initializes a condition variable, like `pthread_cond_init()`.
 * This function is currently unused.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_init(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_condvar_init  __MCF_libcxx_condvar_init
#endif

/* Destroys a condition variable. This function does nothing.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_destroy(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_condvar_destroy  __MCF_libcxx_condvar_destroy
#endif

/* Waits for a condition variable, like `pthread_cond_wait()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_wait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_condvar_wait  __MCF_libcxx_condvar_wait
#endif

/* Waits for a condition variable until a time point, like
 * `pthread_cond_timedwait()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_timedwait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx, const __libcpp_timespec_t* __abs_time) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_condvar_timedwait  __MCF_libcxx_condvar_timedwait
#endif

/* Signals at most one thread that is waiting on the condition variable, like
 * `pthread_cond_signal()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_signal(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_condvar_signal  __MCF_libcxx_condvar_signal
#endif

/* Signals all threads that are waiting on the condition variable, like
 * `pthread_cond_broadcast()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_broadcast(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_condvar_broadcast  __MCF_libcxx_condvar_broadcast
#endif

/* Creates a thread, like `pthread_create()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_thread_create(__libcpp_thread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_thread_create  __MCF_libcxx_thread_create
#endif

/* Awaits a thread to terminate and gets its result, like `pthread_join()`.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_thread_join(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_thread_join  __MCF_libcxx_thread_join
#endif

/* Detaches a thread, like `pthread_detach()`  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_thread_detach(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_thread_detach  __MCF_libcxx_thread_detach
#endif

/* Checks whether a thread object is null.  */
__MCF_LIBCXX_INLINE __MCF_FN_PURE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_isnull(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_thread_isnull  __MCF_libcxx_thread_isnull
#endif

/* Gets a thread itself, like `pthread_self()`.
 * The thread shall be the main thread, or shall have been created by
 * `__libcpp_thread_create()`. Otherwise the behavior is undefined.  */
__MCF_LIBCXX_INLINE __MCF_FN_CONST
__libcpp_thread_id
__MCF_libcxx_thread_get_current_id(void) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_thread_get_current_id  __MCF_libcxx_thread_get_current_id
#endif

/* Gets the ID of another thread.  */
__MCF_LIBCXX_INLINE __MCF_FN_PURE __MCF_CXX11(constexpr)
__libcpp_thread_id
__MCF_libcxx_thread_get_id(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_thread_get_id  __MCF_libcxx_thread_get_id
#endif

/* Checks whether two thread IDs compare equal, like `pthread_equal()`.  */
__MCF_LIBCXX_INLINE __MCF_FN_PURE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_id_equal(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_thread_id_equal  __MCF_libcxx_thread_id_equal
#endif

/* Checks whether two thread IDs compare less, for standard containers.  */
__MCF_LIBCXX_INLINE __MCF_FN_PURE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_id_less(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_thread_id_less  __MCF_libcxx_thread_id_less
#endif

/* Suspends execution of the current thread for an amount of time.  */
__MCF_LIBCXX_IMPORT
void
__MCF_libcxx_thread_sleep_for_ns_count(int64_t __ns) __MCF_NOEXCEPT;

#if defined __cplusplus && (__cplusplus >= 201103L)
__MCF_ALWAYS_INLINE
void
__libcpp_thread_sleep_for(const ::std::chrono::nanoseconds& __ns) __MCF_NOEXCEPT
  {
    __MCF_libcxx_thread_sleep_for_ns_count(__ns.count());
  }
#endif  /* __cplusplus >= 201103L  */

/* Gives up the current time slice, like `sched_yield()`.  */
__MCF_LIBCXX_INLINE
void
__MCF_libcxx_thread_yield(void) __MCF_NOEXCEPT;

#ifndef __MCF_LIBCXX_NO_ALIASES
#  define __libcpp_thread_yield  __MCF_libcxx_thread_yield
#endif

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_LIBCXX_INLINE
int
__MCF_libcxx_execute_once(__libcpp_exec_once_flag* __once, __MCF_once_callback* __init_proc)
  {
    __MCF_gthr_call_once_seh(__once, (__MCF_cxa_dtor_cdecl*)(intptr_t) __init_proc, __MCF_nullptr);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_tls_create(__libcpp_tls_key* __keyp, _MCF_tls_dtor* __dtor_opt) __MCF_NOEXCEPT
  {
    _MCF_tls_key* __key = _MCF_tls_key_new(__dtor_opt);
    *__keyp = __key;
    return (__key == __MCF_nullptr) ? ENOMEM : 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_tls_delete(__libcpp_tls_key __key) __MCF_NOEXCEPT
  {
    _MCF_tls_key_delete(__key);
    return 0;
  }

__MCF_LIBCXX_INLINE
void*
__MCF_libcxx_tls_get(__libcpp_tls_key __key) __MCF_NOEXCEPT
  {
    return _MCF_tls_get(__key);
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_tls_set(__libcpp_tls_key __key, const void* __val_opt) __MCF_NOEXCEPT
  {
    int __err = _MCF_tls_set(__key, __val_opt);
    return (__err != 0) ? EINVAL : 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_mutex_init(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_init(__mtx);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_mutex_destroy(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    (void) __mtx;
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_mutex_lock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_mutex_lock(__mtx, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_LIBCXX_INLINE
bool
__MCF_libcxx_mutex_trylock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return __err == 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_mutex_unlock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_init(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    __MCF_gthr_rc_mutex_init(__rmtx);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_destroy(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    (void) __rmtx;
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_lock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    int __err = __MCF_gthr_rc_mutex_recurse(__rmtx);
    if(__err == 0)
      return 0;

    __err = __MCF_gthr_rc_mutex_wait(__rmtx, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_LIBCXX_INLINE
bool
__MCF_libcxx_recursive_mutex_trylock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    int64_t __timeout;
    int __err = __MCF_gthr_rc_mutex_recurse(__rmtx);
    if(__err == 0)
      return true;

    __timeout = 0;
    __err = __MCF_gthr_rc_mutex_wait(__rmtx, &__timeout);
    return __err == 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_unlock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    __MCF_gthr_rc_mutex_release(__rmtx);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_init(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_init(__cond);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_destroy(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    (void) __cond;
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_wait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_timedwait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx, const __libcpp_timespec_t* __abs_time) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, &__timeout);
    return __err;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_signal(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal(__cond);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_condvar_broadcast(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal_all(__cond);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_thread_create(__libcpp_thread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record __rec[1] = __MCF_0_INIT;
    _MCF_thread* __thrd;

    __rec->__proc = __proc;
    __rec->__arg = __arg;
    __rec->__joinable[0] = 1;

    __thrd = _MCF_thread_new(__MCF_gthr_thread_thunk_v2, __rec, sizeof(*__rec));
    *__thrdp = __thrd;
    return (__thrd == __MCF_nullptr) ? EAGAIN : 0;  /* as specified by POSIX  */
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_thread_join(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  {
    _MCF_thread* __thrd = *__thrdp;
    __MCF_gthr_thread_record* __rec;
    int __err;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if(__thrd->__proc != __MCF_gthr_thread_thunk_v2)
      return EINVAL;

    __rec = (__MCF_gthr_thread_record*) _MCF_thread_get_data(__thrd);

    /* Joining with the calling thread itself would result in deadlocks.  */
    if(__thrd->__tid == _MCF_thread_self_tid())
      return EDEADLK;

    /* Fail if the thread has already been detached.  */
    if(_MCF_atomic_xchg_8_rlx(__rec->__joinable, 0) == 0)
      return EINVAL;

    __err = _MCF_thread_wait(__thrd, __MCF_nullptr);
    __MCF_ASSERT(__err == 0);

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return 0;
  }

__MCF_LIBCXX_INLINE
int
__MCF_libcxx_thread_detach(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  {
    _MCF_thread* __thrd = *__thrdp;
    __MCF_gthr_thread_record* __rec;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if(__thrd->__proc != __MCF_gthr_thread_thunk_v2)
      return EINVAL;

    __rec = (__MCF_gthr_thread_record*) _MCF_thread_get_data(__thrd);

    /* Fail if the thread has already been detached.  */
    if(_MCF_atomic_xchg_8_rlx(__rec->__joinable, 0) == 0)
      return EINVAL;

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return 0;
  }

__MCF_LIBCXX_INLINE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_isnull(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  {
    return *__thrdp == _LIBCPP_NULL_THREAD;
  }

__MCF_LIBCXX_INLINE
__libcpp_thread_id
__MCF_libcxx_thread_get_current_id(void) __MCF_NOEXCEPT
  {
    return _MCF_thread_self_tid();
  }

__MCF_LIBCXX_INLINE __MCF_CXX11(constexpr)
__libcpp_thread_id
__MCF_libcxx_thread_get_id(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  {
    return _MCF_thread_get_tid(*__thrdp);
  }

__MCF_LIBCXX_INLINE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_id_equal(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  {
    return __t1 == __t2;
  }

__MCF_LIBCXX_INLINE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_id_less(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  {
    return __t1 < __t2;
  }

__MCF_LIBCXX_INLINE
void
__MCF_libcxx_thread_yield(void) __MCF_NOEXCEPT
  {
    _MCF_yield();
  }

__MCF_C_DECLARATIONS_END
#ifdef _LIBCPP_END_NAMESPACE_STD
_LIBCPP_END_NAMESPACE_STD
#endif  /* inside libc++  */
#endif  /* __MCFGTHREAD_LIBCXX_  */
