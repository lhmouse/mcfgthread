/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_THREAD_
#define __MCFGTHREAD_THREAD_

#include "fwd.h"
#include "dtor_queue.h"
#include "atomic.h"
#include "teb.h"
#include "tls.h"
#include "cpu_collection.h"
#ifdef __MCF_THREAD_DETAILS
#include "event.h"
#endif

__MCF_CXX(extern "C" {)
#ifndef __MCF_THREAD_IMPORT
#  define __MCF_THREAD_IMPORT
#  define __MCF_THREAD_INLINE  __MCF_GNU_INLINE
#endif

/** This is the control structure of a thread.  */
struct __MCF_thread_base
  {
    __MCF_ALIGNED(16) __MCF_BR(int32_t) __nref;  /* atomic reference count  */
    int32_t __tid;  /* thread id  */
    __MCF_HANDLE __handle;  /* win32 thread handle  */

    _MCF_thread_procedure* __proc;  /* user-defined thread procedure  */
    void* __data_opt;  /* pointer to user-defined data  */

    __MCF_BR(__MCF_dtor_queue) __atexit_queue;  /* for `__cxa_thread_atexit()`  */
    __MCF_BR(__MCF_tls_table) __tls_table;  /* for `_MCF_tls_get()` and `_MCF_tls_set()`  */

#ifdef __MCF_THREAD_DETAILS
    union {
#endif
      void* __libobjc_tls_data;  /* for GCC libobjc  */
#ifdef __MCF_THREAD_DETAILS
      __MCF_BR(_MCF_event) __init_done;
    };
#endif
  };

/** This is the default alignment of user-defined data that is guaranteed by
 * `_MCF_thread_new_aligned()`.  */
#define __MCF_THREAD_DATA_ALIGNMENT   16U

/** This is the maximum alignment of user-defined data that is supported by
 * `_MCF_thread_new_aligned()`. It is defined here for illustration purposes
 * only and you should not make assumption about it.  */
#define __MCF_THREAD_MAX_DATA_ALIGNMENT   0x200000U

/** These thread priority values match Windows APIs and can be passed around as
 * such, but we define only what we find necessary at the moment.  */
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

/** Creates a thread.
 *
 * The `__nref` member is initialized to 2, since a running thread holds a
 * reference to itself. The caller shall call `_MCF_thread_drop_ref()` when the
 * returned pointer is no longer needed.
 *
 * @param `thrdp_opt` is an optional pointer that indicates where to store a
 *     pointer to the new thread. If it is non-null, it is updated before the
 *     new thread begins execution. The value is the same as what this function
 *     will return.
 * @param `stack_size` specifies the stack reservation for the new thread. If it
 *     is zero, the default stack size of the current process will be used.
 * @param `proc` shall point to the thread procedure.
 * @param `data_alignment` specifies the alignment of user-defined data. If it
 *     is zero, `__MCF_THREAD_DATA_ALIGNMENT` is used; if it is non-zero, it
 *     shall be a power of two.
 * @param `data_opt` is an optional pointer to a memory block that will be copied
 *     to initialize user-defined data. If it is null, user-defined data will be
 *     zero-initialized.
 * @param `data_size` specifies the size of user-defined data. If it is zero, no
 *     user-defined data will be allocated.
 * @returns a pointer to the new thread control structure. If a thread cannot be
 *     created, a null pointer is returned and an error code can be obtained via
 *     `_MCF_get_win32_error()`.
 * @since 2.5  */
__MCF_THREAD_IMPORT
_MCF_thread*
_MCF_thread_p_new(_MCF_thread** __thrdp_opt, size_t __stack_size, _MCF_thread_procedure* __proc,
                  size_t __data_alignment, const void* __data_opt, size_t __data_size)
  __MCF_noexcept;

/** Creates a thread.
 *
 * The `__nref` member is initialized to 2, since a running thread holds a
 * reference to itself. The caller shall call `_MCF_thread_drop_ref()` when the
 * returned pointer is no longer needed.
 *
 * This function is an inline wrapper for `_MCF_thread_p_new(NULL, 0, proc,
 * data_alignment, data_opt, data_size)`.
 *
 * @param `proc` shall point to the thread procedure.
 * @param `data_alignment` specifies the alignment of user-defined data. If it
 *     is zero, `__MCF_THREAD_DATA_ALIGNMENT` is used; if it is non-zero, it
 *     shall be a power of two.
 * @param `data_opt` is an optional pointer to a memory block that will be copied
 *     to initialize user-defined data. If it is null, user-defined data will be
 *     zero-initialized.
 * @param `data_size` specifies the size of user-defined data. If it is zero, no
 *     user-defined data will be allocated.
 * @returns a pointer to the new thread control structure. If a thread cannot be
 *     created, a null pointer is returned and an error code can be obtained via
 *     `_MCF_get_win32_error()`.
 * @since 1.4  */
__MCF_THREAD_INLINE
_MCF_thread*
_MCF_thread_new_aligned(_MCF_thread_procedure* __proc, size_t __data_alignment,
                        const void* __data_opt, size_t __data_size)
  __MCF_noexcept;

/** Creates a thread.
 *
 * The `__nref` member is initialized to 2, since a running thread holds a
 * reference to itself. The caller shall call `_MCF_thread_drop_ref()` when the
 * returned pointer is no longer needed.
 *
 * This function is an inline wrapper for `_MCF_thread_p_new(NULL, 0, proc, 0,
 * data_opt, data_size)`.
 *
 * @param `proc` shall point to the thread procedure.
 * @param `data_opt` is an optional pointer to a memory block that will be copied
 *     to initialize user-defined data. If it is null, user-defined data will be
 *     zero-initialized.
 * @param `data_size` specifies the size of user-defined data. If it is zero, no
 *     user-defined data will be allocated.
 * @returns a pointer to the new thread control structure. If a thread cannot be
 *     created, a null pointer is returned and an error code can be obtained via
 *     `_MCF_get_win32_error()`.
 * @since 1.0  */
__MCF_THREAD_INLINE
_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* __proc, const void* __data_opt, size_t __data_size)
  __MCF_noexcept;

