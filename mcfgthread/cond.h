/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_COND_
#define __MCFGTHREAD_COND_

#include "fwd.h"
#include "atomic.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_COND_IMPORT
#  define __MCF_COND_IMPORT
#  define __MCF_COND_INLINE  __MCF_GNU_INLINE
#endif

/* Define the condition variable struct.
 * This takes up the same storage as a pointer.  */
struct __MCF_cond
  {
    uintptr_t __reserved : 9;

#define __MCF_COND_NSLEEP_M   (__UINTPTR_MAX__ >> 9)
    uintptr_t __nsleep : __MCF_PTR_BITS - 9;  /* number of sleeping threads  */
  };

/* Initializes a condition variable dynamically.
 * Static ones should be initialized with `{0}`, like other structs.  */
__MCF_COND_INLINE
void
_MCF_cond_init(_MCF_cond* __cond) __MCF_NOEXCEPT;

/* Puts the current thread to sleep on a condition variable.
 *
 * The calling thread invokes the unlock callback before it goes to sleep on
 * this condition variable, and invokes the relock callback after it awakes. It
 * should unlock and relock the associated mutex, if any. The return value of
 * the unlock callback is passed to the relock callback verbatim. An unlock
 * callback may be provided regardless of a relock callback, but if a relock
 * callback is provided without an unlock callback, it is not invoked at all.
 * Neither callback is allowed to throw exceptions.
 *
 * If the `__timeout_opt` argument points to a positive integer, it denotes the
 * expiration time in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to wait. If it points to zero, the function returns immediately
 * without waiting. If it is null, the function waits indefinitely.
 *
 * Returns 0 if the condition variable has been signaled or there is a spurious
 * wakeup, or -1 if the wait operation has timed out.  */
__MCF_COND_IMPORT
int
_MCF_cond_wait(_MCF_cond* __cond, _MCF_cond_unlock_callback* __unlock_opt, _MCF_cond_relock_callback* __relock_opt, intptr_t __lock_arg, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

/* Wakes up some or all threads that have been put to sleep on this condition
 * variable.
 *
 * Returns the number of threads that have been woken up.  */
__MCF_COND_IMPORT
size_t
_MCF_cond_signal_some_slow(_MCF_cond* __cond, size_t __max) __MCF_NOEXCEPT;

__MCF_COND_INLINE
size_t
_MCF_cond_signal_some(_MCF_cond* __cond, size_t __max) __MCF_NOEXCEPT;

__MCF_COND_INLINE
size_t
_MCF_cond_signal(_MCF_cond* __cond) __MCF_NOEXCEPT;

__MCF_COND_INLINE
size_t
_MCF_cond_signal_all(_MCF_cond* __cond) __MCF_NOEXCEPT;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_COND_INLINE
void
_MCF_cond_init(_MCF_cond* __cond) __MCF_NOEXCEPT
  {
    _MCF_cond __temp = __MCF_0_INIT;
    _MCF_atomic_store_pptr_rel(__cond, &__temp);
  }

__MCF_COND_INLINE
size_t
_MCF_cond_signal_some(_MCF_cond* __cond, size_t __max) __MCF_NOEXCEPT
  {
    _MCF_cond __old;
    _MCF_atomic_load_pptr_acq(&__old, __cond);

    if(__old.__nsleep == 0)
      return 0;

    return _MCF_cond_signal_some_slow(__cond, __max);
  }

__MCF_COND_INLINE
size_t
_MCF_cond_signal(_MCF_cond* __cond) __MCF_NOEXCEPT
  {
    return _MCF_cond_signal_some(__cond, 1);
  }

__MCF_COND_INLINE
size_t
_MCF_cond_signal_all(_MCF_cond* __cond) __MCF_NOEXCEPT
  {
    return _MCF_cond_signal_some(__cond, SIZE_MAX);
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_COND_  */
