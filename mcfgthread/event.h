/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_EVENT_
#define __MCFGTHREAD_EVENT_

#include "fwd.h"
#include "atomic.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_EVENT_IMPORT
#  define __MCF_EVENT_IMPORT
#  define __MCF_EVENT_INLINE  __MCF_GNU_INLINE
#endif

/* Define the event struct.
 * This takes up the same storage as a pointer.  */
struct __MCF_event
  {
    __MCF_EX uintptr_t __value : 8;
    __MCF_EX uintptr_t __reserved_bit : 1;

#define __MCF_EVENT_NSLEEP_M   (__MCF_UPTR_MAX >> 9)
    __MCF_EX uintptr_t __nsleep : __MCF_PTR_BITS - 9;  /* number of sleeping threads  */
  };

/* This is the maximum value of an event. This is a value of type `int` due
 * to potential integral promotion.  */
#define __MCF_EVENT_VALUE_MAX   0xFF

/* Define a macro for static initialization of events. The argument is the
 * initial value of the event.  */
#define __MCF_EVENT_INIT(__value_init)  \
    {  \
      (__value_init), 0, 0  \
      + __MCF_STATIC_ASSERT_0((__value_init) >= 0)  \
      + __MCF_STATIC_ASSERT_0((__value_init) <= __MCF_EVENT_VALUE_MAX)  \
    }

/* Initializes an event dynamically. The argument is the initial value of
 * the event, which shall not be negative.
 * Static ones should be initialized with `__MCF_EVENT_INIT(__value_init)`.
 *
 * Returns 0 if the initialization is successful, or -1 in case of invalid
 * arguments.  */
__MCF_EVENT_INLINE
int
_MCF_event_init(_MCF_event* __eventp, int __value_init) __MCF_NOEXCEPT;

/* Gets the current value of an event.  */
__MCF_EVENT_INLINE
uint8_t
_MCF_event_get(const _MCF_event* __eventp) __MCF_NOEXCEPT;

/* Waits for an event until it does NOT contain an undesired value.
 *
 * If the `__timeout_opt` argument points to a positive integer, it denotes the
 * expiration time in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to wait. If it points to zero, the function returns immediately
 * without waiting. If it is null, the function waits indefinitely.
 *
 * If this event contains the undesired value, this function blocks until
 * some other value has been stored. Otherwise, this function returns its value
 * immediately.
 *
 * Returns a desired value which never equals the lowest byte of `__undesired`,
 * or -1 if the operation has timed out, or -2 in case of invalid arguments.  */
__MCF_EVENT_IMPORT
int
_MCF_event_await_change_slow(_MCF_event* __eventp, int __undesired, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

__MCF_EVENT_INLINE
int
_MCF_event_await_change(_MCF_event* __eventp, int __undesired, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

/* Sets the value of an event. If the value has been changed, other threads are
 * woken up so they can check it.
 *
 * Returns 0 if the value has been updated successfully, or -1 in case of
 * invalid arguments.  */
__MCF_EVENT_IMPORT
int
_MCF_event_set_slow(_MCF_event* __eventp, int __value) __MCF_NOEXCEPT;

__MCF_EVENT_INLINE
int
_MCF_event_set(_MCF_event* __eventp, int __value) __MCF_NOEXCEPT;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_EVENT_INLINE
int
_MCF_event_init(_MCF_event* __eventp, int __value_init) __MCF_NOEXCEPT
  {
    _MCF_event __temp = { 0, 0, 0 };

    if((__value_init < 0) || (__value_init > __MCF_EVENT_VALUE_MAX))
      return -1;

    __temp.__value = (uint8_t) __value_init;
    _MCF_atomic_store_pptr_rel(__eventp, &__temp);
    return 0;
  }

__MCF_EVENT_INLINE
uint8_t
_MCF_event_get(const _MCF_event* __eventp) __MCF_NOEXCEPT
  {
    _MCF_event __temp;
    _MCF_atomic_load_pptr_rlx(&__temp, __eventp);
    return __temp.__value;
  }

__MCF_EVENT_INLINE
int
_MCF_event_await_change(_MCF_event* __eventp, int __undesired, const int64_t* __timeout_opt) __MCF_NOEXCEPT
  {
#if defined __OPTIMIZE__ && !defined __OPTIMIZE_SIZE__
    _MCF_event __old;

    if((__undesired < 0) || (__undesired > __MCF_EVENT_VALUE_MAX))
      return -2;

    /* Check whether the event does not contain the undesired value. If so,
     * don't block at all.  */
    _MCF_atomic_load_pptr_acq(&__old, __eventp);
    if(__old.__value != __undesired)
      return __old.__value;

    /* If a timeout of zero is specified, don't block at all.  */
    if(__timeout_opt && (*__timeout_opt == 0))
      return -1;
#endif  /* speed */

    return _MCF_event_await_change_slow(__eventp, __undesired, __timeout_opt);
  }

__MCF_EVENT_INLINE
int
_MCF_event_set(_MCF_event* __eventp, int __value) __MCF_NOEXCEPT
  {
#if defined __OPTIMIZE__ && !defined __OPTIMIZE_SIZE__
    _MCF_event __old;

    if((__value < 0) || (__value > __MCF_EVENT_VALUE_MAX))
      return -1;

    /* Check whether the event already contains the value. If so, don't do
     * anything, in order to prevent thundering herds.  */
    _MCF_atomic_load_pptr_acq(&__old, __eventp);
    if(__old.__value == __value)
      return 0;
#endif  /* speed */

    return _MCF_event_set_slow(__eventp, __value);
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_EVENT_  */
