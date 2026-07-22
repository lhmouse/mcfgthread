/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_COND_
#define __MCFGTHREAD_COND_

#include "fwd.h"
#include "atomic.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_COND_IMPORT
#  define __MCF_COND_IMPORT
#  define __MCF_COND_INLINE  __MCF_GNU_INLINE
#endif

/** This is the condition variable, which takes up the same storage as a pointer.  */
struct __MCF_cond
  {
    __MCF_EX uintptr_t __reserved_bits : 9;
    __MCF_EX uintptr_t __nsleep : __MCF_PTR_BITS - 9;  /* number of sleeping threads  */
  };

/** Initializes a condition variable dynamically.
 *
 * Static ones should be initialized with `{0}`, like other structs.
 *
 * @param `cnd` points to the condition variable to initialize.
 * @returns nothing.
 * @since 1.0  */
__MCF_COND_INLINE
void
_MCF_cond_init(_MCF_cond* __cnd)
  __MCF_noexcept;

/** Suspends the current thread on a condition variable.
 *
 * The calling thread will invoke the unlock callback before going to sleep, and
 * will invoke the relock callback after it awakes. When the condition variable
 * is accompanied by a mutex, these callbacks should unlock and relock the
 * associated mutex, if any. The return value of the unlock callback will be
 * passed to the relock callback verbatim. Neither callback is allowed to throw
 * an exception.
 *
 * An unlock callback may be provided regardless of a relock callback, but if a
 * relock callback is provided without an unlock callback, it will not be
 * invoked at all.
 *
 * @param `cnd` points to the condition variable to wait on.
 * @param `unlock_opt` is an optional pointer to the unlock callback.
 * @param `relock_opt` is an optional pointer to the relock callback.
 * @param `lock_arg` is a user-defined argument to both callbacks.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *    denotes the expiration time point in the number of milliseconds since
 *    1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *    value of it denotes the number of milliseconds to wait. If it points to
 *    zero, the function returns immediately without waiting. If it is null,
 *    the function waits indefinitely.
 * @returns 0 if the condition variable was signaled or there was a spurious
 *    wakeup, or -1 if the wait operation has timed out.
 * @since 1.0  */
__MCF_COND_IMPORT
int
_MCF_cond_wait(_MCF_cond* __cnd, _MCF_cond_unlock_callback* __unlock_opt,
               _MCF_cond_relock_callback* __relock_opt, intptr_t __lock_arg,
               const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Wakes up some threads that are sleeping on this condition variable.
 *
 * @param `cnd` points to the condition variable to signal.
 * @param `limit` is the maximum number of threads to wake up.
 * @returns the number of threads that have been woken up.
 * @since 1.0  */
__MCF_COND_IMPORT
size_t
_MCF_cond_signal_some_slow(_MCF_cond* __cnd, size_t __limit)
  __MCF_noexcept;

/** Wakes up some threads that are sleeping on this condition variable.
 *
 * This is an inline wrapper for `_MCF_cond_signal_some_slow()`, which does
 * nothing when no thread is sleeping.
 *
 * @param `cnd` points to the condition variable to signal.
 * @param `limit` is the maximum number of threads to wake up.
 * @returns the number of threads that have been woken up.
 * @since 1.0  */
__MCF_COND_INLINE
size_t
_MCF_cond_signal_some(_MCF_cond* __cnd, size_t __limit)
  __MCF_noexcept;

/** Wakes up one thread that is sleeping on this condition variable.
 *
 * This function is equivalent to `_MCF_cond_signal_some(cnd, 1)`.
 *
 * @param `cnd` points to the condition variable to signal.
 * @returns the number of threads that have been woken up.
 * @since 1.0  */
__MCF_COND_INLINE
size_t
_MCF_cond_signal(_MCF_cond* __cnd)
  __MCF_noexcept;

/** Wakes up all threads that are sleeping on this condition variable.
 *
 * This function is equivalent to `_MCF_cond_signal_some(cnd, SIZE_MAX)`.
 *
 * @param `cnd` points to the condition variable to signal.
 * @returns the number of threads that have been woken up.
 * @since 1.0  */
__MCF_COND_INLINE
size_t
_MCF_cond_signal_all(_MCF_cond* __cnd)
  __MCF_noexcept;

/** Define inline functions after all declarations.
 *
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_COND_INLINE
void
_MCF_cond_init(_MCF_cond* __cnd)
  __MCF_noexcept
  {
    _MCF_cond __temp = __MCF_0_INIT;
    _MCF_atomic_store_pptr_rlx(__cnd, &__temp);
  }

__MCF_COND_INLINE
size_t
_MCF_cond_signal_some(_MCF_cond* __cnd, size_t __limit)
  __MCF_noexcept
  {
#if __MCF_EXPAND_INLINE_DEFINITIONS
    _MCF_cond __old_v;
    _MCF_atomic_load_pptr_acq(&__old_v, __cnd);
    if(__old_v.__nsleep == 0)
      return 0;
#endif
    return _MCF_cond_signal_some_slow(__cnd, __limit);
  }

__MCF_COND_INLINE
size_t
_MCF_cond_signal(_MCF_cond* __cnd)
  __MCF_noexcept
  {
    return _MCF_cond_signal_some(__cnd, 1);
  }

__MCF_COND_INLINE
size_t
_MCF_cond_signal_all(_MCF_cond* __cnd)
  __MCF_noexcept
  {
    return _MCF_cond_signal_some(__cnd, SIZE_MAX);
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_COND_  */
