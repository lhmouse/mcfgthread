/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_ONCE_
#define __MCFGTHREAD_ONCE_

#include "fwd.h"
#include "atomic.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_ONCE_IMPORT
#  define __MCF_ONCE_IMPORT
#  define __MCF_ONCE_INLINE  __MCF_GNU_INLINE
#endif

/** This is the once-initialization flag, which takes up the same storage as a
 * pointer.  */
struct __MCF_once
  {
    __MCF_EX uintptr_t __ready : 8;  /* this conforms to the Itanium C++ ABI  */
    __MCF_EX uintptr_t __locked : 1;
    __MCF_EX uintptr_t __nsleep : __MCF_PTR_BITS - 9;  /* number of sleeping threads  */
  };

/** Initializes a once-initialization flag dynamically.
 *
 * Static ones should be initialized with `{0}`, like other structs.
 *
 * A once-initialization flag can be in any of the three states: UNLOCKED,
 * LOCKED and READY. An once-initialization flag is initialized to UNLOCKED.
 *
 * @param `once` points to the once-initialization flag to initialize.
 * @returns nothing.
 * @since 1.0  */
__MCF_ONCE_INLINE
void
_MCF_once_init(_MCF_once* __once)
  __MCF_noexcept;

/** Attempts to lock a once-initialization flag.
 *
 * If this once-initialization flag is in the UNLOCKED state, this function
 * changes it into the LOCKED state and returns 1. If it is in the LOCKED state
 * because another thread has locked it, this function blocks until the other
 * thread releases the lock, or returns -1 if the operation has timed out. If
 * the once flag is already in the READY state, this function does nothing and
 * returns 0 immediately.
 *
 * The return value of this function has the same semantics with the
 * `__cxa_guard_acquire()` function from the Itanium C++ ABI.
 *
 * @param `once` points to the once-initialization flag to lock.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *    denotes the expiration time point in the number of milliseconds since
 *    1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *    value of it denotes the number of milliseconds to wait. If it points to
 *    zero, the function returns immediately without waiting. If it is null,
 *    the function waits indefinitely.
 * @returns 1 if the once-initialization flag has been successfully locked and
 *    the caller should perform initialization, 0 if initialization has finished
 *    and the caller should do nothing, or -1 if the operation has timed out.
 * @since 1.0  */
