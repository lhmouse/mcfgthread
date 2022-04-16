// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_TLS_H_
#define __MCFGTHREAD_TLS_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MCFGTHREAD_TLS_C_
#  define __MCFGTHREAD_TLS_INLINE
#else
#  define __MCFGTHREAD_TLS_INLINE  __MCF_GNU_INLINE
#endif

// Define the prototype for destructors for `_MCF_tls_key_new()`.
typedef void _MCF_tls_dtor(void* __ptr);

// Define the table structure that manages all thread-local objects.
struct __MCF_tls_element
  {
    _MCF_tls_key* __key_opt;  // null denotes an empty element
    void* __value;
  }
  typedef __MCF_tls_element;

struct __MCF_tls_table
  {
    __MCF_tls_element* __begin;  // beginning of hash table
    __MCF_tls_element* __end;  // end of hash table
    size_t __size;  // number of non-empty elements
  }
  typedef __MCF_tls_table;

// Define the structure for thread-local storage keys.
struct __MCF_tls_key
  {
    int __nref[1];  // atomic reference count
    uint8_t __deleted[1];  // deleted?
    uint8_t __reserved[3];
    _MCF_tls_dtor* __dtor_opt;  // destructor, optional
  }
  typedef _MCF_tls_key;

// Creates a thread-local key. The `__nref` member is initialized to 1.
//
// A thread-local key is used to get and set thread-local values, which are
// exclusive to the calling thread. A key may be assigned a destructor. When a
// thread exists, if its value that is associated with a key is non-null, and
// the key has a destructor, the destructor is invoked for the value. The order
// of multiple destructors during a thread's exit is unspecified.
//
// Returns a new thread-local key. The caller is responsible for calling
// `_MCF_tls_key_delete()` when it is no longer needed. If the thread-local key
// cannot be created, a null pointer is returned.
_MCF_tls_key*
_MCF_tls_key_new(_MCF_tls_dtor* __dtor_opt) __MCF_NOEXCEPT;

// Deletes a thread-local key.
//
// Deleting a key prevents the destructor from being invoked thereafter. It is
// the application's responsibility to ensure that all resources about values
// that are attached to this key are deallocated. Passing a deleted key to
// other functions results in undefined behavior.
void
_MCF_tls_key_delete(_MCF_tls_key* __key_opt) __MCF_NOEXCEPT;

void
_MCF_tls_key_delete_nonnull(_MCF_tls_key* __key) __MCF_NOEXCEPT;

__MCFGTHREAD_TLS_INLINE void
_MCF_tls_key_delete(_MCF_tls_key* __key_opt) __MCF_NOEXCEPT
  {
    if(__key_opt)
      _MCF_tls_key_delete_nonnull(__key_opt);
  }

// Gets a value from the table.
//
// Returns the thread-local value if one has been set, or a null pointer
// otherwise. No return value is reserved to indicate errors.
void*
__MCF_tls_table_get(const __MCF_tls_table* __table, const _MCF_tls_key* __key) __MCF_NOEXCEPT
  __attribute__((__pure__));

// Sets a value into the table.
//
// Returns 0 upon success and -1 upon failure.
int
__MCF_tls_table_set(__MCF_tls_table* __table, _MCF_tls_key* __key, const void* __value) __MCF_NOEXCEPT;

// Executes all destructors in the table, and frees dynamic storage if any. It
// is declared here for the sake of completeness, and is not meant to be call
// directly.
void
__MCF_tls_table_finalize(__MCF_tls_table* __table) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_TLS_H_
