/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_CPU_COLLECTION_
#define __MCFGTHREAD_CPU_COLLECTION_

#include "fwd.h"
#include "atomic.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_CPU_COLLECTION_IMPORT
#  define __MCF_CPU_COLLECTION_IMPORT
#  define __MCF_CPU_COLLECTION_INLINE  __MCF_GNU_INLINE
#endif

/* Define the structure for a collection of CPUs.  */
struct __MCF_cpu_element
  {
    uint32_t __id;  /* CPU identifier; read-only  */
    uint16_t __group;  /* processor group index; read-only  */
    __MCF_EX uint16_t __reserved_1 : 15;
    __MCF_EX uint16_t __selected : 1;
  };

struct __MCF_cpu_collection
  {
    __MCF_BR(int32_t) __nref;  /* atomic reference count  */
    uint32_t __size;  /* number of elements in `__data`  */
    __MCF_cpu_element __data[1];
  };

/* Allocates a collection of all active CPUs. On modern versions of Windows
 * where CPU Set APIs are available, this function returns all active CPUs on
 * the system; otherwise only active CPUs in the processor group of the current
 * process are returned.
 *
 * Returns a new collection. `__size` is set to the number of active CPUs. For
 * each CPU, `__id` is set to its identifier, `__group` is set to its processor
 * group index, and `__selected` is initialized to zero. The caller shall call
 * `_MCF_cpu_collection_drop_ref()` when the collection is no longer needed. If
 * the collection cannot be allocated, a null pointer is returned and an error
 * code can be obtained via `_MCF_get_win32_error()`.  */
__MCF_CPU_COLLECTION_IMPORT
_MCF_cpu_collection*
_MCF_cpu_collection_new(void)
  __MCF_noexcept;

/* Creates a copy of a CPU collection.
 *
 * Returns a new collection which is a copy of `__src`. The caller shall call
 * `_MCF_cpu_collection_drop_ref()` when the collection is no longer needed. In
 * case of out of memory, a null pointer is returned.  */
__MCF_CPU_COLLECTION_IMPORT
_MCF_cpu_collection*
_MCF_cpu_collection_new_copy(const _MCF_cpu_collection* __src)
  __MCF_noexcept;

/* Get the number of references of a collection.  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
int32_t
_MCF_cpu_collection_get_ref(const _MCF_cpu_collection* __coll)
  __MCF_noexcept;

/* Adds a reference count of a collection. This may be useful if you wish to
 * pass a pointer to other code.  */
__MCF_CPU_COLLECTION_INLINE
void
_MCF_cpu_collection_add_ref(_MCF_cpu_collection* __coll)
  __MCF_noexcept;

/* Drops a reference count of a collection. When the reference count is reduced
 * to zero, the structure is deallocated.  */
__MCF_CPU_COLLECTION_IMPORT
void
_MCF_cpu_collection_drop_ref_nonnull(_MCF_cpu_collection* __coll)
  __MCF_noexcept;

__MCF_CPU_COLLECTION_INLINE
void
_MCF_cpu_collection_drop_ref(_MCF_cpu_collection* __coll_opt)
  __MCF_noexcept;

/* Gets the number of CPUs in a collection.  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_size(const _MCF_cpu_collection* __coll)
  __MCF_noexcept;

/* Gets the identifier of the CPU at `__index` in the collection.  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_id_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept;

/* Gets the processor group index of the CPU at `__index` in the collection.  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_group_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept;

/* Gets the selection state of the CPU at `__index` in the collection.  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
bool
_MCF_cpu_collection_get_selection_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept;

/* Sets the selection state of the CPU at `__index` in the collection and
 * returns the previous state.  */
__MCF_CPU_COLLECTION_INLINE
bool
_MCF_cpu_collection_set_selection_by_index(_MCF_cpu_collection* __coll, uint32_t __index,
                                           bool __selected)
  __MCF_noexcept;

