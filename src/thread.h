// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_THREAD_H_
#define __MCFGTHREAD_THREAD_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MCFGTHREAD_THREAD_C_
#  define __MCFGTHREAD_THREAD_INLINE  __MCF_GNU_INLINE
#else
#  define __MCFGTHREAD_THREAD_INLINE
#endif

// Define the thread control struct.
struct __MCF_thread
  {
    volatile int __nref[1];  // atomic reference count
    uint32_t __tid;  // thread id
    void* __handle;  // win32 thread handle
    intptr_t __exit_code;
    _MCF_thread_procedure* __proc;  // user-defined thread procedure
    alignas(16) char __data[0];  // user-defined data
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
// via `GetLastError()`.
_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* __proc, const void* __data_opt, size_t __size) __MCF_NOEXCEPT;

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

// Exits from a thread.
void
_MCF_thread_exit(intptr_t __exit_code) __MCF_NOEXCEPT
  __attribute__((__noreturn__));

// Gets a non-owning pointer to the current thread object.
//
// IMPORTANT! This function is only meaningful for the main thread and threads
// that were created by `_MCF_thread_new()`.
_MCF_thread*
_MCF_thread_self(void) __MCF_NOEXCEPT
  __attribute__((__const__));

// This is the per-thread cleanup callback. It is declared here for the sake
// of completeness, and is not meant to be call directly.
void
__MCF_thread_exit_callback(void) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_THREAD_H_
