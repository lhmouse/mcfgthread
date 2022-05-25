/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_LIBCPP_
#define __MCFGTHREAD_LIBCPP_

#include "fwd.h"
#include "gthr_aux.h"

#if defined(__cplusplus) && (__cplusplus >= 201103L)
#  include <chrono>
#endif

#ifdef __cplusplus
_LIBCPP_BEGIN_NAMESPACE_STD
extern "C" {
#endif

/* Define wrappers as required by '__threading_support'.  */
#ifndef __MCF_DECLSPEC_LIBCPP
#  define __MCF_DECLSPEC_LIBCPP(...)  __VA_ARGS__
#endif

/* Define threading types. These usually map to our APIs directly, except
 * the recursive mutex.  */
typedef struct timespec __libcpp_timespec_t;
typedef uint32_t __libcpp_thread_id;
typedef _MCF_thread* __libcpp_thread_t;
typedef _MCF_tls_key* __libcpp_tls_key;

typedef _MCF_once __libcpp_exec_once_flag;
typedef _MCF_cond __libcpp_condvar_t;
typedef _MCF_mutex __libcpp_mutex_t;
typedef __MCF_gthr_rc_mutex __libcpp_recursive_mutex_t;

/* Define macros for static and dynamic initialization.  */
#define _LIBCPP_NULL_THREAD   NULL
#define _LIBCPP_TLS_DESTRUCTOR_CC   /* use default  */

#define _LIBCPP_EXEC_ONCE_INITIALIZER   __MCF_0_INIT
#define _LIBCPP_CONDVAR_INITIALIZER   __MCF_0_INIT
#define _LIBCPP_MUTEX_INITIALIZER   __MCF_0_INIT

/* Initializes a recursive mutex, like `pthread_mutex_init()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_recursive_mutex_init(__libcpp_recursive_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_recursive_mutex_init(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    __MCF_gthr_rc_mutex_init(__rmtx);
    return 0;
  }

static __inline__
int
__libcpp_recursive_mutex_init(__libcpp_recursive_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_recursive_mutex_init(__mtx);
  }

/* Destroys a recursive mutex. This function does nothing.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_recursive_mutex_destroy(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_recursive_mutex_destroy(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    (void) __rmtx;
    return 0;
  }

static __inline__
int
__libcpp_recursive_mutex_destroy(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_recursive_mutex_destroy(__rmtx);
  }

/* Locks a recursive mutex, like `pthread_mutex_lock()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
bool
__MCF_libcpp_recursive_mutex_lock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
bool
__MCF_libcpp_recursive_mutex_lock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    int __err = __MCF_gthr_rc_mutex_recurse(__rmtx);
    if(__err == 0)
      return true;

    __err = __MCF_gthr_rc_mutex_wait(__rmtx, NULL);
    __MCF_ASSERT(__err == 0);
    return true;
  }

static __inline__
bool
__libcpp_recursive_mutex_lock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_recursive_mutex_lock(__rmtx);
  }

/* Tries locking a recursive mutex without blocking, like
 * `pthread_mutex_trylock()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
bool
__MCF_libcpp_recursive_mutex_trylock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
bool
__MCF_libcpp_recursive_mutex_trylock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    int64_t __timeout;
    int __err = __MCF_gthr_rc_mutex_recurse(__rmtx);
    if(__err == 0)
      return true;

    __timeout = 0;
    __err = __MCF_gthr_rc_mutex_wait(__rmtx, &__timeout);
    return __err == 0;
  }

static __inline__
bool
__libcpp_recursive_mutex_trylock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_recursive_mutex_trylock(__rmtx);
  }

/* Unlocks a recursive mutex, like `pthread_mutex_unlock()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_recursive_mutex_unlock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_recursive_mutex_unlock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    __MCF_gthr_rc_mutex_release(__rmtx);
    return 0;
  }

static __inline__
int
__libcpp_recursive_mutex_unlock(__libcpp_recursive_mutex_t* __rmtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_recursive_mutex_unlock(__rmtx);
  }

/* Initializes a mutex, like `pthread_mutex_init()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_mutex_init(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_mutex_init(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_init(__mtx);
    return 0;
  }

static __inline__
int
__libcpp_mutex_init(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_mutex_init(__mtx);
  }

/* Destroys a mutex. This function does nothing.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_mutex_destroy(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_mutex_destroy(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    (void) __mtx;
    return 0;
  }

static __inline__
int
__libcpp_mutex_destroy(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_mutex_destroy(__mtx);
  }

/* Locks a mutex, like `pthread_mutex_lock()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
bool
__MCF_libcpp_mutex_lock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
bool
__MCF_libcpp_mutex_lock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_mutex_lock(__mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return true;
  }

static __inline__
bool
__libcpp_mutex_lock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_mutex_lock(__mtx);
  }

/* Tries locking a mutex without blocking, like `pthread_mutex_trylock()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
bool
__MCF_libcpp_mutex_trylock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
bool
__MCF_libcpp_mutex_trylock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int64_t __timeout = 0;
    int __err = _MCF_mutex_lock(__mtx, &__timeout);
    return __err == 0;
  }

static __inline__
bool
__libcpp_mutex_trylock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_mutex_trylock(__mtx);
  }

/* Unlocks a mutex, like `pthread_mutex_unlock()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_mutex_unlock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_mutex_unlock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    _MCF_mutex_unlock(__mtx);
    return 0;
  }

static __inline__
int
__libcpp_mutex_unlock(__libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_mutex_unlock(__mtx);
  }

/* Initializes a condition variable, like `pthread_cond_init()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_init(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_init(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_init(__cond);
    return 0;
  }

static __inline__
int
__libcpp_condvar_init(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_condvar_init(__cond);
  }

/* Destroys a condition variable. This function does nothing.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_destroy(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_destroy(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    (void) __cond;
    return 0;
  }

static __inline__
int
__libcpp_condvar_destroy(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_condvar_destroy(__cond);
  }

/* Waits for a condition variable, like `pthread_cond_wait()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_wait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_wait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, NULL);
    __MCF_ASSERT(__err == 0);
    return 0;
  }

static __inline__
int
__libcpp_condvar_wait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_condvar_wait(__cond, __mtx);
  }

/* Waits for a condition variable until a time point, like
 * `pthread_cond_timedwait()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_timedwait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx, const __libcpp_timespec_t* __abs_time) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_timedwait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx, const __libcpp_timespec_t* __abs_time) __MCF_NOEXCEPT
  {
    int64_t __timeout = __MCF_gthr_timeout_from_timespec(__abs_time);
    int __err = _MCF_cond_wait(__cond, __MCF_gthr_mutex_unlock_callback, __MCF_gthr_mutex_relock_callback, (intptr_t) __mtx, &__timeout);
    return __err;
  }

static __inline__
int
__libcpp_condvar_timedwait(__libcpp_condvar_t* __cond, __libcpp_mutex_t* __mtx, const __libcpp_timespec_t* __abs_time) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_condvar_timedwait(__cond, __mtx, __abs_time);
  }

/* Signals at most one thread that is waiting on the condition variable, like
 * `pthread_cond_signal()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_signal(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_signal(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal(__cond);
    return 0;
  }

static __inline__
int
__libcpp_condvar_signal(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_condvar_signal(__cond);
  }

/* Signals all threads that are waiting on the condition variable, like
 * `pthread_cond_broadcast()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_broadcast(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_condvar_broadcast(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond_signal_all(__cond);
    return 0;
  }

static __inline__
int
__libcpp_condvar_broadcast(__libcpp_condvar_t* __cond) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_condvar_broadcast(__cond);
  }

/* Performs one-time initialization, like `pthread_once()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_execute_once(__libcpp_exec_once_flag* __once, void __init_proc(void));

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_execute_once(__libcpp_exec_once_flag* __once, void __init_proc(void))
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
__libcpp_execute_once(__libcpp_exec_once_flag* __once, void __init_proc(void))
  {
    return __MCF_libcpp_execute_once(__once, __init_proc);
  }

/* Creates a thread, like `pthread_create()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_thread_create_v2(__libcpp_thread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_thread_create_v2(__libcpp_thread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record __rec[1];
    _MCF_thread* __thrd;

    __rec->__proc = __proc;
    __rec->__arg = __arg;
    __rec->__joinable = 1;

    __thrd = _MCF_thread_new(__MCF_gthr_thread_thunk_v2, __rec, sizeof(*__rec));
    *__thrdp = __thrd;
    return (__thrd == NULL) ? -1 : 0;
  }

static __inline__
int
__libcpp_thread_create(__libcpp_thread_t* __thrdp, __MCF_gthr_thread_procedure* __proc, void* __arg) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_thread_create_v2(__thrdp, __proc, __arg);
  }

/* Awaits a thread to terminate and gets its result, like `pthread_join()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_thread_join_v2(__libcpp_thread_t __thrd, void** __resp_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_thread_join_v2(__libcpp_thread_t __thrd, void** __resp_opt) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record* __rec;
    int __err;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if(__thrd->__proc != __MCF_gthr_thread_thunk_v2)
      return -1;

    __rec = (__MCF_gthr_thread_record*) _MCF_thread_get_data(__thrd);

    /* Fail if the thread has already been detached.  */
    if(_MCF_atomic_xchg_8_rlx(&(__rec->__joinable), 0) == 0)
      return -1;

    /* Wait for it.  */
    if(__thrd == _MCF_thread_self())
      return -2;

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
__libcpp_thread_join(__libcpp_thread_t __thrd, void** __resp_opt) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_thread_join_v2(__thrd, __resp_opt);
  }

