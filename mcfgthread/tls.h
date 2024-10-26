/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_TLS_
#define __MCFGTHREAD_TLS_

#include "fwd.h"
#include "atomic.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_TLS_IMPORT
#  define __MCF_TLS_IMPORT
#  define __MCF_TLS_INLINE  __MCF_GNU_INLINE
#endif

/* Define the table structure that manages all thread-local objects.  */
struct __MCF_tls_element
  {
    _MCF_tls_key* __key_opt;  /* null denotes an empty element  */
    void* __value_opt;
  };

struct __MCF_tls_table
  {
    __MCF_tls_element* __begin;  /* beginning of hash table  */
    __MCF_tls_element* __end;  /* end of hash table  */
    size_t __size_hint;  /* number of non-empty elements  */
  };

/* Define the structure for thread-local storage keys.  */
struct __MCF_tls_key
  {
    int32_t __nref[1];  /* atomic reference count  */
    uint8_t __deleted[1];  /* deleted?  */
    uint8_t __reserved_bytes[3];
    _MCF_tls_dtor* __dtor_opt;  /* destructor, optional  */
  };

/* Creates a thread-local key. The `__nref` member is initialized to 1.
 *
 * A thread-local key is used to get and set thread-local values, which are
 * exclusive to the calling thread. A key may be assigned a destructor. When a
 * thread exists, for every value which is not a null pointer and is associated
 * with a key that has a destructor and has not been deleted, the destructor is
 * invoked. The order of destructors during a thread's exit is unspecified.
 *
 * Returns a new thread-local key. The caller is responsible for calling
 * `_MCF_tls_key_delete()` when it is no longer needed. If the thread-local key
 * cannot be created, a null pointer is returned and an error code can be
 * obtained via `_MCF_get_win32_error()`.  */
__MCF_TLS_IMPORT
_MCF_tls_key*
_MCF_tls_key_new(__MCF_cxa_dtor_any_t __dtor_opt) __MCF_noexcept;

/* Get the number of references of a thread-local key.  */
__MCF_TLS_INLINE __MCF_FN_PURE
int32_t
_MCF_tls_key_get_ref(const _MCF_tls_key* __key) __MCF_noexcept;

/* Adds a reference count of a thread-local key. This may be useful if you
 * wish to pass a pointer to other code.  */
__MCF_TLS_INLINE
void
_MCF_tls_key_add_ref(_MCF_tls_key* __key) __MCF_noexcept;

/* Drops a reference count of a thread-local key. When the reference count is
 * reduced to zero, the structure is deallocated.  */
__MCF_TLS_IMPORT
void
_MCF_tls_key_drop_ref_nonnull(_MCF_tls_key* __key) __MCF_noexcept;

__MCF_TLS_INLINE
void
_MCF_tls_key_drop_ref(_MCF_tls_key* __key_opt) __MCF_noexcept;

/* Marks a thread-local key as deleted and drops a reference count of it. This
 * prevents the destructor from being invoked thereafter. Users should ensure
 * that all resources associated with this key will eventually be deallocated.
 * This function makes a call to `_MCF_tls_key_drop_ref_nonnull()` implicitly.  */
__MCF_TLS_INLINE
void
_MCF_tls_key_delete_nonnull(_MCF_tls_key* __key) __MCF_noexcept;

__MCF_TLS_INLINE
void
_MCF_tls_key_delete(_MCF_tls_key* __key_opt) __MCF_noexcept;

/* Gets the destructor, which may be null if none has been given.  */
__MCF_TLS_INLINE __MCF_FN_PURE __MCF_CXX11(constexpr)
_MCF_tls_dtor*
_MCF_tls_key_get_destructor(const _MCF_tls_key* __key) __MCF_noexcept;

/* Gets a value from the table.
 *
 * Returns the thread-local value if one has been set, or a null pointer
 * otherwise. No return value is reserved to indicate errors.  */
__MCF_TLS_IMPORT __MCF_FN_PURE
void*
__MCF_tls_table_get(const __MCF_tls_table* __table, const _MCF_tls_key* __key) __MCF_noexcept;

/* Sets a value into the table.
 *
 * If `__old_value_opt` is not a null pointer and the new value has been set,
 * its old value is stored into `*__old_value_opt`.
 *
 * Returns 0 upon success and -1 upon failure.  */
__MCF_TLS_IMPORT
int
__MCF_tls_table_xset(__MCF_tls_table* __table, _MCF_tls_key* __key, void** __old_value_opt, const void* __value_opt) __MCF_noexcept;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_TLS_INLINE
int32_t
_MCF_tls_key_get_ref(const _MCF_tls_key* __key) __MCF_noexcept
  {
    return _MCF_atomic_load_32_rlx(__key->__nref);
  }

__MCF_TLS_INLINE
void
_MCF_tls_key_add_ref(_MCF_tls_key* __key) __MCF_noexcept
  {
    int32_t __old_ref = _MCF_atomic_xadd_32_rlx(__key->__nref, 1);
    __MCF_ASSERT(__old_ref < 0x7FFFFFFF);
    __MCF_ASSERT(__old_ref > 0);
  }

__MCF_TLS_INLINE
void
_MCF_tls_key_drop_ref(_MCF_tls_key* __key_opt) __MCF_noexcept
  {
    if(__key_opt)
      _MCF_tls_key_drop_ref_nonnull(__key_opt);
  }

__MCF_TLS_INLINE
void
_MCF_tls_key_delete_nonnull(_MCF_tls_key* __key) __MCF_noexcept
  {
    _MCF_atomic_store_8_rlx(__key->__deleted, 1);
    _MCF_tls_key_drop_ref_nonnull(__key);
  }

__MCF_TLS_INLINE
void
_MCF_tls_key_delete(_MCF_tls_key* __key_opt) __MCF_noexcept
  {
    if(__key_opt)
      _MCF_tls_key_delete_nonnull(__key_opt);
  }

__MCF_TLS_INLINE __MCF_CXX11(constexpr)
_MCF_tls_dtor*
_MCF_tls_key_get_destructor(const _MCF_tls_key* __key) __MCF_noexcept
  {
    return __key->__dtor_opt;
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_TLS_  */
