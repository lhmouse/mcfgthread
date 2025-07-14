/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_THREAD_
#define __MCFGTHREAD_THREAD_

#include "fwd.h"
#include "dtor_queue.h"
#include "tls.h"
#include "atomic.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_THREAD_IMPORT
#  define __MCF_THREAD_IMPORT
#  define __MCF_THREAD_INLINE  __MCF_GNU_INLINE
#endif

/* Define the thread control struct.  */
struct __MCF_thread
  {
    int32_t __nref[1];  /* atomic reference count  */
    uint32_t __tid;  /* thread id  */
    __MCF_HANDLE __handle;  /* win32 thread handle  */

    _MCF_thread_procedure* __proc;  /* user-defined thread procedure  */
    void* __data_opt;  /* pointer to user-defined data  */

    __MCF_dtor_queue __atexit_queue[1];  /* for `__cxa_thread_atexit()`  */
    __MCF_tls_table __tls_table[1];  /* for `_MCF_tls_get()` and `_MCF_tls_set()`  */

    void* __libobjc_tls_data;  /* for GCC libobjc  */

    /* `__data_opt` shall always point to `__data_storage` below. The space
     * preceding it is reserved for future use. It is not safe to assume the
     * offset of `__data_storage` to be a constant.  */
#define __MCF_THREAD_DATA_ALIGNMENT   16U
    __MCF_EX __MCF_ALIGNED(__MCF_THREAD_DATA_ALIGNMENT) char __data_storage[0];
  };

/* This is the maximum alignment of user-defined data that is supported by
 * `_MCF_thread_new_aligned()`. It is defined here for illustration purposes
 * only and you should not make assumption about it.  */
#define __MCF_THREAD_MAX_DATA_ALIGNMENT   0x200000U

/* Define thread priority constants, from lowest to highest.
 * These values match Windows APIs and can be passed around as such, but we
 * define only what we find necessary at the moment.  */
enum __MCF_thread_priority __MCF_CXX11(: int)
  {
    _MCF_thread_priority_idle           = -15,
    _MCF_thread_priority_low            =  -2,
    _MCF_thread_priority_below_normal   =  -1,
    _MCF_thread_priority_normal         =   0,
    _MCF_thread_priority_above_normal   =  +1,
    _MCF_thread_priority_high           =  +2,
    _MCF_thread_priority_realtime       = +15,
    __MCF_thread_priority_end
  };

/* Creates a thread. The `__nref` member is initialized to 2, because a running
 * thread holds a reference to itself.
 *
 * If `__size` is specified as non-zero, storage for user-defined data is
 * reserved. The storage is initialized to zeroes. If `__data_opt` is non-null,
 * it is used to initialize the storage before the new thread begins execution.
 * The `__align` parameter specifies the alignment of user-defined data. If it
 * is not zero, then it must be a power of two; otherwise this function fails
 * with `ERROR_NOT_SUPPORTED`.
 *
 * Returns a new thread control structure. The caller is required to call
 * `_MCF_thread_drop_ref()` when it is no longer needed. If the thread cannot
 * be created, a null pointer is returned and an error code can be obtained
 * via `_MCF_get_win32_error()`.  */
__MCF_THREAD_IMPORT
_MCF_thread*
_MCF_thread_new_aligned(_MCF_thread_procedure* __proc, size_t __align, const void* __data_opt, size_t __size)
  __MCF_noexcept;

__MCF_THREAD_INLINE
_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* __proc, const void* __data_opt, size_t __size)
  __MCF_noexcept;

/* Attaches a thread that was not created by `_MCF_thread_new_aligned()`.
 *
 * This function takes ownership of the thread control structure and assigns it
 * to the calling thread, which shall not have already had a control structure.
 * The caller must initialize the struct properly. The fields `__nref`, `__tid`
 * and `__handle` must be zeroes.
 *
 * This function will not fail. In case of failure to allocate resources, the
 * current process is terminated. Be advised, this function can be called
 * implicitly by the compiler or its runtime library. If proper error handling
 * is desired, it's recommended to ensure that all threads be created with
 * `_MCF_thread_new_aligned()`.  */
__MCF_THREAD_IMPORT
_MCF_thread*
__MCF_thread_attach_foreign(_MCF_thread* __thrd)
  __MCF_noexcept;

/* Gets a pointer to user-defined data of a thread. If the thread does not have
 * user-defined data, because zero was specified for the `__size` parameter to
 * `_MCF_thread_new()`, a null pointer is returned.  */
__MCF_THREAD_INLINE __MCF_FN_PURE
__MCF_CXX(const) void*
_MCF_thread_get_data(const _MCF_thread* __thrd)
  __MCF_noexcept;

