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

/** This structure describes one logical CPU in a CPU collection.  */
struct __MCF_cpu_element
  {
    uint32_t __id;  /* CPU identifier; read-only  */
    uint16_t __group;  /* processor group index; read-only  */
    uint8_t __core_idx;  /* core index in physical package; read-only  */
    bool __selected;
    uint8_t __effic_cls;  /* `PROCESSOR_RELATIONSHIP::EfficiencyClass`  */
    uint8_t __sched_cls;  /* `SYSTEM_CPU_SET_INFORMATION::CpuSet.SchedulingClass`  */
    uint16_t __reserved_x2;
  };

/** This is the header of a CPU collection object that is allocated by
 * `_MCF_cpu_collection_new()`. More elements may follow `__data`.  */
struct __MCF_cpu_collection
  {
    __MCF_BR(int32_t) __nref;  /* atomic reference count  */
    uint32_t __size;  /* number of elements in `__data`  */
    __MCF_cpu_element __data[1];
  };

/** Allocates a collection of all active CPUs.
 *
 * On modern versions of Windows where CPU Set APIs are available, this function
 * returns all active CPUs on the system; otherwise, only active CPUs in the
 * processor group of the current process are returned. Initially, all CPUs are
 * unselected.
 *
 * The caller shall call `_MCF_cpu_collection_drop_ref()` to deallocate the CPU
 * collection when it is no longer needed.
 *
 * @returns a new CPU collection. If a CPU collection cannot be allocated, a
 *    null pointer is returned and an error code can be obtained via
 *    `_MCF_get_win32_error()`.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT
_MCF_cpu_collection*
_MCF_cpu_collection_new(void)
  __MCF_noexcept;

/** Allocates a copy of an existing CPU collection.
 *
 * The caller shall call `_MCF_cpu_collection_drop_ref()` to deallocate the CPU
 * collection when it is no longer needed.
 *
 * @param `src` shall point to the CPU collection to copy.
 * @returns an exact copy of `*src`. If a CPU collection cannot be allocated, a
 *    null pointer is returned. This function does not set the last error code.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT
_MCF_cpu_collection*
_MCF_cpu_collection_new_copy(const _MCF_cpu_collection* __src)
  __MCF_noexcept;

/** Gets the reference count of a CPU collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @returns the reference count of the CPU collection.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
int32_t
_MCF_cpu_collection_get_ref(const _MCF_cpu_collection* __coll)
  __MCF_noexcept;

/** Increments the reference count of a CPU collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @returns nothing.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE
void
_MCF_cpu_collection_add_ref(_MCF_cpu_collection* __coll)
  __MCF_noexcept;

/** Decrements the reference count of a CPU collection, and if it is reduced to
 * zero, deallocates the CPU collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @returns nothing.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT
void
_MCF_cpu_collection_drop_ref_nonnull(_MCF_cpu_collection* __coll)
  __MCF_noexcept;

/** Decrements the reference count of a CPU collection, and if it is reduced to
 * zero, deallocates the CPU collection.
 *
 * This function does nothing if `coll_opt` is a null pointer.
 *
 * @param `coll_opt` is an optional pointer to a CPU collection.
 * @returns nothing.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE
void
_MCF_cpu_collection_drop_ref(_MCF_cpu_collection* __coll_opt)
  __MCF_noexcept;

/** Gets the number of CPUs in a collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @returns the number of CPUs in the CPU collection.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_size(const _MCF_cpu_collection* __coll)
  __MCF_noexcept;

/** Gets the identifier of the CPU at `index` in the collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `index` shall be a valid index to a CPU in the collection.
 * @returns the identifier of the CPU at the specified index.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_id_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept;

/** Gets the processor group of the CPU at `index` in the collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `index` shall be a valid index to a CPU in the collection.
 * @returns the processor group which the CPU at the specified index belongs to.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_group_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept;

/** Gets the physical core identifier of the CPU at `index` in the collection.
 *
 * This value is calculated as `__group * 256 + __core_idx`.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `index` shall be a valid index to a CPU in the collection.
 * @returns the physical core identifier of the CPU at the specified index.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_core_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept;

/** Gets the efficiency class of the CPU at `index` in the collection.
 *
 * This value is a copy of `PROCESSOR_RELATIONSHIP::EfficiencyClass`. Typically,
 * it is 1 for Intel P-cores or ARM big cores, and 0 for Intel E-cores or ARM
 * little cores. On a homogeneous CPU, this value is always zero.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `index` shall be a valid index to a CPU in the collection.
 * @returns the efficiency class of the CPU at the specified index.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_efficiency_class_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept;

/** Gets the scheduling class of the CPU at `index` in the collection.
 *
 * This value is a copy of `SYSTEM_CPU_SET_INFORMATION::CpuSet.SchedulingClass`.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `index` shall be a valid index to a CPU in the collection.
 * @returns the scheduling class of the CPU at the specified index.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_scheduling_class_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept;

/** Gets the selection state of the CPU at `index` in the collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `index` shall be a valid index to a CPU in the collection.
 * @returns whether the CPU at the specified index is selected.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
bool
_MCF_cpu_collection_get_selection_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept;

/** Selects or deselects the CPU at `index` in the collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `index` shall be a valid index to a CPU in the collection.
 * @param `selected` specifies whether the CPU is to be selected. If this
 *    parameter is false, the CPU is deselected.
 * @returns whether the CPU at the specified index was selected before the call.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE
bool
_MCF_cpu_collection_set_selection_by_index(_MCF_cpu_collection* __coll, uint32_t __index,
                                           bool __selected)
  __MCF_noexcept;

/** Selects or deselects all CPUs in the collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `selected` specifies whether the CPUs are to be selected. If this
 *    parameter is false, the CPUs are deselected.
 * @returns nothing.
 * @since 2.5  */
