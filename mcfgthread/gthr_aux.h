/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_GTHR_AUX_
#define __MCFGTHREAD_GTHR_AUX_

#include "fwd.h"
#include "once.h"
#include "cond.h"
#include "mutex.h"
#include "shared_mutex.h"
#include "thread.h"
#include <time.h>  /* struct timespec  */

__MCF_CXX(extern "C" {)
#ifndef __MCF_GTHR_AUX_IMPORT
#  define __MCF_GTHR_AUX_IMPORT
#  define __MCF_GTHR_AUX_INLINE  __MCF_GNU_INLINE
#endif

/* The types may be reused by other headers.  */
typedef struct __MCF_gthr_rc_mutex __MCF_gthr_rc_mutex;
typedef struct __MCF_gthr_thread_record __MCF_gthr_thread_record;
typedef void __MCF_gthr_once_callback(void);
typedef void* __MCF_gthr_thread_fn(void* __arg);

/* This structure defines a minimal recursive mutex.  */
struct __MCF_gthr_rc_mutex
  {
    __MCF_BR(int32_t) __owner;  /* owner thread ID  */
    int __depth;  /* recursion depth  */
    __MCF_BR(_MCF_mutex) __mutex;
  };

/* This structure is used to implement POSIX threads.  */
struct __MCF_gthr_thread_record
  {
    uint8_t __magic_guid[16];
    __MCF_gthr_thread_fn* __proc;
    void* __arg_or_result;
  };

/* If a thread exits with an exit unwind, then `__gthread_join()` stores this
 * value into `*__resp_opt`.  */
#define __MCF_GTHR_CANCELLED   ((void*) -1)

/* Implements `__gthread_once()`.
 *
 * At the moment (2026-07-16) GCC does not support SEH on i686.
 *
 * @param `once` shall point to a once-initialization flag.
 * @param `init_proc` points to the function that should be called exactly once.
 * @param `arg` is a user-defined parameter for `init_proc`.
 * @returns nothing.  */
__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_call_once_seh(_MCF_once* __once, __MCF_cxa_dtor_any_ __init_proc, void* __arg)
  __MCF_MAY_THROW;

/* Converts an absolute time point of `struct timespec` to the number of
 * milliseconds since 1970-01-01T00:00:00Z.
 *
 * If the value is negative, it will be clamped to zero. If the value would
 * overflow, it will be clamped to the maximum value.
 *
 * @param `abs_time` points to a time point.
 * @returns the number of milliseconds since 1970-01-01T00:00:00Z.  */
__MCF_GTHR_AUX_IMPORT __MCF_FN_PURE
int64_t
__MCF_gthr_timeout_from_timespec(const __MCF_timespec* __abs_time)
  __MCF_noexcept;

/* Initializes a recursive mutex.
 *
 * @param `rmtx` points to a recursive mutex to initialize.
 * @returns nothing.  */
__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_rc_mutex_init(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept;

/* Tests whether the current thread owns a recursive mutex.
 *
 * If the current thread owns the recursive mutex, this function increments the
 * recursion depth and succeeds; otherwise this function fails without modifying
 * the state of the mutex.
 *
 * @param `rmtx` points to a recursive mutex to test.
 * @returns 0 if the current thread owns the mutex, or -1 otherwise.  */
__MCF_GTHR_AUX_INLINE
int
__MCF_gthr_rc_mutex_recurse(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept;

/* Waits for a recursive mutex to become available, and locks it.
 *
 * This function waits for the owner of the recursive mutex to release it, then
 * takes ownership of the mutex, and sets the recursion depth to 1. The calling
 * thread shall not have owned the mutex before calling this function; otherwise
 * a deadlock may occur.
 *
 * @param `rmtx` points to a recursive mutex to wait for.
 * @returns 0 if the current thread has acquired ownership of the mutex, or -1
 *     if the operation has timed out.  */
__MCF_GTHR_AUX_INLINE
int
__MCF_gthr_rc_mutex_wait(__MCF_gthr_rc_mutex* __rmtx, const int64_t* __timeout_opt)
  __MCF_noexcept;

/* Unlocks a recursive mutex.
 *
 * This function decrements the recursion depth of the recursive mutex, and if
 * the recursion depth is reduced to zero, releases the ownership of the mutex.
 * The calling thread shall have owned the mutex before calling this function;
 * otherwise the behavior is undefined.
 *
 * @param `rmtx` points to a recursive mutex to unlock.
 * @returns nothing.  */
__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_rc_mutex_release(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept;

/* This callback is to be used with condition variables. The argument is a
 * pointer to an object of type `_MCF_mutex`.  */
__MCF_GTHR_AUX_IMPORT
intptr_t
__MCF_gthr_mutex_unlock_callback(intptr_t __arg)
  __MCF_noexcept;

/* This callback is to be used with condition variables. The argument is a
 * pointer to an object of type `_MCF_mutex`.  */
__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked)
  __MCF_noexcept;

/* Suspends the current thread on a condition variable.
 *
 * This function unlocks the mutex and suspends the current thread, until either
 * the condition variable is signaled, or the given timeout has been exceeded.
 * In either case, this function relocks the mutex before returning. The calling
 * thread shall have owned the mutex before calling this function; otherwise the
 * behavior is undefined.
 *
 * @param `cnd` points to the condition variable to wait on.
 * @param `mtx` points to the mutex to use.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns 0 if the condition variable was signaled or there was a spurious
 *     wakeup, or -1 if the wait operation has timed out.  */
__MCF_GTHR_AUX_IMPORT
int
__MCF_gthr_cond_mutex_wait(_MCF_cond* __cnd, _MCF_mutex* __mtx, const int64_t* __timeout_opt)
  __MCF_noexcept;

/* This callback is to be used with condition variables. The argument is a
 * pointer to an object of type `_MCF_shared_mutex`.  */
__MCF_GTHR_AUX_IMPORT
intptr_t
__MCF_gthr_shared_mutex_unlock_callback(intptr_t __arg)
  __MCF_noexcept;

/* This callback is to be used with condition variables. The argument is a
 * pointer to an object of type `_MCF_shared_mutex`.  */
__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_shared_mutex_relock_shared_callback(intptr_t __arg, intptr_t __unlocked)
  __MCF_noexcept;

/* This callback is to be used with condition variables. The argument is a
 * pointer to an object of type `_MCF_shared_mutex`.  */
__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_shared_mutex_relock_exclusive_callback(intptr_t __arg, intptr_t __unlocked)
  __MCF_noexcept;

/* Suspends the current thread on a condition variable.
 *
 * This function unlocks the shared mutex and suspends the current thread, until
 * either the condition variable is signaled, or the given timeout has been
 * exceeded. In either case, this function relocks the shared mutex in shared
 * mode before returning. The calling thread shall have owned the shared mutex
 * in shared mode before calling this function; otherwise the behavior is
 * undefined.
 *
 * @param `cnd` points to the condition variable to wait on.
 * @param `smtx` points to the shared mutex to use.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns 0 if the condition variable was signaled or there was a spurious
 *     wakeup, or -1 if the wait operation has timed out.  */
__MCF_GTHR_AUX_IMPORT
int
__MCF_gthr_cond_shared_mutex_wait_shared(_MCF_cond* __cnd, _MCF_shared_mutex* __smtx,
                                         const int64_t* __timeout_opt)
  __MCF_noexcept;

/* Suspends the current thread on a condition variable.
 *
 * This function unlocks the shared mutex and suspends the current thread, until
 * either the condition variable is signaled, or the given timeout has been
 * exceeded. In either case, this function relocks the shared mutex in exclusive
 * mode before returning. The calling thread shall have owned the shared mutex
 * in exclusive mode before calling this function; otherwise the behavior is
 * undefined.
 *
 * @param `cnd` points to the condition variable to wait on.
 * @param `smtx` points to the shared mutex to use.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns 0 if the condition variable was signaled or there was a spurious
 *     wakeup, or -1 if the wait operation has timed out.  */
__MCF_GTHR_AUX_IMPORT
int
__MCF_gthr_cond_shared_mutex_wait_exclusive(_MCF_cond* __cnd, _MCF_shared_mutex* __smtx,
                                            const int64_t* __timeout_opt)
  __MCF_noexcept;

/* This callback is to be used with condition variables. The argument is a
 * pointer to an object of type `__MCF_gthr_rc_mutex`.  */
__MCF_GTHR_AUX_IMPORT
intptr_t
__MCF_gthr_recursive_mutex_unlock_callback(intptr_t __arg)
  __MCF_noexcept;

/* This callback is to be used with condition variables. The argument is a
 * pointer to an object of type `__MCF_gthr_rc_mutex`.  */
__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_recursive_mutex_relock_callback(intptr_t __arg, intptr_t __unlocked)
  __MCF_noexcept;

/* Suspends the current thread on a condition variable.
 *
 * This function sets the recursion depth to 0 and unlocks the recursive mutex,
 * and then suspends the current thread, until either the condition variable is
 * signaled, or the given timeout has been exceeded. In either case, this
 * function relocks the recursive mutex and restores the recursion depth before
 * returning. The calling thread shall have owned the recursive mutex before
 * calling this function; otherwise the behavior is undefined.
 *
 * @param `cnd` points to the condition variable to wait on.
 * @param `rmtx` points to the recursive mutex to use.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns 0 if the condition variable was signaled or there was a spurious
 *     wakeup, or -1 if the wait operation has timed out.  */
__MCF_GTHR_AUX_IMPORT
int
__MCF_gthr_cond_recursive_mutex_wait(_MCF_cond* __cnd, __MCF_gthr_rc_mutex* __rmtx,
                                     const int64_t* __timeout_opt)
  __MCF_noexcept;

/* Creates a thread.
 *
 * @param `proc` shall point to the thread procedure.
 * @param `arg` is a user-defined parameter for the thread procedure.
 * @returns a pointer to the new thread, or null in case of a failure.  */
__MCF_GTHR_AUX_IMPORT
_MCF_thread*
__MCF_gthr_thread_create_v3(__MCF_gthr_thread_fn* __proc, void* __arg)
  __MCF_noexcept;

/* Creates a thread.
 *
 * @param `thrdp_opt` is an optional pointer that indicates where to store a
 *     pointer to the new thread. If it is non-null, it is updated before the
 *     new thread begins execution. The value is the same as what this function
 *     will return.
 * @param `proc` shall point to the thread procedure.
 * @param `arg` is a user-defined parameter for the thread procedure.
 * @returns a pointer to the new thread, or null in case of a failure.  */
__MCF_GTHR_AUX_IMPORT
_MCF_thread*
__MCF_gthr_thread_create_v4(_MCF_thread** __thrdp_opt, __MCF_gthr_thread_fn* __proc, void* __arg)
  __MCF_noexcept;

/* Waits for a thread to terminate.
 *
 * @param `thrd` points to the thread to wait for. It shall not denote the
 *     calling thread; otherwise the behavior is undefined.
 * @param `resp_opt` is an optional pointer that indicates where to store the
 *     exit code of the thread. If it is non-null, the exit code is stored into
 *     `*resp_opt`. If the thread has exited with an exit unwind, then the exit
 *     code will be `__MCF_GTHR_CANCELLED`.
 * @returns nothing.  */
__MCF_GTHR_AUX_IMPORT
void
__MCF_gthr_thread_join_v3(_MCF_thread* __thrd, void** __resp_opt)
  __MCF_noexcept;

/* Terminates the current thread.
 *
 * This function does not return.
 *
 * @param `result` is the exit code of the current thread.
 * @returns nothing.  */
__MCF_GTHR_AUX_IMPORT __MCF_NEVER_RETURN
void
__MCF_gthr_thread_exit_v3(void* __result)
  __MCF_noexcept;

/* Define inline functions after all declarations.
 *
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_call_once_seh(_MCF_once* __once, __MCF_cxa_dtor_any_ __init_proc, void* __arg)
  __MCF_MAY_THROW
  {
    int __err = _MCF_once_wait(__once, __MCF_nullptr);
    if(__err == 0)
      return;  /* already initialized  */

    __MCF_ASSERT(__err == 1);
    __MCF_gthr_call_once_seh_take_over(__once, __init_proc, __arg);
    _MCF_signal_fence_acq();
  }

__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_rc_mutex_init(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept
  {
    __rmtx->__owner[0] = 0;
    __rmtx->__depth = 0;
    _MCF_mutex_init(__rmtx->__mutex);
  }

__MCF_GTHR_AUX_INLINE
int
__MCF_gthr_rc_mutex_recurse(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept
  {
    /* Check whether the mutex has already been owned.  */
    if(_MCF_atomic_load_32_rlx(__rmtx->__owner) != __MCF_tid())
      return -1;

    /* Increment the recursion count.  */
    __MCF_ASSERT(__rmtx->__depth < 0x7FFFFFFF);
    __rmtx->__depth ++;
    return 0;
  }

__MCF_GTHR_AUX_INLINE
int
__MCF_gthr_rc_mutex_wait(__MCF_gthr_rc_mutex* __rmtx, const int64_t* __timeout_opt)
  __MCF_noexcept
  {
    /* Attempt to take ownership.  */
    int __err = _MCF_mutex_lock(__rmtx->__mutex, __timeout_opt);
    if(__err != 0)
      return __err;

    /* The calling thread owns the mutex now.  */
    __MCF_ASSERT(__rmtx->__owner[0] == 0);
    _MCF_atomic_store_32_rlx(__rmtx->__owner, __MCF_tid());
    __MCF_ASSERT(__rmtx->__depth == 0);
    __rmtx->__depth = 1;
    return 0;
  }

__MCF_GTHR_AUX_INLINE
void
__MCF_gthr_rc_mutex_release(__MCF_gthr_rc_mutex* __rmtx)
  __MCF_noexcept
  {
    /* Reduce a level of recursion.  */
    __MCF_ASSERT(__rmtx->__owner[0] != 0);
    __MCF_ASSERT(__rmtx->__depth > 0);
    __rmtx->__depth --;
    if(__rmtx->__depth != 0)
      return;

    /* Give up ownership now.  */
    _MCF_atomic_store_32_rlx(__rmtx->__owner, 0);
    _MCF_mutex_unlock(__rmtx->__mutex);
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_GTHR_AUX_  */