/* Gets the number of references of a thread struct.  */
__MCF_THREAD_INLINE __MCF_FN_PURE
int32_t
_MCF_thread_get_ref(const _MCF_thread* __thrd)
  __MCF_noexcept;

/* Adds a reference count of a thread structure. This may be useful if you
 * wish to pass a pointer to other code.  */
__MCF_THREAD_INLINE
void
_MCF_thread_add_ref(_MCF_thread* __thrd)
  __MCF_noexcept;

/* Drops a reference count of a thread structure. An active thread owns a
 * reference count of itself and `_MCF_thread_new()` returns another one. When
 * the reference count is reduced to zero, the structure is deallocated.  */
__MCF_THREAD_IMPORT
void
_MCF_thread_drop_ref_nonnull(_MCF_thread* __thrd)
  __MCF_noexcept;

__MCF_THREAD_INLINE
void
_MCF_thread_drop_ref(_MCF_thread* __thrd_opt)
  __MCF_noexcept;

/* Gets the ID of a thread.  */
__MCF_THREAD_INLINE __MCF_FN_PURE __MCF_CXX11(constexpr)
uint32_t
_MCF_thread_get_tid(const _MCF_thread* __thrd)
  __MCF_noexcept;

/* Gets the handle of a thread.  */
__MCF_THREAD_INLINE __MCF_FN_PURE __MCF_CXX11(constexpr)
__MCF_HANDLE
_MCF_thread_get_handle(const _MCF_thread* __thrd)
  __MCF_noexcept;

/* Exits from a thread.  */
__MCF_THREAD_IMPORT __MCF_NEVER_RETURN
void
_MCF_thread_exit(void)
  __MCF_noexcept;

/* Waits for a thread to finish execution.
 *
 * If the `__timeout_opt` argument points to a positive integer, it denotes the
 * expiration time in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to wait. If it points to zero, the function returns immediately
 * without waiting. If it is null, the function waits indefinitely.
 *
 * Returns 0 if the thread has terminated, or -1 if the wait operation has
 * timed out.  */
__MCF_THREAD_IMPORT
int
_MCF_thread_wait(const _MCF_thread* __thrd_opt, const int64_t* __timeout_opt)
  __MCF_noexcept;

/* Gets the priority of a thread.
 *
 * If the `__thrd_opt` argument is null, the priority of the current thread is
 * returned. If `__thrd_opt` is not null but doesn't point to a valid thread
 * object, the behavior is undefined.
 *
 * Returns the thread priority as an enumerator. No value has been reserved to
 * indicate an error.  */
__MCF_THREAD_IMPORT
_MCF_thread_priority
_MCF_thread_get_priority(const _MCF_thread* __thrd_opt)
  __MCF_noexcept;

/* Sets the priority of a thread.
 *
 * The `__priority` argument may be an arbitrary integer within -15 and +15, but
 * specifying an `_MCF_thread_priority_*` constant is recommended. Setting high
 * priority values may require certain privileges. If the `__thrd_opt` argument
 * is null, the priority of the current thread will be set. If `__thrd_opt` is
 * not null but doesn't point to a valid thread object, the behavior of this
 * function is undefined.
 *
 * Returns 0 upon success and -1 upon failure.  */
__MCF_THREAD_IMPORT
int
_MCF_thread_set_priority(_MCF_thread* __thrd_opt, _MCF_thread_priority __priority)
  __MCF_noexcept;

/* Gets a non-owning pointer to the current thread object. This function never
 * returns a null pointer. If the calling thread was not created by
 * `_MCF_thread_new_aligned()`, a thread object with no user data is allocated
 * and returned.  */
__MCF_THREAD_IMPORT __MCF_FN_CONST
_MCF_thread*
_MCF_thread_self(void)
  __MCF_noexcept;

/* Gets the thread ID of the current thread.  */
__MCF_THREAD_INLINE __MCF_FN_CONST
uint32_t
_MCF_thread_self_tid(void)
  __MCF_noexcept;

/* Gives up the current time slice.  */
__MCF_THREAD_IMPORT
void
_MCF_yield(void)
  __MCF_noexcept;

/* Suspends the calling thread for a given amount of time.
 *
 * If the `__timeout_opt` argument points to a positive integer, it denotes the
 * expiration time in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to sleep. If it points to a value of zero, the function returns
 * immediately. If it is null, the function sleeps indefinitely. This operation
 * can be interrupted by Ctrl-C events.
 *
 * Returns 0 if the operation has timed out, or -1 if an interrupt occurred.  */
__MCF_THREAD_IMPORT
int
_MCF_sleep(const int64_t* __timeout_opt)
  __MCF_noexcept;

/* Suspends the calling thread for a given amount of time.
 *
 * If the `__timeout_opt` argument points to a positive integer, it denotes the
 * expiration time in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to sleep. If it points to a value of zero, the function returns
 * immediately. If it is null, the function sleeps indefinitely.  */
