/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2023, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_THREAD_
#define __MCFGTHREAD_THREAD_

#include "fwd.h"
#include "dtor_queue.h"
#include "tls.h"
#include "atomic.h"

__MCF_C_DECLARATIONS_BEGIN
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
    __extension__ char __data_storage[0] __attribute__((__aligned__(__MCF_THREAD_DATA_ALIGNMENT)));
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
_MCF_thread_new_aligned(_MCF_thread_procedure* __proc, size_t __align, const void* __data_opt, size_t __size) __MCF_NOEXCEPT;

__MCF_THREAD_INLINE
_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* __proc, const void* __data_opt, size_t __size) __MCF_NOEXCEPT;

/* Attaches a thread that was not created by `_MCF_thread_new_aligned()`. This
 * function takes ownership of the thread control structure and assigns it to
 * the calling thread, which shall not have already had a control structure.
 * The caller must initialize the struct properly. The fields `__nref`, `__tid`
 * and `__handle` must be zeroes.
 *
 * Returns `__thrd`. This function will not fail.  */
__MCF_THREAD_IMPORT
_MCF_thread*
__MCF_thread_attach_foreign(_MCF_thread* __thrd) __MCF_NOEXCEPT;

/* Gets a pointer to user-defined data of a thread. If the thread does not have
 * user-defined data, because zero was specified for the `__size` parameter to
 * `_MCF_thread_new()`, a null pointer is returned.  */
__MCF_THREAD_INLINE __MCF_CXX11(constexpr)
__MCF_CXX(const) void*
_MCF_thread_get_data(const _MCF_thread* __thrd) __MCF_NOEXCEPT __attribute__((__pure__));

/* Gets the number of references of a thread struct.  */
__MCF_THREAD_INLINE
int32_t
_MCF_thread_get_ref(const _MCF_thread* __thrd) __MCF_NOEXCEPT __attribute__((__pure__));

/* Adds a reference count of a thread structure. This may be useful if you
 * wish to pass a pointer to other code.  */
__MCF_THREAD_INLINE
void
_MCF_thread_add_ref(_MCF_thread* __thrd) __MCF_NOEXCEPT;

/* Drops a reference count of a thread structure. An active thread owns a
 * reference count of itself and `_MCF_thread_new()` returns another one. When
 * the reference count is reduced to zero, the structure is deallocated.  */
__MCF_THREAD_IMPORT
void
_MCF_thread_drop_ref_nonnull(_MCF_thread* __thrd) __MCF_NOEXCEPT;

__MCF_THREAD_INLINE
void
_MCF_thread_drop_ref(_MCF_thread* __thrd_opt) __MCF_NOEXCEPT;

/* Gets the ID of a thread.  */
__MCF_THREAD_INLINE __MCF_CXX11(constexpr)
uint32_t
_MCF_thread_get_tid(const _MCF_thread* __thrd) __MCF_NOEXCEPT __attribute__((__pure__));

/* Gets the handle of a thread.  */
__MCF_THREAD_INLINE __MCF_CXX11(constexpr)
__MCF_HANDLE
_MCF_thread_get_handle(const _MCF_thread* __thrd) __MCF_NOEXCEPT __attribute__((__pure__));

/* Exits from a thread.  */
__MCF_THREAD_IMPORT
void
_MCF_thread_exit(void) __MCF_NOEXCEPT __attribute__((__noreturn__));

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
_MCF_thread_wait(const _MCF_thread* __thrd_opt, const int64_t* __timeout_opt) __MCF_NOEXCEPT;

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
_MCF_thread_get_priority(const _MCF_thread* __thrd_opt) __MCF_NOEXCEPT;

/* Sets the priority of a thread.
 *
 * The `__priority` argument may be an arbitrary integer within -15 and +15, but
 * specifying an `_MCF_thread_priority_*` constant is recommended. Setting high
 * priority values may require certain priviledges. If the `__thrd_opt` argument
 * is null, the priority of the current thread will be set. If `__thrd_opt` is
 * not null but doesn't point to a valid thread object, the behavior of this
 * function is undefined.
 *
 * Returns 0 upon success and -1 upon failure.  */
__MCF_THREAD_IMPORT
int
_MCF_thread_set_priority(_MCF_thread* __thrd_opt, _MCF_thread_priority __priority) __MCF_NOEXCEPT;

/* Gets a non-owning pointer to the current thread object. If the calling thread
 * was not created by `_MCF_thread_new_aligned()`, a thread object with no user
 * data is allocated and returned.  */
__MCF_THREAD_IMPORT
_MCF_thread*
_MCF_thread_self(void) __MCF_NOEXCEPT __attribute__((__const__));

/* Gets the thread ID of the current thread.  */
__MCF_THREAD_INLINE
uint32_t
_MCF_thread_self_tid(void) __MCF_NOEXCEPT __attribute__((__const__));

/* Gives up the current time slice.  */
__MCF_THREAD_IMPORT
void
_MCF_yield(void) __MCF_NOEXCEPT;

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
_MCF_sleep(const int64_t* __timeout_opt) __MCF_NOEXCEPT;

/* Suspends the calling thread for a given amount of time.
 *
 * If the `__timeout_opt` argument points to a positive integer, it denotes the
 * expiration time in number of milliseconds since 1970-01-01T00:00:00Z. If it
 * points to a negative integer, the absolute value of it denotes the number of
 * milliseconds to sleep. If it points to a value of zero, the function returns
 * immediately. If it is null, the function sleeps indefinitely.  */