__MCF_ONCE_IMPORT
int
_MCF_once_wait_slow(_MCF_once* __once, const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Attempts to lock a once-initialization flag.
 *
 * If this once-initialization flag is in the UNLOCKED state, this function
 * changes it into the LOCKED state and returns 1. If it is in the LOCKED state
 * because another thread has locked it, this function blocks until the other
 * thread releases the lock, or returns -1 if the operation has timed out. If
 * the once flag is already in the READY state, this function does nothing and
 * returns 0 immediately.
 *
 * The return value of this function has the same semantics with the
 * `__cxa_guard_acquire()` function from the Itanium C++ ABI.
 *
 * This is an inline wrapper for `_MCF_once_wait_slow()`, which does nothing
 * if the once-initialization flag is already in the READY state.
 *
 * @param `once` points to the once-initialization flag to lock.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *    denotes the expiration time point in the number of milliseconds since
 *    1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *    value of it denotes the number of milliseconds to wait. If it points to
 *    zero, the function returns immediately without waiting. If it is null,
 *    the function waits indefinitely.
 * @returns 1 if the once-initialization flag has been successfully locked and
 *    the caller should perform initialization, 0 if initialization has finished
 *    and the caller should do nothing, or -1 if the operation has timed out.
 * @since 1.0  */
__MCF_ONCE_INLINE
int
_MCF_once_wait(_MCF_once* __once, const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Attempts to lock a once-initialization flag with consume semantics.
 *
 * If this once-initialization flag is in the UNLOCKED state, this function
 * changes it into the LOCKED state and returns 1. If it is in the LOCKED state
 * because another thread has locked it, this function blocks until the other
 * thread releases the lock, or returns -1 if the operation has timed out. If
 * the once flag is already in the READY state, this function does nothing and
 * returns 0 immediately.
 *
 * This function avoids an atomic load with acquire semantics in the fast path,
 * but is more tricky and must be used with caution. In order to prevent any
 * mis-optimization, the caller shall only access any protected data through
 * the updated pointer, and never compare the pointer with anything else.
 *
 * The return value of this function has the same semantics with the
 * `__cxa_guard_acquire()` function from the Itanium C++ ABI.
 *
 * This is an inline wrapper for `_MCF_once_wait_slow()`, which does nothing
 * if the once-initialization flag is already in the READY state.
 *
 * @param `once` points to the once-initialization flag to lock.
 * @param `ref_ptr` points to a pointer to the protected data that will be
 *    accessed. Upon success, the function applies a load dependency to the
 *    pointer without changing its value. The pointer shall only be used to
 *    access data; COMPARING THE POINTER WITH ANYTHING IS EXTREMELY DANGEROUS
 *    as it may trick the compiler to lose dependency information and cause
 *    a race condition.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *    denotes the expiration time point in the number of milliseconds since
 *    1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *    value of it denotes the number of milliseconds to wait. If it points to
 *    zero, the function returns immediately without waiting. If it is null,
 *    the function waits indefinitely.
 * @returns 1 if the once-initialization flag has been successfully locked and
 *    the caller should perform initialization, 0 if initialization has finished
 *    and the caller should do nothing, or -1 if the operation has timed out.
 * @since 2.5  */
__MCF_ONCE_INLINE
int
_MCF_once_consume_wait(_MCF_once* __once, void** __ref_ptr, const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Cancels once initialization.
 *
 * This function changes the once-initialization flag to the UNLOCKED state and
 * wakes up the next thread that is waiting for it, which will observe
 * `_MCF_once_wait()` return a value of 1. The caller shall have locked the
 * once-initialization flag, otherwise the behavior is undefined.
 *
 * @param `once` points to the once-initialization flag to cancel.
 * @returns nothing.
 * @since 1.0  */
__MCF_ONCE_IMPORT
void
_MCF_once_abort(_MCF_once* __once)
  __MCF_noexcept;

/** Completes once initialization.
 *
 * This function changes the once-initialization flag to the READY state and
 * wakes up all threads that are waiting for it, which will observe
 * `_MCF_once_wait()` return a value of 0. The caller shall have locked the
 * once-initialization flag, otherwise the behavior is undefined.
 *
 * @param `once` points to the once-initialization flag to complete.
 * @returns nothing.
 * @since 1.0  */
__MCF_ONCE_IMPORT
void
_MCF_once_release(_MCF_once* __once)
  __MCF_noexcept;

/** Define inline functions after all declarations.
 *
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_ONCE_INLINE
void
_MCF_once_init(_MCF_once* __once)
  __MCF_noexcept
  {
    _MCF_once __temp = __MCF_0_INIT;
    _MCF_atomic_store_pptr_rlx(__once, &__temp);
  }

__MCF_ONCE_INLINE
int
_MCF_once_wait(_MCF_once* __once, const int64_t* __timeout_opt)
  __MCF_noexcept
  {
#if __MCF_EXPAND_INLINE_DEFINITIONS
    /* Use raw integers to allow better optimizations.  */
    uintptr_t __old_bits;
    _MCF_atomic_load_pptr_acq(&__old_bits, __once);
    if(__old_bits & 1)
      return 0;
#endif
    return _MCF_once_wait_slow(__once, __timeout_opt);
  }

__MCF_ONCE_INLINE
int
_MCF_once_consume_wait(_MCF_once* __once, void** __ref_ptr, const int64_t* __timeout_opt)
  __MCF_noexcept
  {
#if __MCF_EXPAND_INLINE_DEFINITIONS
    /* Use raw integers to allow better optimizations.  */
    uintptr_t __old_bits;
    _MCF_atomic_load_pptr_rlx(&__old_bits, __once);
    if(__old_bits & 1) {
#  if !defined __MCF_M_X86_ASM
      /* `__ready` is always `1` but this has to incur a load dependency.  */
      *__ref_ptr = (char*) *__ref_ptr + (__old_bits >> 1);
#  endif
      _MCF_signal_fence_acq();
      return 0;
    }
#endif
    (void) __ref_ptr;
    return _MCF_once_wait_slow(__once, __timeout_opt);
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_ONCE_  */