__MCF_THREAD_IMPORT
void
_MCF_sleep_noninterruptible(const int64_t* __timeout_opt)
  __MCF_noexcept;

/* Gets a thread-local value. The calling thread shall have been created by
 * `_MCF_thread_new()`, or shall be the main thread.
 *
 * Returns the thread-local value if one has been set by the calling thread, or
 * a null pointer otherwise. No return value is reserved to indicate errors.  */
__MCF_THREAD_INLINE __MCF_FN_PURE
void*
_MCF_tls_get(const _MCF_tls_key* __key)
  __MCF_noexcept;

/* Sets a thread-local value. The calling thread shall have been created by
 * `_MCF_thread_new()`, or shall be the main thread.
 *
 * If `__old_value_opt` is not a null pointer and the new value has been set,
 * its old value is stored into `*__old_value_opt`.
 *
 * Returns 0 upon success and -1 upon failure.  */
__MCF_THREAD_INLINE
int
_MCF_tls_xset(_MCF_tls_key* __key, void** __old_value_opt, const void* __value_opt)
  __MCF_noexcept;

__MCF_THREAD_INLINE
int
_MCF_tls_set(_MCF_tls_key* __key, const void* __value_opt)
  __MCF_noexcept;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_THREAD_INLINE
_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* __proc, const void* __data_opt, size_t __size)
  __MCF_noexcept
  {
    return _MCF_thread_new_aligned(__proc, 0, __data_opt, __size);
  }

__MCF_THREAD_INLINE
__MCF_CXX(const) void*
_MCF_thread_get_data(const _MCF_thread* __thrd)
  __MCF_noexcept
  {
    return __builtin_assume_aligned(__thrd->__data_opt, __MCF_THREAD_DATA_ALIGNMENT);
  }

#ifdef __cplusplus
extern "C++" inline
void*
_MCF_thread_get_data(_MCF_thread* __thrd)
  __MCF_noexcept
  {
    return __builtin_assume_aligned(__thrd->__data_opt, __MCF_THREAD_DATA_ALIGNMENT);
  }
#endif  /* __cplusplus  */

__MCF_THREAD_INLINE
int32_t
_MCF_thread_get_ref(const _MCF_thread* __thrd)
  __MCF_noexcept
  {
    return _MCF_atomic_load_32_rlx(__thrd->__nref);
  }

__MCF_THREAD_INLINE
void
_MCF_thread_add_ref(_MCF_thread* __thrd)
  __MCF_noexcept
  {
    int32_t __old_ref = _MCF_atomic_xadd_32_rlx(__thrd->__nref, 1);
    __MCF_ASSERT(__old_ref < 0x7FFFFFFF);
    __MCF_ASSERT(__old_ref > 0);
  }

__MCF_THREAD_INLINE
void
_MCF_thread_drop_ref(_MCF_thread* __thrd_opt)
  __MCF_noexcept
  {
    if(__thrd_opt)
      _MCF_thread_drop_ref_nonnull(__thrd_opt);
  }

__MCF_THREAD_INLINE __MCF_CXX11(constexpr)
uint32_t
_MCF_thread_get_tid(const _MCF_thread* __thrd)
  __MCF_noexcept
  {
    return __thrd->__tid;
  }

__MCF_THREAD_INLINE __MCF_CXX11(constexpr)
__MCF_HANDLE
_MCF_thread_get_handle(const _MCF_thread* __thrd)
  __MCF_noexcept
  {
    return __thrd->__handle;
  }

__MCF_THREAD_INLINE
uint32_t
_MCF_thread_self_tid(void)
  __MCF_noexcept
  {
    uint32_t __tid;
    __MCF_TEB_LOAD_32_IMMEDIATE(&__tid, __MCF_64_32(0x48, 0x24));
    return __tid;
  }

__MCF_THREAD_INLINE
void*
_MCF_tls_get(const _MCF_tls_key* __key)
  __MCF_noexcept
  {
    _MCF_thread* __self = _MCF_thread_self();
    return __MCF_tls_table_get(__self->__tls_table, __key);
  }

__MCF_THREAD_INLINE
int
_MCF_tls_xset(_MCF_tls_key* __key, void** __old_value_opt, const void* __value_opt)
  __MCF_noexcept
  {
    _MCF_thread* __self = _MCF_thread_self();
    return __MCF_tls_table_xset(__self->__tls_table, __key, __old_value_opt, __value_opt);
  }

__MCF_THREAD_INLINE
int
_MCF_tls_set(_MCF_tls_key* __key, const void* __value_opt)
  __MCF_noexcept
  {
    _MCF_thread* __self = _MCF_thread_self();
    return __MCF_tls_table_xset(__self->__tls_table, __key, __MCF_nullptr, __value_opt);
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_THREAD_  */