/** Attaches a thread that was not created by `_MCF_thread_new()`.
 *
 * This function takes ownership of the thread control structure and assigns it
 * to the calling thread, which shall not have already had a control structure.
 * The caller must initialize the structure properly; the fields `__nref`, `__tid`
 * and `__handle` must be zeroes.
 *
 * This function will not fail. If an error happens while allocating any
 * resources, the current process will be terminated. Be advised, this function
 * can be called implicitly by the compiler or from its runtime library. If
 * proper error handling is desired, it's recommended to ensure that all threads
 * be created with `_MCF_thread_new()`.
 *
 * @param `thrd` is a pointer to a thread control structure to attach.
 * @returns `thrd`.
 * @since 1.5  */
__MCF_THREAD_IMPORT
_MCF_thread*
__MCF_thread_attach_foreign(_MCF_thread* __thrd)
  __MCF_noexcept;

/** Gets a pointer to user-defined data of a thread.
 *
 * If the thread does not have user-defined data, a null pointer is returned.
 *
 * @param `thrd` is a pointer to a thread control structure.
 * @returns a pointer to the user-defined data, or null if none exists.
 * @since 1.0  */
__MCF_THREAD_INLINE __MCF_FN_PURE
__MCF_CXX(const) void*
_MCF_thread_get_data(const _MCF_thread* __thrd)
  __MCF_noexcept;

/** Gets the reference count of a thread.
 *
 * @param `thrd` shall point to a thread control structure.
 * @returns the reference count of the thread.
 * @since 1.4  */
__MCF_THREAD_INLINE __MCF_FN_PURE
int32_t
_MCF_thread_get_ref(const _MCF_thread* __thrd)
  __MCF_noexcept;

/** Increments the reference count of a thread.
 *
 * @param `thrd` shall point to a thread control structure.
 * @returns nothing.
 * @since 1.0  */
