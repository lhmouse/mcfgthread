/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
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

/** This structure defines one value and its associated key.  */
struct __MCF_tls_element
  {
    _MCF_tls_key* __key_opt;  /* null denotes an empty element  */
    void* __value_opt;
  };

/** This structure implements a hash table of thread-local values.  */
struct __MCF_tls_table
  {
    __MCF_tls_element* __begin;  /* beginning of hash table  */
    __MCF_tls_element* __end;  /* end of hash table  */
    size_t __size_hint;  /* number of non-empty elements  */
  };

/** This is the thread-local key, which stores the destructor and deletion state.
 * The address of a thread-local key denotes a unique value in the thread-local
 * table of the current thread.  */
struct __MCF_tls_key
  {
    __MCF_BR(int32_t) __nref;  /* atomic reference count  */
    __MCF_BR(bool) __deleted;  /* deleted?  */
    uint8_t __reserved_bytes[3];
    _MCF_tls_dtor* __dtor_opt;  /* destructor, optional  */
  };

/** Creates a thread-local key.
 *
 * A thread-local key is used to get and set thread-local values, which are
 * exclusive to the calling thread. A key may be assigned a destructor. When a
 * thread exists, for every value which is not a null pointer and is associated
 * with a key with a destructor and has not been deleted, the destructor will be
 * invoked. The order of destructors during a thread's exit is unspecified.
 *
 * The caller shall call `_MCF_tls_key_drop_ref()` or `_MCF_tls_key_delete()` to
 * deallocate the thread-local key when it is no longer needed.
 *
 * @param `dtor_opt` is an optional pointer to the destructor for values.
 * @returns a new thread-local key. In case of out of memory, a null pointer is
 *     returned.  */
__MCF_TLS_IMPORT
_MCF_tls_key*
_MCF_tls_key_new(__MCF_cxa_dtor_any_ __dtor_opt)
  __MCF_noexcept;

/** Gets the reference count of a thread-local key.
 *
 * @param `key` shall point to a thread-local key.
 * @returns the reference count of the thread-local key.  */
__MCF_TLS_INLINE __MCF_FN_PURE
int32_t
_MCF_tls_key_get_ref(const _MCF_tls_key* __key)
  __MCF_noexcept;

/** Increments the reference count of a thread-local key.
 *
 * @param `key` shall point to a thread-local key.
 * @returns nothing.  */
__MCF_TLS_INLINE
void
_MCF_tls_key_add_ref(_MCF_tls_key* __key)
  __MCF_noexcept;

/** Decrements the reference count of a thread-local key, and if it is reduced to
 * zero, deallocates the thread-local key.
 *
 * @param `key` shall point to a thread-local key.
 * @returns nothing.  */
__MCF_TLS_IMPORT
void
_MCF_tls_key_drop_ref_nonnull(_MCF_tls_key* __key)
  __MCF_noexcept;

/** Decrements the reference count of a thread-local key, and if it is reduced to
 * zero, deallocates the thread-local key.
 *
 * This function does nothing if `key_opt` is a null pointer.
 *
 * @param `key_opt` is an optional pointer to a thread-local key.
 * @returns nothing.  */
__MCF_TLS_INLINE
void
_MCF_tls_key_drop_ref(_MCF_tls_key* __key_opt)
  __MCF_noexcept;

/** Marks a thread-local key deleted, decrements the reference count of it, and
 * if it is reduced to zero, deallocates the thread-local key.
 *
 * After this function has been called on a thread-local key, the destructor
 * will no longer be called. Users should ensure that all resources associated
 * with this key will eventually be deallocated.
 *
 * This function calls `_MCF_tls_key_drop_ref_nonnull()` implicitly.
 *
 * @param `key` shall point to a thread-local key.
 * @returns nothing.  */
__MCF_TLS_INLINE
void
_MCF_tls_key_delete_nonnull(_MCF_tls_key* __key)
  __MCF_noexcept;

