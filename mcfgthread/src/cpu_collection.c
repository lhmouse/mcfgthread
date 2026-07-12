/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_CPU_COLLECTION_IMPORT  __MCF_DLLEXPORT
#define __MCF_CPU_COLLECTION_INLINE  __MCF_DLLEXPORT
#include "../cpu_collection.h"
#include "../atomic.h"
#include "xglobals.h"

__MCF_DLLEXPORT
_MCF_cpu_collection*
_MCF_cpu_collection_new(void)
  {
    _MCF_cpu_collection* coll;
    if(__MCF_crt_GetSystemCpuSetInformation_opt) {
      /* Use CPU Set APIs if they are available. These work reliably in a 32-bit
       * process on a 64-bit system.  */
      SYSTEM_CPU_SET_INFORMATION* info = __builtin_alloca(2048);
      DWORD info_cb = 2048;
      if(!__MCF_crt_GetSystemCpuSetInformation_opt(info, info_cb, &info_cb, NULL, 0)) {
        if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
          return nullptr;

        info = __builtin_alloca(info_cb);
        if(!__MCF_crt_GetSystemCpuSetInformation_opt(info, info_cb, &info_cb, NULL, 0))
          return nullptr;
      }

      coll = __MCF_malloc_0(offsetof(_MCF_cpu_collection, __data)
                   + info_cb / RTL_SIZEOF_THROUGH_FIELD(SYSTEM_CPU_SET_INFORMATION, CpuSet)
                     * sizeof(__MCF_cpu_element));
      if(!coll)
        return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, nullptr);

      /* Each active logical processor is indicated by a variable-size structure
       * of type `SYSTEM_CPU_SET_INFORMATION`.  */
      SYSTEM_CPU_SET_INFORMATION* end_of_info = (void*) ((char*) info + info_cb);
      while(info != end_of_info) {
        if(info->Type == CpuSetInformation) {
          /* The system seems to return CPU Set identifiers in increasing order,
           * but that's undocumented, so keep them sorted, with Insertion Sort.  */
          uint32_t pos1 = coll->__size, pos2 = coll->__size;
          while((pos1 != 0) && (coll->__data[-- pos1].__id > info->CpuSet.LogicalProcessorIndex))
            coll->__data[pos2 --] = coll->__data[pos1];

          /* Add CPU at the upper bound.  */
          coll->__size ++;
          coll->__data[pos2].__id = info->CpuSet.LogicalProcessorIndex;
          coll->__data[pos2].__group = info->CpuSet.Group;
        }

        info = (SYSTEM_CPU_SET_INFORMATION*) ((char*) info + info->Size);
      }
    }
    else {
      /* When CPU Set APIs are not available, a process is limited to a single
       * processor group.  */
      DWORD_PTR proc_mask, sys_mask;
      if(!GetProcessAffinityMask(NtCurrentProcess(), &proc_mask, &sys_mask))
        return nullptr;

      coll = __MCF_malloc_0(offsetof(_MCF_cpu_collection, __data)
                            + __MCF_PTR_BITS * sizeof(__MCF_cpu_element));
      if(!coll)
        return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, nullptr);

      /* Get the current processor group index from somewhere else.  */
      PROCESSOR_NUMBER proc_number;
      GetCurrentProcessorNumberEx(&proc_number);

      /* Each active logical processor is indicated by a bit in `proc_mask` in
       * the current process group. This ensures that CPU identifiers are sorted,
       * and also start from 256 like CPU Set APIs.  */
      uint32_t bit_index = 0;
      uintptr_t mask_reg = proc_mask;
      while(mask_reg != 0) {
        if(mask_reg & 1) {
          /* Add CPU into the end.  */
          uint32_t pos = coll->__size ++;
          coll->__data[pos].__id = 256 + bit_index;
          coll->__data[pos].__group = proc_number.Group;
        }

        bit_index ++;
        mask_reg >>= 1;
      }
    }

    /* Initialize the collection.  */
    _MCF_atomic_store_32_rlx(coll->__nref, 1);
    __MCF_ASSERT(coll->__size > 0);
    return coll;
  }

__MCF_DLLEXPORT
_MCF_cpu_collection*
_MCF_cpu_collection_new_copy(const _MCF_cpu_collection* src)
  {
    _MCF_cpu_collection* coll = __MCF_malloc_copy(src,
                                      offsetof(_MCF_cpu_collection, __data)
                                      + src->__size * sizeof(__MCF_cpu_element));
    if(!coll)
      return nullptr;

    /* Initialize the collection.  */
    _MCF_atomic_store_32_rlx(coll->__nref, 1);
    __MCF_ASSERT(coll->__size > 0);
    return coll;
  }

__MCF_DLLEXPORT
void
_MCF_cpu_collection_drop_ref_nonnull(_MCF_cpu_collection* coll)
  {
    int32_t old_ref = _MCF_atomic_xsub_32_arl(coll->__nref, 1);
    __MCF_ASSERT(old_ref > 0);
    if(old_ref != 1)
      return;

    /* Deallocate all associated resources.  */
    __MCF_mfree_nonnull(coll);
  }

static inline
__MCF_cpu_element*
do_find_cpu_opt(const _MCF_cpu_collection* coll, uint32_t id)
  {
    __MCF_ASSERT(coll);

    /* The CPUs have been sorted by `_MCF_cpu_collection_new()`, so we can do a
     * binary search here.  */
    uint32_t bpos = 0;
    uint32_t epos = coll->__size;

    for(;;) {
      if(bpos == epos)
        return nullptr;

      uint32_t mpos = bpos + (epos - bpos) / 2;
      const __MCF_cpu_element* elem = coll->__data + mpos;
      if(id == elem->__id)
        return (void*) elem;

      if(id < elem->__id)
        epos = mpos;
      else
        bpos = mpos + 1;
    }
  }

__MCF_DLLEXPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_find(const _MCF_cpu_collection* coll, uint32_t id)
  {
    const __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return UINT32_MAX;

    /* Returns the index in `__data`.  */
    return (uint32_t) (elem - coll->__data);
  }

__MCF_DLLEXPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_group(const _MCF_cpu_collection* coll, uint32_t id)
  {
    const __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return UINT32_MAX;

    /* Returns the processor group index.  */
    return elem->__group;
  }

__MCF_DLLEXPORT __MCF_FN_PURE
bool
_MCF_cpu_collection_get_selection(const _MCF_cpu_collection* coll, uint32_t id)
  {
    const __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return false;

    /* Returns the current state.  */
    return elem->__selected;
  }

__MCF_DLLEXPORT
int
_MCF_cpu_collection_set_selection(_MCF_cpu_collection* coll, uint32_t id, bool selected)
  {
    __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return -1;

    /* Returns the previous state.  */
    int old_selected = elem->__selected;
    elem->__selected = selected;
    return old_selected;
  }