__MCF_THREAD_INLINE
void
_MCF_thread_add_ref(_MCF_thread* __thrd)
  __MCF_noexcept;

/** Decrements the reference count of a thread, and if it is reduced to zero,
 * deallocates the thread control structure.
 *
 * @param `thrd` shall point to a thread control structure.
 * @returns nothing.
 * @since 1.0  */
__MCF_THREAD_IMPORT
void
_MCF_thread_drop_ref_nonnull(_MCF_thread* __thrd)
  __MCF_noexcept;

/** Decrements the reference count of a thread, and if it is reduced to zero,
 * deallocates the thread control structure.
 *
 * This function does nothing if `thrd_opt` is a null pointer.
 *
 * @param `thrd_opt` is an optional pointer to a thread control structure.
 * @returns nothing.
 * @since 1.0  */
__MCF_THREAD_INLINE
void
_MCF_thread_drop_ref(_MCF_thread* __thrd_opt)
  __MCF_noexcept;

/** Gets the identifier of a thread.
 *
 * @param `thrd` shall point to a thread control structure.
 * @returns the identifier of the thread.
 * @since 1.0  */
__MCF_THREAD_INLINE __MCF_CXX11(constexpr) __MCF_FN_PURE
uint32_t
_MCF_thread_get_tid(const _MCF_thread* __thrd)
  __MCF_noexcept;

/** Gets the handle of a thread.
 *
 * This is the Windows handle that is returned by `CreateThread()`.
 *
 * @param `thrd` shall point to a thread control structure.
 * @returns the handle of the thread.
 * @since 1.0  */
__MCF_THREAD_INLINE __MCF_CXX11(constexpr) __MCF_FN_PURE
__MCF_HANDLE
_MCF_thread_get_handle(const _MCF_thread* __thrd)
  __MCF_noexcept;

/** Terminates the current thread.
 *
 * This function performs thread-local cleanup before calling `ExitThread()`, to
 * avoid potential deadlocks in `DllMain()` or TLS callbacks.
 *
 * This function does not return.
 *
 * @since 1.0  */
__MCF_THREAD_IMPORT __MCF_NEVER_RETURN
void
_MCF_thread_exit(void)
  __MCF_noexcept;

/** Waits for a thread to terminate.
 *
 * @param `thrd_opt` points to the thread to wait for. It's not recommended to
 *     pass a null pointer, which indicates the calling thread itself and the
 *     operation will always time out.
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns 0 if the thread has terminated, or -1 if the wait operation has
 *     timed out.
 * @since 1.0  */