/* Detaches a thread, like `pthread_detach()`  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_thread_detach_v2(__libcpp_thread_t __thrd) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_thread_detach_v2(__libcpp_thread_t __thrd) __MCF_NOEXCEPT
  {
    __MCF_gthr_thread_record* __rec;

    /* As there is no type information, we examine the thread procedure to
     * ensure we don't mistake a thread of a wrong type.  */
    if(__thrd->__proc != __MCF_gthr_thread_thunk_v2)
      return -1;

    __rec = (__MCF_gthr_thread_record*) _MCF_thread_get_data(__thrd);

    /* Fail if the thread has already been detached.  */
    if(_MCF_atomic_xchg_8_rlx(&(__rec->__joinable), 0) == 0)
      return -1;

    /* Free the thread.  */
    _MCF_thread_drop_ref(__thrd);
    return 0;
  }

static __inline__
int
__libcpp_thread_detach(__libcpp_thread_t __thrd) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_thread_detach_v2(__thrd);
  }

/* Compares two thread IDs.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE) __MCF_CXX11(constexpr)
bool
__MCF_libcpp_thread_id_equal(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE) __MCF_CXX11(constexpr)
bool
__MCF_libcpp_thread_id_equal(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  {
    return __t1 == __t2;
  }

static __inline__ __MCF_CXX11(constexpr)
bool
__libcpp_thread_id_equal(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_thread_id_equal(__t1, __t2);
  }

/* Compares two thread IDs.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE) __MCF_CXX11(constexpr)
bool
__MCF_libcpp_thread_id_less(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE) __MCF_CXX11(constexpr)
bool
__MCF_libcpp_thread_id_less(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  {
    return __t1 < __t2;
  }

static __inline__ __MCF_CXX11(constexpr)
bool
__libcpp_thread_id_less(__libcpp_thread_id __t1, __libcpp_thread_id __t2) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_thread_id_less(__t1, __t2);
  }

/* Checks whether a thread handle is null.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE) __MCF_CXX11(constexpr)
bool
__MCF_libcpp_thread_isnull(const __libcpp_thread_t* __thrd) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE) __MCF_CXX11(constexpr)
bool
__MCF_libcpp_thread_isnull(const __libcpp_thread_t* __thrd) __MCF_NOEXCEPT
  {
    return *__thrd == NULL;
  }

static __inline__ __MCF_CXX11(constexpr)
bool
__libcpp_thread_isnull(const __libcpp_thread_t* __thrd) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_thread_isnull(__thrd);
  }

/* Gets the current thread ID.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
__libcpp_thread_id
__MCF_libcpp_thread_get_current_id(void) __MCF_NOEXCEPT
  __attribute__((__const__));

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
__libcpp_thread_id
__MCF_libcpp_thread_get_current_id(void) __MCF_NOEXCEPT
  {
    return _MCF_thread_self_tid();
  }

static __inline__
__libcpp_thread_id
__libcpp_thread_get_current_id(void) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_thread_get_current_id();
  }

/* Gets the ID of a thread by its handle.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
__libcpp_thread_id
__MCF_libcpp_thread_get_id(const __libcpp_thread_t* __thrd) __MCF_NOEXCEPT
  __attribute__((__const__));

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
__libcpp_thread_id
__MCF_libcpp_thread_get_id(const __libcpp_thread_t* __thrd) __MCF_NOEXCEPT
  {
    return _MCF_thread_get_tid(*__thrd);
  }

static __inline__
__libcpp_thread_id
__libcpp_thread_get_id(const __libcpp_thread_t* __thrd) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_thread_get_id(__thrd);
  }

/* Gives up the current time slice, like `sched_yield()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
void
__MCF_libcpp_thread_yield(void) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
void
__MCF_libcpp_thread_yield(void) __MCF_NOEXCEPT
  {
    _MCF_yield();
  }

static __inline__
void
__libcpp_thread_yield(void) __MCF_NOEXCEPT
  {
    __MCF_libcpp_thread_yield();
  }

__MCF_DECLSPEC_LIBCPP()
void
__MCF_libcpp_thread_sleep_for(double __ns) __MCF_NOEXCEPT;

#if defined(__cplusplus) && (__cplusplus >= 201103L)
/* Suspends the current thread for the given interval.  */
__MCF_ALWAYS_INLINE
void
__libcpp_thread_sleep_for(const ::std::chrono::nanoseconds& __ns) __MCF_NOEXCEPT
  {
    __MCF_libcpp_thread_sleep_for((double) __ns.count() * 0.000001);
  }
