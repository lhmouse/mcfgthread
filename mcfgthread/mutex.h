/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_MUTEX_
#define __MCFGTHREAD_MUTEX_

#include "fwd.h"
#include "atomic.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_MUTEX_IMPORT
#  define __MCF_MUTEX_IMPORT
#  define __MCF_MUTEX_INLINE  __MCF_GNU_INLINE
#endif

/* This is the mutex, which takes up the same storage as a pointer.  */
struct __MCF_mutex
  {
    __MCF_EX uintptr_t __locked : 1;
    __MCF_EX uintptr_t __sp_mask : 4;  /* mask of spinning threads; 1b/thread  */
    __MCF_EX uintptr_t __sp_nfail : 4;  /* number of timeouts after spinning  */
    __MCF_EX uintptr_t __nsleep : __MCF_PTR_BITS - 9;  /* number of sleeping threads  */
  };

/* If the spinning failure counter of a mutex has reached this number, newcomers
 * will not attempt to spin at all. This value must not be greater than the
 * maximum value of `__sp_nfail`, and must not be zero.  */
#define __MCF_MUTEX_SP_NFAIL_THRESHOLD   10U

/* Initializes a mutex dynamically.
 *
 * Static ones should be initialized with `{0}`, like other structs.
 *
 * @param `mtx` points to the mutex to initialize.
 * @returns nothing.  */
__MCF_MUTEX_INLINE
void
_MCF_mutex_init(_MCF_mutex* __mtx)
  __MCF_noexcept;

/* Attempts to lock a mutex.
 *
 * If the mutex is not locked, this function locks it and returns immediately;
 * otherwise, it waits until the mutex becomes unlocked, locks it, and returns.
 * This is a simple mutex that is not recursive and performs no error checking.
 * If the caller attempts to lock a mutex which it has already held, deadlocks
 * may occur.
 *
 * @param `mtx` points to the mutex to lock.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns 0 if the mutex has been successfully locked, or -1 if the operation
 *     has timed out.  */
__MCF_MUTEX_IMPORT
int
_MCF_mutex_lock_slow(_MCF_mutex* __mtx, const int64_t* __timeout_opt)
  __MCF_noexcept;

/* Attempts to lock a mutex.
 *
 * If the mutex is not locked, this function locks it and returns immediately;
 * otherwise, it waits until the mutex becomes unlocked, locks it, and returns.
 * This is a simple mutex that is not recursive and performs no error checking.
 * If the caller attempts to lock a mutex which it has already held, deadlocks
 * may occur.
 *
 * This is an inline wrapper for `_MCF_mutex_lock_slow()`, which is optimized
 * when there's little to no contention, for example, when there's only one
 * thread.
 *
 * @param `mtx` points to the mutex to lock.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns 0 if the mutex has been successfully locked, or -1 if the operation
 *     has timed out.  */
__MCF_MUTEX_INLINE
int
_MCF_mutex_lock(_MCF_mutex* __mtx, const int64_t* __timeout_opt)
  __MCF_noexcept;

/* Unlocks a mutex.
 *
 * If the mutex has not been locked, the behavior is undefined. This function
 * may be called by a different thread from which locked the same mutex.
 *
 * @param `mtx` points to the mutex to unlock.
 * @returns nothing.  */
__MCF_MUTEX_IMPORT
void
_MCF_mutex_unlock_slow(_MCF_mutex* __mtx)
  __MCF_noexcept;

/* Unlocks a mutex.
 *
 * If the mutex has not been locked, the behavior is undefined. This function
 * may be called by a different thread from which locked the same mutex.
 *
 * This is an inline wrapper for `_MCF_mutex_unlock_slow()`, which is optimized
 * when there's little to no contention, for example, when there's only one
 * thread.
 *
 * @param `mtx` points to the mutex to unlock.
 * @returns nothing.  */
__MCF_MUTEX_INLINE
void
_MCF_mutex_unlock(_MCF_mutex* __mtx)
  __MCF_noexcept;

/* Define inline functions after all declarations.
 *
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_MUTEX_INLINE
void
_MCF_mutex_init(_MCF_mutex* __mtx)
  __MCF_noexcept
  {
    _MCF_mutex __temp = __MCF_0_INIT;
    _MCF_atomic_store_pptr_rlx(__mtx, &__temp);
  }

__MCF_MUTEX_INLINE
int
_MCF_mutex_lock(_MCF_mutex* __mtx, const int64_t* __timeout_opt)
  __MCF_noexcept
  {
#if __MCF_EXPAND_INLINE_DEFINITIONS
    /* Use raw integers to work around issues about missed optimizations.
     * See <https://github.com/lhmouse/mcfgthread/issues/320>.  */
    uintptr_t __old_bits, __new_bits;
    _MCF_atomic_load_pptr_rlx(&__old_bits, __mtx);
    if((__old_bits & 1) == 0) {  /* __locked == 0 */
      __new_bits = __old_bits + 1U - (((((uint32_t) __old_bits >> 4) & 0x1EU) + 0x1EU) & 0x20U);
      if(_MCF_atomic_cmpxchg_weak_pptr_acq(__mtx, &__old_bits, &__new_bits))
        return 0;
    }
#endif
    return _MCF_mutex_lock_slow(__mtx, __timeout_opt);
  }

__MCF_MUTEX_INLINE
void
_MCF_mutex_unlock(_MCF_mutex* __mtx)
  __MCF_noexcept
  {
#if __MCF_EXPAND_INLINE_DEFINITIONS
    /* Use raw integers to work around issues about missed optimizations.
     * See <https://github.com/lhmouse/mcfgthread/issues/320>.  */
    uintptr_t __old_bits, __new_bits;
    _MCF_atomic_load_pptr_rlx(&__old_bits, __mtx);
    __MCF_ASSERT((__old_bits & 1) != 0);  /* __locked != 0 */
    if((__old_bits & (__MCF_UINTPTR_MAX - 0x1E1U)) == 0) {  /* __sp_mask = __nsleep = 0 */
      __new_bits = __old_bits - 1U;
      if(_MCF_atomic_cmpxchg_weak_pptr_rel(__mtx, &__old_bits, &__new_bits))
        return;
    }
#endif
    _MCF_mutex_unlock_slow(__mtx);
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_MUTEX_  */