__MCF_THREAD_IMPORT
int
_MCF_thread_wait(const _MCF_thread* __thrd_opt, const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Gets the priority of a thread.
 *
 * @param `thrd_opt` points to the thread to get the priority of. If it is null,
 *     the priority of the current thread is returned.
 * @returns the priority of the thread as an integer. No value has been reserved
 *     to indicate an error.
 * @since 1.3  */
__MCF_THREAD_IMPORT
_MCF_thread_priority
_MCF_thread_get_priority(const _MCF_thread* __thrd_opt)
  __MCF_noexcept;

/** Sets the priority of a thread.
 *
 * @param `thrd_opt` points to the thread to set the priority of. If it is null,
 *     the priority of the current thread is changed.
 * @param `priority` is the new priority of the thread. It may be an arbitrary
 *     integer within -15 and +15, but specifying an `_MCF_thread_priority_*`
 *     constant is recommended.
 * @returns 0 upon success, -1 in case of an invalid argument, or -2 in case of
 *     a system error which can be obtained via `_MCF_get_win32_error()`.
 * @since 1.3  */
__MCF_THREAD_IMPORT
int
_MCF_thread_set_priority(_MCF_thread* __thrd_opt, _MCF_thread_priority __priority)
  __MCF_noexcept;

/** Gets a non-owning pointer to the control structure of the current thread.
 *
 * This function never returns a null pointer. If the calling thread was not
 * created by `_MCF_thread_p_new()`, a thread control structure with no user data
 * is allocated and returned.
 *
 * @returns the control structure of the current thread.
 * @since 1.0  */
__MCF_THREAD_IMPORT __MCF_FN_CONST
_MCF_thread*
_MCF_thread_self(void)
  __MCF_noexcept;

/** Gets the identifier of the current thread.
 *
 * This is the same value that is returned by `GetCurrentThreadId()`.
 *
 * @returns the identifier of the current thread as an unsigned integer.
 * @since 1.0  */
__MCF_THREAD_INLINE __MCF_FN_CONST
uint32_t
_MCF_thread_self_tid(void)
  __MCF_noexcept;

/** Gives up the current time slice to other threads.
 *
 * @returns nothing.
 * @since 1.0  */
__MCF_THREAD_IMPORT
void
_MCF_yield(void)
  __MCF_noexcept;

/** Suspends the calling thread for a given amount of time.
 *
 * This operation can be interrupted by Ctrl+C.
 *
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns 0 if the operation has timed out, or -1 if the operation was
 *     interrupted.
 * @since 1.0  */
__MCF_THREAD_IMPORT
int
_MCF_sleep(const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Suspends the calling thread for a given amount of time.
 *
 * This operation cannot be interrupted.
 *
 * @param `timeout_opt` points to the timeout value. If it is positive, it
 *     denotes the expiration time point in the number of milliseconds since
 *     1970-01-01T00:00:00Z. If it points to a negative integer, the absolute
 *     value of it denotes the number of milliseconds to wait. If it points to
 *     zero, the function returns immediately without waiting. If it is null,
 *     the function waits indefinitely.
 * @returns nothing.
 * @since 1.3  */
__MCF_THREAD_IMPORT
void
_MCF_sleep_noninterruptible(const int64_t* __timeout_opt)
  __MCF_noexcept;

/** Gets a thread-local value.
 *
 * If the key has been marked deleted, a null pointer is returned.
 *
 * @param `key` shall point to a thread-local key.
 * @returns the value associated with the key, or a null pointer if no such
 *     value has been set. If the key has been marked deleted, a null pointer is
 *     also returned. No return value is reserved to indicate an error.
 * @since 1.0  */
__MCF_THREAD_INLINE __MCF_FN_PURE
void*
_MCF_tls_get(const _MCF_tls_key* __key)
  __MCF_noexcept;

/** Sets a thread-local value and returns its old value.
 *
 * If the key has been marked deleted, the function fails.
 *
 * @param `key` shall point to a thread-local key.
 * @param `old_value_opt` is an optional pointer to a location where the old
 *     value will be stored.
 * @param `__value_opt` is the new value to associate with the key.
 * @returns 0 if the value has been set successfully, -1 if the key has been
 *     marked deleted, or -2 if out of memory.
 * @since 1.4  */
__MCF_THREAD_INLINE
int
_MCF_tls_xset(_MCF_tls_key* __key, void** __old_value_opt, const void* __value_opt)
  __MCF_noexcept;

/** Sets a thread-local value.
 *
 * If the key has been marked deleted, the function fails.
 *
 * @param `key` shall point to a thread-local key.
 * @param `__value_opt` is the new value to associate with the key.
 * @returns 0 if the value has been set successfully, -1 if the key has been
 *     marked deleted, or -2 if out of memory.
 * @since 1.0  */
__MCF_THREAD_INLINE
int
_MCF_tls_set(_MCF_tls_key* __key, const void* __value_opt)
  __MCF_noexcept;

/** Gets the affinity of a thread.
 *
 * The caller shall allocate and supply a CPU collection, where the function
 * will select CPUs according to the affinity of the target thread.
 *
 * @param `thrd_opt` points to the thread to get the affinity of. If it is null,
 *     the affinity of the current thread is returned.
 * @param `coll` points to a CPU collection where to set CPU selections.
 * @returns 0 upon success, -1 in case of an invalid argument, or -2 in case of
 *     a system error which can be obtained via `_MCF_get_win32_error()`.
 * @since 2.5  */
__MCF_THREAD_IMPORT
int
_MCF_thread_get_affinity(const _MCF_thread* __thrd_opt, _MCF_cpu_collection* __coll)
  __MCF_noexcept;

/** Sets the affinity of a thread.
 *
 * The caller shall allocate and supply a CPU collection to indicate which CPUs
 * the target thread is allowed to run on. The caller shall not specify a CPU
 * collection with no selection; the function will fail.
 *
 * @param `thrd_opt` points to the thread to set the affinity of. If it is null,
 *     the affinity of the current thread is changed.
 * @param `coll` points to a CPU collection where to set CPU selections.
 * @returns 0 upon success, -1 in case of an invalid argument, or -2 in case of
 *     a system error which can be obtained via `_MCF_get_win32_error()`.
 * @since 2.5  */
__MCF_THREAD_IMPORT
int
_MCF_thread_set_affinity(_MCF_thread* __thrd_opt, const _MCF_cpu_collection* __coll)
  __MCF_noexcept;

/** Define inline functions after all declarations.
 *
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_THREAD_INLINE
_MCF_thread*
_MCF_thread_new_aligned(_MCF_thread_procedure* __proc, size_t __data_alignment,
                        const void* __data_opt, size_t __data_size)
  __MCF_noexcept
  {
    return _MCF_thread_p_new(__MCF_nullptr, 0, __proc, __data_alignment, __data_opt, __data_size);
  }

__MCF_THREAD_INLINE
_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* __proc, const void* __data_opt, size_t __data_size)
  __MCF_noexcept
  {
    return _MCF_thread_p_new(__MCF_nullptr, 0, __proc, 0, __data_opt, __data_size);
  }

__MCF_THREAD_INLINE __MCF_FN_PURE
__MCF_CXX(const) void*
_MCF_thread_get_data(const _MCF_thread* __thrd)
  __MCF_noexcept
  {
    return __builtin_assume_aligned(__thrd->__data_opt, __MCF_THREAD_DATA_ALIGNMENT);
  }

#ifdef __cplusplus
extern "C++" inline __MCF_FN_PURE
void*
_MCF_thread_get_data(_MCF_thread* __thrd)
  __MCF_noexcept
  {
    return __builtin_assume_aligned(__thrd->__data_opt, __MCF_THREAD_DATA_ALIGNMENT);
  }
#endif  /* __cplusplus  */

__MCF_THREAD_INLINE __MCF_FN_PURE
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

__MCF_THREAD_INLINE __MCF_CXX11(constexpr) __MCF_FN_PURE
uint32_t
_MCF_thread_get_tid(const _MCF_thread* __thrd)
  __MCF_noexcept
  {
    return (uint32_t) __thrd->__tid;
  }

__MCF_THREAD_INLINE __MCF_CXX11(constexpr) __MCF_FN_PURE
__MCF_HANDLE
_MCF_thread_get_handle(const _MCF_thread* __thrd)
  __MCF_noexcept
  {
    return __thrd->__handle;
  }

__MCF_THREAD_INLINE __MCF_FN_CONST
uint32_t
_MCF_thread_self_tid(void)
  __MCF_noexcept
  {
    return (uint32_t) __MCF_tid();
  }

__MCF_THREAD_INLINE __MCF_FN_PURE
void*
_MCF_tls_get(const _MCF_tls_key* __key)
  __MCF_noexcept
  {
    return __MCF_tls_table_get(_MCF_thread_self()->__tls_table, __key);
  }

__MCF_THREAD_INLINE
int
_MCF_tls_xset(_MCF_tls_key* __key, void** __old_value_opt, const void* __value_opt)
  __MCF_noexcept
  {
    return __MCF_tls_table_xset(_MCF_thread_self()->__tls_table, __key,
                                __old_value_opt, __value_opt);
  }

__MCF_THREAD_INLINE
int
_MCF_tls_set(_MCF_tls_key* __key, const void* __value_opt)
  __MCF_noexcept
  {
    return __MCF_tls_table_xset(_MCF_thread_self()->__tls_table, __key,
                                __MCF_nullptr, __value_opt);
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_THREAD_  */
