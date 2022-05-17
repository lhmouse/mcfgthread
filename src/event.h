/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_EVENT_
#define __MCFGTHREAD_EVENT_

#include "fwd.h"
#include "atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCF_DECLSPEC_EVENT
#  define __MCF_DECLSPEC_EVENT(...)  __VA_ARGS__
#endif

/* Define the event struct.
 * This takes up the same storage as a pointer.  */
struct __MCF_event
  {
    uintptr_t __value : 8;

    uintptr_t __nsleep : __MCF_PTR_BITS - 8;  /* number of sleeping threads  */
#define __MCF_EVENT_NSLEEP_M   (__UINTPTR_MAX__ >> 8)
  };

/* This is the maximum value of an event.  */
#define __MCF_EVENT_VALUE_MAX   __UINT8_MAX__

/* Define a macro for static initialization of events. The argument is the
 * initial value of the event.  */
#define __MCF_EVENT_INIT(__value_init)  \
    { __MCF_STATIC_ASSERT((__value_init) >= 0) +  \
      __MCF_STATIC_ASSERT((__value_init) <= __MCF_EVENT_VALUE_MAX) +  \
      (__value_init), 0  }

/* Initializes an event dynamically. The argument is the initial value of
 * the event, which shall not be negative.
 * Static ones should be initialized with `__MCF_EVENT_INIT(__value_init)`.
 *
 * Returns 0 if the initialization is successful, or -1 in case of invalid
 * arguments.  */
__MCF_DECLSPEC_EVENT(__MCF_GNU_INLINE)
int
_MCF_event_init(_MCF_event* __event, int __value_init) __MCF_NOEXCEPT;

__MCF_DECLSPEC_EVENT(__MCF_GNU_INLINE)
int
_MCF_event_init(_MCF_event* __event, int __value_init) __MCF_NOEXCEPT
  {
    if((__value_init < 0) || (__value_init > __MCF_EVENT_VALUE_MAX))
      return -1;

    _MCF_event __temp = { (uint8_t) __value_init, 0 };
    _MCF_atomic_store_pptr_rel(__event, &__temp);
    return 0;
  }

/* Gets the current value of an event.  */
__MCF_DECLSPEC_EVENT(__MCF_GNU_INLINE)
uint8_t
_MCF_event_get(const _MCF_event* __event) __MCF_NOEXCEPT;

__MCF_DECLSPEC_EVENT(__MCF_GNU_INLINE)
uint8_t
_MCF_event_get(const _MCF_event* __event) __MCF_NOEXCEPT
  {
    _MCF_event __temp;
    _MCF_atomic_load_pptr_rlx(&__temp, __event);
    return __temp.__value;
  }

/* Wait for an event until it does NOT contain an undesired value.
 *
 * If the `__timeout_opt` argument points to a positive integer, it denotes the
 * expiration time in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to wait. If it points to zero, the function returns immediately
 * without waiting. If it is null, the function waits indefinitely.
 *
 * If this event contains the undesired value, this function blocks until
 * some other value has been stored. Otherwise, this function returns 0
 * immediately.
 *
 * Returns 0 if the value does not equal the lowest byte of `__undesired`, or
 * -1 if the operation has timed out, or -2 in case of invalid arguments.  */
__MCF_DECLSPEC_EVENT()
int
_MCF_event_await_change_slow(_MCF_event* __event, int __undesired, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_EVENT(__MCF_GNU_INLINE)
int
_MCF_event_await_change(_MCF_event* __event, int __undesired, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

__MCF_DECLSPEC_EVENT(__MCF_GNU_INLINE)
int
_MCF_event_await_change(_MCF_event* __event, int __undesired, const int64_t* __timeout_opt) __MCF_NOEXCEPT
  {
    if((__undesired < 0) || (__undesired > __MCF_EVENT_VALUE_MAX))
      return -2;

    _MCF_event __old;
    _MCF_atomic_load_pptr_acq(&__old, __event);

    /* Check whether the event does not contain the undesired value. If so,
     * don't block at all.  */
    if(__old.__value != (uint8_t) __undesired)
      return 0;

    if(__timeout_opt && (*__timeout_opt == 0))
      return -1;

    return _MCF_event_await_change_slow(__event, __undesired, __timeout_opt);
  }

/* Sets the value of an event. If the value has been changed, other threads are
 * woken up so they can check it.
 *
 * Returns 0 if the value has been updated successfully, or -1 in case of
 * invalid arguments.  */
__MCF_DECLSPEC_EVENT()
int
_MCF_event_set_slow(_MCF_event* __event, int __value) __MCF_NOEXCEPT;

__MCF_DECLSPEC_EVENT(__MCF_GNU_INLINE)
int
_MCF_event_set(_MCF_event* __event, int __value) __MCF_NOEXCEPT;

__MCF_DECLSPEC_EVENT(__MCF_GNU_INLINE)
int
_MCF_event_set(_MCF_event* __event, int __value) __MCF_NOEXCEPT
  {
    if((__value < 0) || (__value > __MCF_EVENT_VALUE_MAX))
      return -1;

    _MCF_event __old;
    _MCF_atomic_load_pptr_acq(&__old, __event);

    /* Check whether the event already contains the value. If so, don't do
     * anything, in order to prevent thundering herds.  */
    if(__old.__value == (uint8_t) __value)
      return 0;

    return _MCF_event_set_slow(__event, __value);
  }

#ifdef __cplusplus
}
#endif

#endif  /* __MCFGTHREAD_EVENT_  */
