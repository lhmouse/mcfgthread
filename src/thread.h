// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_THREAD_H_
#define __MCFGTHREAD_THREAD_H_

#include "fwd.h"
#include "dtor_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCFGTHREAD_THREAD_C_
#  define __MCFGTHREAD_THREAD_INLINE  __MCF_GNU_INLINE
#else
#  define __MCFGTHREAD_THREAD_INLINE
#endif

// Define the prototype for thread procedures.
typedef void _MCF_thread_procedure(_MCF_thread* __thrd);

// Define the thread control struct.
struct __MCF_thread
  {
    int __nref[1];  // atomic reference count
    uint32_t __tid;  // thread id
    __MCF_HANDLE __handle;  // win32 thread handle

    __MCF_dtor_queue __atexit_queue;  // for `__cxa_thread_atexit()`

    _MCF_thread_procedure* __proc;  // user-defined thread procedure
    intptr_t __exit_code[1];
    char __data[0] __MCF_ALIGNED(16);  // user-defined data
  }
  typedef _MCF_thread;

// Creates a thread. The `__nref` member is initialized to 2.
//
// If `__size` is specified as non-zero, storage for user-defined data is
// reserved. The storage is initialized to zeroes. If `__data_opt` is non-null,
// it is used to initialize the storage before the new thread begins execution.
//
// Returns a new thread control structure. The caller is required to call
// `_MCF_thread_drop_ref()` when it is no longer needed. If the thread cannot
// be created, a null pointer is returned and an error code can be obtained
// via `_MCF_get_win32_error()`.
_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* __proc, const void* __data_opt, size_t __size) __MCF_NOEXCEPT;

// Gets a pointer to user-defined data of a thread.
__MCF_CXX(const) void*
_MCF_thread_get_data(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_CXX11(constexpr)
__MCFGTHREAD_THREAD_INLINE __MCF_CXX(const) void*
_MCF_thread_get_data(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return (char*) __thrd->__data;
  }

#ifdef __cplusplus
extern "C++"
__MCF_CXX11(constexpr)
inline void*
_MCF_thread_get_data(_MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return __thrd->__data;
  }
#endif  // __cplusplus

// Adds a reference count of a thread structure. This may be useful if you
// wish to pass a pointer to other code.
void
_MCF_thread_add_ref(_MCF_thread* __thrd) __MCF_NOEXCEPT;

__MCFGTHREAD_THREAD_INLINE void
_MCF_thread_add_ref(_MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    int __old_ref = __atomic_fetch_add(__thrd->__nref, 1, __ATOMIC_RELAXED);
    __MCFGTHREAD_ASSERT(__old_ref > 0);
  }

// Drops a reference count of a thread structure. An active thread owns a
// reference count of itself and `_MCF_thread_new()` returns another one. When
// the reference count is reduced to zero, the structure is deallocated.
void
_MCF_thread_drop_ref(_MCF_thread* __thrd) __MCF_NOEXCEPT;

// Gets the ID of a thread.
uint32_t
_MCF_thread_get_tid(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_CXX11(constexpr)
__MCFGTHREAD_THREAD_INLINE uint32_t
_MCF_thread_get_tid(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return __thrd->__tid;
  }

// Gets the handle of a thread.
__MCF_HANDLE
_MCF_thread_get_handle(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCF_CXX11(constexpr)
__MCFGTHREAD_THREAD_INLINE __MCF_HANDLE
_MCF_thread_get_handle(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return __thrd->__handle;
  }

// Gets the exit code of a thread.
// This may not be meaningful when the target thread is running.
intptr_t
_MCF_thread_get_exit_code(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  __attribute__((__pure__));

__MCFGTHREAD_THREAD_INLINE intptr_t
_MCF_thread_get_exit_code(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return __atomic_load_n(__thrd->__exit_code, __ATOMIC_ACQUIRE);
  }

// Exits from a thread.
void
_MCF_thread_exit(intptr_t __exit_code) __MCF_NOEXCEPT
  __attribute__((__noreturn__));

// Waits for a thread to finish execution.
//
// If the `timeout` argument points to a positive integer, it denotes the wait
// expiration time, in number of milliseconds since 1970-01-01T00:00:00Z. If it
// points to a negative integer, the absolute value of it denotes the number of
// milliseconds to wait. If it points to zero, the function returns immediately
// without waiting. If it is null, the function waits indefinitely.
//
// Returns 0 if the thread has terminated, or -1 if the wait operation has
// timed out.
int
_MCF_thread_wait(const _MCF_thread* __thrd, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

// Gets a non-owning pointer to the current thread object.
//
// IMPORTANT! This function is only meaningful for the main thread and threads
// that were created by `_MCF_thread_new()`.
_MCF_thread*
_MCF_thread_self(void) __MCF_NOEXCEPT
  __attribute__((__const__));

// Suspends the calling thread for a given amount of time.
//
// If the `timeout` argument points to a positive integer, it denotes the sleep
// expiration time, in number of milliseconds since 1970-01-01T00:00:00Z. If it
// points to a negative integer, the absolute value of it denotes the number of
// milliseconds to sleep. If it points to a value of zero, the function returns
// immediately. If it is null, the function sleeps indefinitely.
void
_MCF_sleep(const int64_t* __timeout_opt) __MCF_NOEXCEPT;

// This is the per-thread cleanup callback. It is declared here for the sake
// of completeness, and is not meant to be call directly.
void
__MCF_thread_exit_callback(void) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_THREAD_H_
