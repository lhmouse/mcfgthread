/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_EVENT_
#define __MCFGTHREAD_EVENT_

#include "fwd.h"
#include "atomic.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_EVENT_IMPORT
#  define __MCF_EVENT_IMPORT
#  define __MCF_EVENT_INLINE  __MCF_GNU_INLINE
#endif

/** This is the event object, which takes up the same storage as a pointer. An
 * event object is like a futex on Linux, except that its value is only a byte
 * rather than a 32-bit integer.  */
struct __MCF_event
  {
    __MCF_EX uintptr_t __value : 8;
    __MCF_EX uintptr_t __reserved_bit : 1;
    __MCF_EX uintptr_t __nsleep : __MCF_PTR_BITS - 9;  /* number of sleeping threads  */
  };

/** This is the maximum value of an event. This is a value of type `int` due
 * to potential integral promotion.  */
#define __MCF_EVENT_VALUE_MAX   0xFF

/** This is a macro for static initialization of an event. The argument is the
 * initial value of the event.  */
#define __MCF_EVENT_INIT(__value_init)  \
    { (__value_init), 0, 0  \
      + __MCF_STATIC_ASSERT_0((__value_init) >= 0)  \
      + __MCF_STATIC_ASSERT_0((__value_init) <= __MCF_EVENT_VALUE_MAX)  }

/** Initializes an event dynamically.
 *
 * Static ones should be initialized with `__MCF_EVENT_INIT(value_init)`.
 *
 * @param `eventp` points to the event to initialize.
 * @param `value_init` is the initial value of the event, which shall not be
 *     negative and shall not exceed `__MCF_EVENT_VALUE_MAX`.
 * @returns 0 if the initialization is successful, or -1 if `value_init` is out
 *     of range. The function will not fail if all arguments are valid.  */
__MCF_EVENT_INLINE
int
_MCF_event_init(_MCF_event* __eventp, int __value_init)
  __MCF_noexcept;

/** Gets the current value of an event.
 *
 * @param `eventp` points to the event to get.
 * @returns the current value of the event as a byte.  */
__MCF_EVENT_INLINE
uint8_t
_MCF_event_get(const _MCF_event* __eventp)
  __MCF_noexcept;

/** Waits for an event until it does NOT contain an undesired value.
 *
 * If this event contains the undesired value, this function blocks until some
 * other value has been stored. Otherwise, this function returns immediately.
 *
 * @param `eventp` points to the event to wait for.
 * @param `undesired` is the undesired value as an integer. Only the lowest byte
 *     is compared.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns a non-negative integer if the event has been changed to a value
 *     other than `undesired`, or -1 if the wait operation has timed out.  */
__MCF_EVENT_IMPORT
int
_MCF_event_await_change_slow(_MCF_event* __eventp, int __undesired, const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Waits for an event until it does NOT contain an undesired value.
 *
 * If this event contains the undesired value, this function blocks until some
 * other value has been stored. Otherwise, this function returns immediately.
 *
 * This is an inline wrapper for `_MCF_event_await_change_slow()`, which does
 * nothing if the value is already not undesired.
 *
 * @param `eventp` points to the event to wait for.
 * @param `undesired` is the undesired value as an integer. Only the lowest byte
 *     is compared.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns a non-negative integer if the event has been changed to a value
 *     other than `undesired`, or -1 if the wait operation has timed out.  */
__MCF_EVENT_INLINE
int
_MCF_event_await_change(_MCF_event* __eventp, int __undesired, const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Sets the value of an event and wakes up all threads.
 *
 * @param `eventp` points to the event to set.
 * @param `value` is the new value to set, which shall not be negative and shall
 *     not exceed `__MCF_EVENT_VALUE_MAX`.
 * @returns 0 if the value has been updated successfully, or -1 if `value` is out
 *     of range. The function will not fail if all arguments are valid.  */
__MCF_EVENT_IMPORT
int
_MCF_event_set_slow(_MCF_event* __eventp, int __value)
  __MCF_noexcept;

/** Sets the value of an event and wakes up all threads.
 *
 * This function is the same as `_MCF_event_set_slow()`.
 *
 * @param `eventp` points to the event to set.
 * @param `value` is the new value to set, which shall not be negative and shall
 *     not exceed `__MCF_EVENT_VALUE_MAX`.
 * @returns 0 if the value has been updated successfully, or -1 if `value` is out
 *     of range. The function will not fail if all arguments are valid.  */
__MCF_EVENT_INLINE
int
_MCF_event_set(_MCF_event* __eventp, int __value)
  __MCF_noexcept;

/** Define inline functions after all declarations.
 *
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_EVENT_INLINE
int
_MCF_event_init(_MCF_event* __eventp, int __value_init)
  __MCF_noexcept
  {
    if((__value_init < 0) || (__value_init > __MCF_EVENT_VALUE_MAX))
      return -1;
    else {
      _MCF_event __temp = __MCF_0_INIT;
      __temp.__value = (uint8_t) __value_init;
      _MCF_atomic_store_pptr_rlx(__eventp, &__temp);
      return 0;
    }
  }

__MCF_EVENT_INLINE
uint8_t
_MCF_event_get(const _MCF_event* __eventp)
  __MCF_noexcept
  {
    _MCF_event __temp;
    _MCF_atomic_load_pptr_rlx(&__temp, __eventp);
    return __temp.__value;
  }

__MCF_EVENT_INLINE
int
_MCF_event_await_change(_MCF_event* __eventp, int __undesired, const int64_t* __timeout_opt)
  __MCF_noexcept
  {
#if __MCF_EXPAND_INLINE_DEFINITIONS
    _MCF_event __old_v;
    _MCF_atomic_load_pptr_acq(&__old_v, __eventp);
    if(__old_v.__value != (uint8_t) __undesired)
      return __old_v.__value;
#endif
    return _MCF_event_await_change_slow(__eventp, __undesired, __timeout_opt);
  }

__MCF_EVENT_INLINE
int
_MCF_event_set(_MCF_event* __eventp, int __value)
  __MCF_noexcept
  {
    return _MCF_event_set_slow(__eventp, __value);
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_EVENT_  */