#endif

/* Allocates a thread-specific key, like `pthread_key_create()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_tls_create(__libcpp_tls_key* __keyp, void __dtor_opt(void*)) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_tls_create(__libcpp_tls_key* __keyp, void __dtor_opt(void*)) __MCF_NOEXCEPT
  {
    _MCF_tls_key* __key = _MCF_tls_key_new(__dtor_opt);
    *__keyp = __key;
    return (__key == NULL) ? -1 : 0;
  }

static __inline__
int
__libcpp_tls_create(__libcpp_tls_key* __keyp, void __dtor_opt(void*)) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_tls_create(__keyp, __dtor_opt);
  }

/* Destroys a thread-specific key, like `pthread_key_delete()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_key_delete(__libcpp_tls_key __key) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_key_delete(__libcpp_tls_key __key) __MCF_NOEXCEPT
  {
    _MCF_tls_key_delete(__key);
    return 0;
  }

static __inline__
int
__libcpp_key_delete(__libcpp_tls_key __key) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_key_delete(__key);
  }

/* Gets a thread-specific value, like `pthread_getspecific()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
void*
__MCF_libcpp_tls_get(__libcpp_tls_key __key) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
void*
__MCF_libcpp_tls_get(__libcpp_tls_key __key) __MCF_NOEXCEPT
  {
    return _MCF_tls_get(__key);
  }

static __inline__
void*
__libcpp_tls_get(__libcpp_tls_key __key) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_tls_get(__key);
  }

/* Sets a thread-specific value, like `pthread_setspecific()`.  */
__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_tls_set(__libcpp_tls_key __key, const void* __val_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_LIBCPP(__MCF_GNU_INLINE)
int
__MCF_libcpp_tls_set(__libcpp_tls_key __key, const void* __val_opt) __MCF_NOEXCEPT
  {
    int __err = _MCF_tls_set(__key, __val_opt);
    return (__err != 0) ? -1 : 0;
  }

static __inline__
int
__libcpp_tls_set(__libcpp_tls_key __key, const void* __val_opt) __MCF_NOEXCEPT
  {
    return __MCF_libcpp_tls_set(__key, __val_opt);
  }

#ifdef __cplusplus
}
_LIBCPP_END_NAMESPACE_STD
#endif

#endif  /* __MCFGTHREAD_LIBCPP_  */
