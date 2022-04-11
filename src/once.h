// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_ONCE_H_
#define __MCFGTHREAD_ONCE_H_

#include "fwd.h"
#include "atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MCFGTHREAD_ONCE_C_
#  define __MCFGTHREAD_ONCE_INLINE
#else
#  define __MCFGTHREAD_ONCE_INLINE  __MCF_GNU_INLINE
#endif

// Define the once flag struct.
// This takes up the same storage as a pointer.
struct __MCF_once
  {
    uintptr_t __ready : 8;  // this conforms to the Itanium C++ ABI
    uintptr_t __locked : 1;

    uintptr_t __nsleep : __MCF_PTR_SIZE - 9;  // number of sleeping threads
#define __MCF_ONCE_NSLEEP_M  (UINTPTR_MAX >> 9)
  }
  typedef _MCF_once;

// Initializes a once-initialization flag dynamically.
// Static ones should be initialized with `{0}`, like other structs.
//
// Each once-initialization flag can be in any of the three states: UNLOCKED,
// LOCKED and READY. An once-initialization flag is initialized to the UNLOCKED
// state.
void
_MCF_once_init(_MCF_once* __once) __MCF_NOEXCEPT;

__MCFGTHREAD_ONCE_INLINE void
_MCF_once_init(_MCF_once* __once) __MCF_NOEXCEPT
  {
    _MCF_once __temp = { 0 };
    __MCF_ATOMIC_STORE_PTR_REL(__once, &__temp);
  }

// Attempts to lock a once-initialization flag.
// The return value of this function has the same semantics with the
// `__cxa_guard_acquire()` function from the Itanium C++ ABI; see
// <https://itanium-cxx-abi.github.io/cxx-abi/abi.html#once-ctor>
//
// If the `timeout` argument points to a positive integer, it denotes the wait
// expiration time, in number of milliseconds since 1970-01-01T00:00:00Z. If it
// points to a negative integer, the absolute value of it denotes the number of
// milliseconds to wait. If it points to zero, the function returns immediately
// without waiting. If it is null, the function waits indefinitely.
//
// If this once-initialization flag is in the UNLOCKED state, this function
// changes it into the LOCKED state and returns 1. If it is in the LOCKED state
// because another thread has locked it, this function blocks until the other
// thread releases the lock, or returns -1 if the operation times out. If it is
// in the READY state, this function does nothing and returns 0 immediately.
int
_MCF_once_wait(_MCF_once* __once, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

int
_MCF_once_wait_slow(_MCF_once* __once, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

__MCFGTHREAD_ONCE_INLINE int
_MCF_once_wait(_MCF_once* __once, const int64_t* __timeout_opt) __MCF_NOEXCEPT
  {
    _MCF_once __old;
    __MCF_ATOMIC_LOAD_PTR_ACQ(&__old, __once);

    if(__builtin_expect(__old.__ready, 1))
      return 0;

    if(__timeout_opt && (*__timeout_opt == 0) && __builtin_expect(__old.__locked, 0))
      return -1;

    return _MCF_once_wait_slow(__once, __timeout_opt);
  }

// Cancels a once-initialization flag which shall be in the LOCKED state. If
// the flag has not been locked already, the behavior is undefined.
//
// This function changes it into the UNLOCKED state and wakes up the next
// threads that is sleeping on this flag, whose calls to `_MCF_once_wait()`
// will return 1.
//
// Returns the number of threads that have been woken up.
size_t
_MCF_once_abort(_MCF_once* __once) __MCF_NOEXCEPT;

// Releases a once-initialization flag which shall be in the LOCKED state. If
// the flag has not been locked already, the behavior is undefined.
//
// This function changes it into the READY state and wakes up all threads that
// are sleeping on this flag, whose calls to `_MCF_once_wait()` will return 0.
//
// Returns the number of threads that have been woken up.
size_t
_MCF_once_release(_MCF_once* __once) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_ONCE_H_