__MCF_CPU_COLLECTION_INLINE
void
_MCF_cpu_collection_set_all_selections(_MCF_cpu_collection* __coll, bool __selected)
  __MCF_noexcept;

/** Finds a CPU by its identifier in the collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `id` is the identifier of the CPU to find.
 * @returns the index of the CPU in the collection, or `UINT32_MAX` if no CPU
 *    with the specified identifier is found.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_find(const _MCF_cpu_collection* __coll, uint32_t __id)
  __MCF_noexcept;

/** Gets the processor group of a CPU by its identifier in the collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `id` is the identifier of the CPU to find.
 * @returns the processor group of the CPU, or `UINT32_MAX` if no CPU with
 *    the specified identifier is found.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_group(const _MCF_cpu_collection* __coll, uint32_t __id)
  __MCF_noexcept;

/** Gets the physical core identifier of a CPU by its identifier in the
 * collection.
 *
 * This value is calculated as `__group * 256 + __core_idx`.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `id` is the identifier of the CPU to find.
 * @returns the physical core identifier of the CPU, or `UINT32_MAX` if no CPU
 *    with the specified identifier is found.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_core(const _MCF_cpu_collection* __coll, uint32_t __id)
  __MCF_noexcept;

/** Gets the efficiency class of a CPU by its identifier in the collection.
 *
 * This value is a copy of `PROCESSOR_RELATIONSHIP::EfficiencyClass`. Typically,
 * it is 1 for Intel P-cores or ARM big cores, and 0 for Intel E-cores or ARM
 * little cores. On a homogeneous CPU, this value is always zero.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `id` is the identifier of the CPU to find.
 * @returns the efficiency class of the CPU, or `UINT32_MAX` if no CPU with the
 *    specified identifier is found.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_efficiency_class(const _MCF_cpu_collection* __coll, uint32_t __id)
  __MCF_noexcept;

/** Gets the scheduling class of a CPU by its identifier in the collection.
 *
 * This value is a copy of `SYSTEM_CPU_SET_INFORMATION::CpuSet.SchedulingClass`.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `id` is the identifier of the CPU to find.
 * @returns the scheduling class of the CPU, or `UINT32_MAX` if no CPU with the
 *    specified identifier is found.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_scheduling_class(const _MCF_cpu_collection* __coll, uint32_t __id)
  __MCF_noexcept;

/** Gets the selection state of a CPU by its identifier in the collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `id` is the identifier of the CPU to find.
 * @returns whether the CPU with the specified identifier is selected.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT __MCF_FN_PURE
bool
_MCF_cpu_collection_get_selection(const _MCF_cpu_collection* __coll, uint32_t __id)
  __MCF_noexcept;

/** Selects or deselects a CPU by its identifier in the collection.
 *
 * @param `coll` shall point to a CPU collection.
 * @param `id` is the identifier of the CPU to find.
 * @param `selected` specifies whether the CPU is to be selected. If this
 *    parameter is false, the CPU is deselected.
 * @returns 1 if the CPU with the specified identifier was selected before the call,
 *    0 if it was unselected before the call, or -1 if no CPU with the specified
 *    identifier is found.
 * @since 2.5  */
__MCF_CPU_COLLECTION_IMPORT
int
_MCF_cpu_collection_set_selection(_MCF_cpu_collection* __coll, uint32_t __id, bool __selected)
  __MCF_noexcept;

/** Define inline functions after all declarations.
 *
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
uint32_t
_MCF_cpu_collection_get_core_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept
  {
    __MCF_ASSERT(__index < __coll->__size);
    return __coll->__data[__index].__group * 256U + __coll->__data[__index].__core_idx;
  }

__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_efficiency_class_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept
  {
    __MCF_ASSERT(__index < __coll->__size);
    return __coll->__data[__index].__effic_cls;
  }

__MCF_CPU_COLLECTION_INLINE __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_scheduling_class_by_index(const _MCF_cpu_collection* __coll, uint32_t __index)
  __MCF_noexcept
  {
    __MCF_ASSERT(__index < __coll->__size);
    return __coll->__data[__index].__sched_cls;
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
    bool __old_selected;
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
