/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_LIBCXX_
#define __MCFGTHREAD_LIBCXX_

#include "fwd.h"
#include "gthr_aux.h"
#include <errno.h>
#ifdef __cplusplus
#  include <chrono>
#endif

#ifdef _LIBCPP_BEGIN_NAMESPACE_STD
_LIBCPP_BEGIN_NAMESPACE_STD
#endif  /* inside libc++  */
__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_DECLSPEC_LIBCXX_IMPORT
#  define __MCF_DECLSPEC_LIBCXX_IMPORT
#  define __MCF_DECLSPEC_LIBCXX_INLINE  __MCF_GNU_INLINE
#endif

/* Define threading support types for libc++.  */
typedef struct timespec __libcpp_timespec_t;
typedef _MCF_thread* __libcpp_thread_t;
typedef uint32_t __libcpp_thread_id;
typedef _MCF_tls_key* __libcpp_tls_key;

typedef _MCF_once __libcpp_exec_once_flag;
typedef _MCF_cond __libcpp_condvar_t;
typedef _MCF_mutex __libcpp_mutex_t;
typedef __MCF_gthr_rc_mutex __libcpp_recursive_mutex_t;

/* Define macros for initialization.  */
#define _LIBCPP_NULL_THREAD   NULL
#define _LIBCPP_TLS_DESTRUCTOR_CC  /* default calling convention  */
#define _LIBCPP_EXEC_ONCE_INITIALIZER   __MCF_0_INIT
#define _LIBCPP_CONDVAR_INITIALIZER   __MCF_0_INIT
#define _LIBCPP_MUTEX_INITIALIZER   __MCF_0_INIT