/** Marks a thread-local key deleted, decrements the reference count of it, and
 * if it is reduced to zero, deallocates the thread-local key.
 *
 * After this function has been called on a thread-local key, the destructor
 * will no longer be called. Users should ensure that all resources associated
 * with this key will eventually be deallocated.
 *
 * This function does nothing if `key_opt` is a null pointer. This function
 * calls `_MCF_tls_key_drop_ref_nonnull()` implicitly.
 *
 * @param `key_opt` is an optional pointer to a thread-local key.
 * @returns nothing.  */
__MCF_TLS_INLINE
void
_MCF_tls_key_delete(_MCF_tls_key* __key_opt)
  __MCF_noexcept;

/** Gets the destructor of a thread-local key.
 *
 * @param `key` shall point to a thread-local key.
 * @returns the destructor of the thread-local key, which may be null if none
 *     was specified when the key was created.  */
__MCF_TLS_INLINE __MCF_CXX11(constexpr) __MCF_FN_PURE
_MCF_tls_dtor*
_MCF_tls_key_get_destructor(const _MCF_tls_key* __key)
  __MCF_noexcept;

/** Gets a value from a thread-local table.
 *
 * If the key has been marked deleted, a null pointer is returned.
 *
 * @param `table` shall point to a thread-local table.
 * @param `key` shall point to a thread-local key.
 * @returns the value associated with the key in the table, or a null pointer if
 *    no such value has been set. If the key has been marked deleted, a null
 *    pointer is also returned. No return value is reserved to indicate an error.  */
__MCF_TLS_IMPORT __MCF_FN_PURE
void*
__MCF_tls_table_get(const __MCF_tls_table* __table, const _MCF_tls_key* __key)
  __MCF_noexcept;

/** Sets a value into a thread-local table.
 *
 * If the key has been marked deleted, the function fails.
 *
 * @param `table` shall point to a thread-local table.
 * @param `key` shall point to a thread-local key.
 * @param `old_value_opt` is an optional pointer to a location where the old
 *     value will be stored.
 * @param `__value_opt` is the new value to associate with the key.
 * @returns 0 if the value has been set successfully, -1 if the key has been
 *     marked deleted, or -2 if out of memory.  */
__MCF_TLS_IMPORT
int
__MCF_tls_table_xset(__MCF_tls_table* __table, _MCF_tls_key* __key, void** __old_value_opt,
                     const void* __value_opt)
  __MCF_noexcept;

/** Define inline functions after all declarations.
 *
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_TLS_INLINE __MCF_FN_PURE
int32_t
_MCF_tls_key_get_ref(const _MCF_tls_key* __key)
  __MCF_noexcept
  {
    return _MCF_atomic_load_32_rlx(__key->__nref);
  }

__MCF_TLS_INLINE
void
_MCF_tls_key_add_ref(_MCF_tls_key* __key)
  __MCF_noexcept
  {
    int32_t __old_ref = _MCF_atomic_xadd_32_rlx(__key->__nref, 1);
    __MCF_ASSERT(__old_ref < 0x7FFFFFFF);
    __MCF_ASSERT(__old_ref > 0);
  }

__MCF_TLS_INLINE
void
_MCF_tls_key_drop_ref(_MCF_tls_key* __key_opt)
  __MCF_noexcept
  {
    if(__key_opt)
      _MCF_tls_key_drop_ref_nonnull(__key_opt);
  }

__MCF_TLS_INLINE
void
_MCF_tls_key_delete_nonnull(_MCF_tls_key* __key)
  __MCF_noexcept
  {
    _MCF_atomic_store_b_rlx(__key->__deleted, true);
    _MCF_tls_key_drop_ref_nonnull(__key);
  }

__MCF_TLS_INLINE
void
_MCF_tls_key_delete(_MCF_tls_key* __key_opt)
  __MCF_noexcept
  {
    if(__key_opt)
      _MCF_tls_key_delete_nonnull(__key_opt);
  }

__MCF_TLS_INLINE __MCF_CXX11(constexpr) __MCF_FN_PURE
_MCF_tls_dtor*
_MCF_tls_key_get_destructor(const _MCF_tls_key* __key)
  __MCF_noexcept
  {
    return __key->__dtor_opt;
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_TLS_  */