/* Selects or deselects all CPUs in the collection.  */
__MCF_CPU_COLLECTION_INLINE
void
_MCF_cpu_collection_set_all_selections(_MCF_cpu_collection* __coll, bool __selected)
  __MCF_noexcept;

/* Returns the index of a CPU in the collection. If the CPU is not found,
 * `UINT32_MAX` is returned.  */
__MCF_CPU_COLLECTION_IMPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_find(const _MCF_cpu_collection* __coll, uint32_t __id)
  __MCF_noexcept;

/* Returns the processor group of a CPU in the collection. If the CPU is not
 * found, `UINT32_MAX` is returned.  */
__MCF_CPU_COLLECTION_IMPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_group(const _MCF_cpu_collection* __coll, uint32_t __id)
  __MCF_noexcept;

/* Gets the selection state of a CPU in the collection.  */
__MCF_CPU_COLLECTION_IMPORT __MCF_FN_PURE
bool
_MCF_cpu_collection_get_selection(const _MCF_cpu_collection* __coll, uint32_t __id)
  __MCF_noexcept;

/* Selects or deselects a CPU in the collection. This function fails if `__id`
 * doesn't exist in the collection. There's no way to add a new CPU into the
 * collection.
 *
 * Returns 1 if the CPU was selected before the call, 0 if the CPU was
 * unselected before the call, or -1 if the CPU does not exist in the collection.  */
__MCF_CPU_COLLECTION_IMPORT
int
_MCF_cpu_collection_set_selection(_MCF_cpu_collection* __coll, uint32_t __id, bool __selected)
  __MCF_noexcept;

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
int32_t
_MCF_cpu_collection_get_ref(const _MCF_cpu_collection* __coll)
  __MCF_noexcept
  {
    return _MCF_atomic_load_32_rlx(__coll->__nref);
  }

__MCF_CPU_COLLECTION_INLINE
void
_MCF_cpu_collection_add_ref(_MCF_cpu_collection* __coll)
  __MCF_noexcept
  {
    int32_t __old_ref = _MCF_atomic_xadd_32_rlx(__coll->__nref, 1);
    __MCF_ASSERT(__old_ref < 0x7FFFFFFF);
    __MCF_ASSERT(__old_ref > 0);
  }

__MCF_CPU_COLLECTION_INLINE
void
_MCF_cpu_collection_drop_ref(_MCF_cpu_collection* __coll_opt)
  __MCF_noexcept
  {
    if(__coll_opt)
      _MCF_cpu_collection_drop_ref_nonnull(__coll_opt);
  }

__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_size(const _MCF_cpu_collection* __coll)
  __MCF_noexcept
  {
    return __coll->__size;
  }

__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_id_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept
  {
    __MCF_ASSERT(__index < __coll->__size);
    return __coll->__data[__index].__id;
  }

__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_group_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept
  {
    __MCF_ASSERT(__index < __coll->__size);
    return __coll->__data[__index].__group;
  }

__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
bool
_MCF_cpu_collection_get_selection_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept
  {
    __MCF_ASSERT(__index < __coll->__size);
    return __coll->__data[__index].__selected;
  }

__MCF_CPU_COLLECTION_INLINE
bool
_MCF_cpu_collection_set_selection_by_index(_MCF_cpu_collection* __coll, uint32_t __index,
                                           bool __selected)
  __MCF_noexcept
  {
    int __old_selected;
    __MCF_ASSERT(__index < __coll->__size);
    __old_selected = __coll->__data[__index].__selected;
    __coll->__data[__index].__selected = __selected;
    return __old_selected;
  }

__MCF_CPU_COLLECTION_INLINE
void
_MCF_cpu_collection_set_all_selections(_MCF_cpu_collection* __coll, bool __selected)
  __MCF_noexcept
  {
    uint32_t __index;
    for(__index = 0;  __index < __coll->__size;  ++__index)
      __coll->__data[__index].__selected = __selected;
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_CPU_COLLECTION_  */