__MCF_THREAD_IMPORT
void
_MCF_sleep_noninterruptible(const int64_t* __timeout_opt) __MCF_NOEXCEPT;

/* Gets a thread-local value. The calling thread shall have been created by
 * `_MCF_thread_new()`, or shall be the main thread.
 *
 * Returns the thread-local value if one has been set by the calling thread, or
 * a null pointer otherwise. No return value is reserved to indicate errors.  */
__MCF_THREAD_INLINE
void*
_MCF_tls_get(const _MCF_tls_key* __key) __MCF_NOEXCEPT __attribute__((__pure__));

/* Sets a thread-local value. The calling thread shall have been created by
 * `_MCF_thread_new()`, or shall be the main thread.
 *
 * If `__old_value_opt` is not a null pointer and the new value has been set,
 * its old value is stored into `*__old_value_opt`.
 *
 * Returns 0 upon success and -1 upon failure.  */
__MCF_THREAD_INLINE
int
_MCF_tls_xset(_MCF_tls_key* __key, void** __old_value_opt, const void* __value_opt) __MCF_NOEXCEPT;

__MCF_THREAD_INLINE
int
_MCF_tls_set(_MCF_tls_key* __key, const void* __value_opt) __MCF_NOEXCEPT;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_THREAD_INLINE
_MCF_thread*
_MCF_thread_new(_MCF_thread_procedure* __proc, const void* __data_opt, size_t __size) __MCF_NOEXCEPT
  {
    return _MCF_thread_new_aligned(__proc, 0, __data_opt, __size);
  }

__MCF_THREAD_INLINE __MCF_CXX11(constexpr)
__MCF_CXX(const) void*
_MCF_thread_get_data(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return __builtin_assume_aligned(__thrd->__data_opt, __MCF_THREAD_DATA_ALIGNMENT);
  }

#ifdef __cplusplus
extern "C++"
inline __MCF_CXX11(constexpr)
void*
_MCF_thread_get_data(_MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return __builtin_assume_aligned(__thrd->__data_opt, __MCF_THREAD_DATA_ALIGNMENT);
  }
#endif  /* __cplusplus  */

__MCF_THREAD_INLINE
int32_t
_MCF_thread_get_ref(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return _MCF_atomic_load_32_rlx(__thrd->__nref);
  }

__MCF_THREAD_INLINE
void
_MCF_thread_add_ref(_MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    int32_t __old_ref = _MCF_atomic_xadd_32_rlx(__thrd->__nref, 1);
    __MCF_ASSERT(__old_ref < __INT32_MAX__);
    __MCF_ASSERT(__old_ref > 0);
  }

__MCF_THREAD_INLINE
void
_MCF_thread_drop_ref(_MCF_thread* __thrd_opt) __MCF_NOEXCEPT
  {
    if(__thrd_opt)
      _MCF_thread_drop_ref_nonnull(__thrd_opt);
  }

__MCF_THREAD_INLINE __MCF_CXX11(constexpr)
uint32_t
_MCF_thread_get_tid(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return __thrd->__tid;
  }

__MCF_THREAD_INLINE __MCF_CXX11(constexpr)
__MCF_HANDLE
_MCF_thread_get_handle(const _MCF_thread* __thrd) __MCF_NOEXCEPT
  {
    return __thrd->__handle;
  }

__MCF_THREAD_INLINE
uint32_t
_MCF_thread_self_tid(void) __MCF_NOEXCEPT
  {
    uint32_t __tid;
    char* __teb __attribute__((__unused__));
#if defined(__amd64__)
    /* Current TEB starts at `gs:0`.  */
    __asm__ ("{ movl %%gs:0x48, %0 | mov %0, dword ptr gs:[0x48] }" : "=r"(__tid));
#elif defined(__i386__)
    /* Current TEB starts at `fs:0`.  */
    __asm__ ("{ movl %%fs:0x24, %0 | mov %0, dword ptr fs:[0x24] }" : "=r"(__tid));
#elif defined(__aarch64__)
    /* Current TEB base is `x18`.  */
    __asm__ ("ldr %w0, [x18, #0x48]" : "=r"(__tid));
#elif defined(__arm__)
    /* Current TEB base is moved from co-processor p15.  */
    __asm__ ("mrc p15, #0, %0, c13, c0, #2" : "=r"(__teb));
    __tid = *(uint32_t*) (__teb + 0x24);
#else
#  error TODO: CPU not supported
#endif
    return __tid;
  }

__MCF_THREAD_INLINE
void*
_MCF_tls_get(const _MCF_tls_key* __key) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    return (__self == NULL)
        ? NULL  /* foreign thread  */
        : __MCF_tls_table_get(__self->__tls_table, __key);
  }

__MCF_THREAD_INLINE
int
_MCF_tls_xset(_MCF_tls_key* __key, void** __old_value_opt, const void* __value_opt) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    return (__self == NULL)
        ? -1  /* allocation failure  */
        : __MCF_tls_table_xset(__self->__tls_table, __key, __old_value_opt, __value_opt);
  }

__MCF_THREAD_INLINE
int
_MCF_tls_set(_MCF_tls_key* __key, const void* __value_opt) __MCF_NOEXCEPT
  {
    _MCF_thread* __self = _MCF_thread_self();
    return (__self == NULL)
        ? -1  /* allocation failure  */
        : __MCF_tls_table_xset(__self->__tls_table, __key, NULL, __value_opt);
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_THREAD_  */