/* Performs one-time initialization, like `pthread_once()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_execute_once(__libcpp_exec_once_flag* __once, __MCF_once_callback* __init_proc);

__MCF_ALIAS(__libcpp_execute_once, __MCF_libcxx_execute_once);

/* Allocates a thread-specific key, like `pthread_key_create()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_tls_create(__libcpp_tls_key* __keyp, _MCF_tls_dtor* __dtor_opt) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_tls_create, __MCF_libcxx_tls_create);

/* Destroys a thread-specific key, like `pthread_key_delete()`.
 * This function is currently unused.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_tls_delete(__libcpp_tls_key __key) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_tls_delete, __MCF_libcxx_tls_delete);

/* Gets a thread-specific value, like `pthread_getspecific()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
void*
__MCF_libcxx_tls_get(__libcpp_tls_key __key) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_ALIAS(__libcpp_tls_get, __MCF_libcxx_tls_get);

/* Sets a thread-specific value, like `pthread_setspecific()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_tls_set(__libcpp_tls_key __key, const void* __val_opt) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_tls_set, __MCF_libcxx_tls_set);

/* Initializes a mutex, like `pthread_mutex_init()`.
 * This function is currently unused.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_mutex_init(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_mutex_init, __MCF_libcxx_mutex_init);

/* Destroys a mutex. This function does nothing.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_mutex_destroy(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_mutex_destroy, __MCF_libcxx_mutex_destroy);

/* Locks a mutex, like `pthread_mutex_lock()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_mutex_lock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_mutex_lock, __MCF_libcxx_mutex_lock);

/* Tries locking a mutex without blocking, like `pthread_mutex_trylock()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
bool
__MCF_libcxx_mutex_trylock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_mutex_trylock, __MCF_libcxx_mutex_trylock);

/* Unlocks a mutex, like `pthread_mutex_unlock()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_mutex_unlock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_mutex_unlock, __MCF_libcxx_mutex_unlock);

/* Initializes a recursive mutex, like `pthread_mutex_init()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_init(__libcpp_recursive_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_recursive_mutex_init, __MCF_libcxx_recursive_mutex_init);

/* Destroys a recursive mutex. This function does nothing.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_destroy(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_recursive_mutex_destroy, __MCF_libcxx_recursive_mutex_destroy);

/* Locks a recursive mutex, like `pthread_mutex_lock()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_lock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_recursive_mutex_lock, __MCF_libcxx_recursive_mutex_lock);

/* Tries locking a recursive mutex without blocking, like
 * `pthread_mutex_trylock()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
bool
__MCF_libcxx_recursive_mutex_trylock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_recursive_mutex_trylock, __MCF_libcxx_recursive_mutex_trylock);

/* Unlocks a recursive mutex, like `pthread_mutex_unlock()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_unlock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_recursive_mutex_unlock, __MCF_libcxx_recursive_mutex_unlock);

/* Initializes a condition variable, like `pthread_cond_init()`.
 * This function is currently unused.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_init(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_condvar_init, __MCF_libcxx_condvar_init);

/* Destroys a condition variable. This function does nothing.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_destroy(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_condvar_destroy, __MCF_libcxx_condvar_destroy);

/* Waits for a condition variable, like `pthread_cond_wait()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_wait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_condvar_wait, __MCF_libcxx_condvar_wait);

/* Waits for a condition variable until a time point, like
 * `pthread_cond_timedwait()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_timedwait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx, const __libcpp_timespec_t* __abs_time) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_condvar_timedwait, __MCF_libcxx_condvar_timedwait);

/* Signals at most one thread that is waiting on the condition variable, like
 * `pthread_cond_signal()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_signal(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_condvar_signal, __MCF_libcxx_condvar_signal);

/* Signals all threads that are waiting on the condition variable, like
 * `pthread_cond_broadcast()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_broadcast(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_condvar_broadcast, __MCF_libcxx_condvar_broadcast);

/* Creates a thread, like `pthread_create()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_thread_create(__libcpp_thread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_thread_create, __MCF_libcxx_thread_create);

/* Awaits a thread to terminate and gets its result, like `pthread_join()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_thread_join(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_thread_join, __MCF_libcxx_thread_join);

/* Detaches a thread, like `pthread_detach()`  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_thread_detach(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_thread_detach, __MCF_libcxx_thread_detach);

/* Checks whether a thread object is null.  */
__MCF_DECLSPEC_LIBCXX_INLINE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_isnull(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_ALIAS(__libcpp_thread_isnull, __MCF_libcxx_thread_isnull);

/* Gets a thread itself, like `pthread_self()`.
 * The thread shall be the main thread, or shall have been created by
 * `__libcpp_thread_create()`. Otherwise the behavior is undefined.  */
__MCF_DECLSPEC_LIBCXX_INLINE
__libcpp_thread_id
__MCF_libcxx_thread_get_current_id(void) __MCF_NOEXCEPT
  __attribute__((__const__));

__MCF_ALIAS(__libcpp_thread_get_current_id, __MCF_libcxx_thread_get_current_id);

/* Gets the ID of another thread.  */
__MCF_DECLSPEC_LIBCXX_INLINE __MCF_CXX11(constexpr)
__libcpp_thread_id
__MCF_libcxx_thread_get_id(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_ALIAS(__libcpp_thread_get_id, __MCF_libcxx_thread_get_id);

/* Checks whether two thread IDs compare equal, like `pthread_equal()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_id_equal(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_ALIAS(__libcpp_thread_id_equal, __MCF_libcxx_thread_id_equal);

/* Checks whether two thread IDs compare less, for standard containers.  */
__MCF_DECLSPEC_LIBCXX_INLINE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_id_less(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_ALIAS(__libcpp_thread_id_less, __MCF_libcxx_thread_id_less);

/* Suspends execution of the current thread for an amount of time.  */
__MCF_DECLSPEC_LIBCXX_IMPORT
void
__MCF_libcxx_thread_sleep_for_ns_count(int64_t __ns) __MCF_NOEXCEPT;

#ifdef __cplusplus
__MCF_ALWAYS_INLINE
void
__libcpp_thread_sleep_for(const chrono::nanoseconds& __ns) __MCF_NOEXCEPT
  {
    __MCF_libcxx_thread_sleep_for_ns_count(__ns.count());
  }
#endif  /* __cplusplus  */

/* Gives up the current time slice, like `sched_yield()`.  */
__MCF_DECLSPEC_LIBCXX_INLINE
void
__MCF_libcxx_thread_yield(void) __MCF_NOEXCEPT;

__MCF_ALIAS(__libcpp_thread_yield, __MCF_libcxx_thread_yield);

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_execute_once(__libcpp_exec_once_flag* __once, __MCF_once_callback* __init_proc)
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

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_tls_create(__libcpp_tls_key* __keyp, _MCF_tls_dtor* __dtor_opt) __MCF_NOEXCEPT
  {
    _MCF_tls_key* __key = _MCF_tls_key_new(__dtor_opt);
    *__keyp = __key;
    return (__key == NULL) ? ENOMEM : 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_tls_delete(__libcpp_tls_key __key) __MCF_NOEXCEPT
  {
    _MCF_tls_key_delete(__key);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
void*
__MCF_libcxx_tls_get(__libcpp_tls_key __key) __MCF_NOEXCEPT
  {
    return _MCF_tls_get(__key);
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_tls_set(__libcpp_tls_key __key, const void* __val_opt) __MCF_NOEXCEPT
  {
    int __err = _MCF_tls_set(__key, __val_opt);
    return (__err != 0) ? EINVAL : 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_mutex_init(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_init(__mtx);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_mutex_destroy(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    (void) __mtx;
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_mutex_lock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_mutex_lock(__mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
bool
__MCF_libcxx_mutex_trylock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return __err == 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_mutex_unlock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_init(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    __MCF_gthr_rc_mutex_init(__rmtx);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_destroy(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    (void) __rmtx;
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_lock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    int __err = __MCF_gthr_rc_mutex_recurse(__rmtx);
    if(__err == 0)
      return 0;

    __err = __MCF_gthr_rc_mutex_wait(__rmtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
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

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_recursive_mutex_unlock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    __MCF_gthr_rc_mutex_release(__rmtx);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_init(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_init(__cond);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_destroy(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    (void) __cond;
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_wait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_timedwait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx, const __libcpp_timespec_t* __abs_time) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, &__timeout);
    return __err;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_signal(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal(__cond);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_condvar_broadcast(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal_all(__cond);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
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
    return (__thrd == NULL) ? EAGAIN : 0;  /* as specified by POSIX  */
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_thread_join(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record* __rec;
    int __err;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if((*__thrdp)->__proc != __MCF_gthr_thread_thunk_v2)
      return EINVAL;

    /* Fail if the thread has already been detached.  */
    __rec = (__MCF_gthr_thread_record*) _MCF_thread_get_data(*__thrdp);
    if(_MCF_atomic_xchg_8_rlx(__rec->__joinable, 0) == 0)
      return EINVAL;

    /* Wait for it.  */
    if(*__thrdp == _MCF_thread_self())
      return EDEADLK;

    __err = _MCF_thread_wait(*__thrdp, NULL);
    __MCF_ASSERT(__err == 0);

    /* Free the thread.  */
    _MCF_thread_drop_ref(*__thrdp);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
int
__MCF_libcxx_thread_detach(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record* __rec;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if((*__thrdp)->__proc != __MCF_gthr_thread_thunk_v2)
      return EINVAL;

    /* Fail if the thread has already been detached.  */
    __rec = (__MCF_gthr_thread_record*) _MCF_thread_get_data(*__thrdp);
    if(_MCF_atomic_xchg_8_rlx(__rec->__joinable, 0) == 0)
      return EINVAL;

    /* Free the thread.  */
    _MCF_thread_drop_ref(*__thrdp);
    return 0;
  }

__MCF_DECLSPEC_LIBCXX_INLINE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_isnull(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  {
    return *__thrdp == _LIBCPP_NULL_THREAD;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
__libcpp_thread_id
__MCF_libcxx_thread_get_current_id(void) __MCF_NOEXCEPT
  {
    return _MCF_thread_self_tid();
  }

__MCF_DECLSPEC_LIBCXX_INLINE __MCF_CXX11(constexpr)
__libcpp_thread_id
__MCF_libcxx_thread_get_id(const __libcpp_thread_t* __thrdp) __MCF_NOEXCEPT
  {
    return _MCF_thread_get_tid(*__thrdp);
  }

__MCF_DECLSPEC_LIBCXX_INLINE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_id_equal(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  {
    return __t1 == __t2;
  }

__MCF_DECLSPEC_LIBCXX_INLINE __MCF_CXX11(constexpr)
bool
__MCF_libcxx_thread_id_less(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  {
    return __t1 < __t2;
  }

__MCF_DECLSPEC_LIBCXX_INLINE
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
