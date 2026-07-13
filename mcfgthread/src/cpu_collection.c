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
      SYSTEM_CPU_SET_INFORMATION* info = __builtin_alloca(1024);
      DWORD info_cb = 1024;
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

      SYSTEM_CPU_SET_INFORMATION* end_of_info = (void*) ((char*) info + info_cb);
      while(info != end_of_info) {
        if(info->Type == CpuSetInformation) {
          /* The system seems to return CPU Set identifiers in increasing order,
           * but that's undocumented, so keep them sorted, with Insertion Sort.  */
          uint32_t pos1 = coll->__size, pos2 = coll->__size;
          while((pos1 != 0) && (coll->__data[-- pos1].__id > info->CpuSet.Id))
            coll->__data[pos2 --] = coll->__data[pos1];

          /* Add CPU at the upper bound.  */
          coll->__size ++;
          coll->__data[pos2].__id = info->CpuSet.Id;
          coll->__data[pos2].__group = info->CpuSet.Group;
          coll->__data[pos2].__core_idx = info->CpuSet.CoreIndex;
          coll->__data[pos2].__effic_cls = info->CpuSet.EfficiencyClass;
          coll->__data[pos2].__sched_cls = info->CpuSet.SchedulingClass;
        }

        info = (SYSTEM_CPU_SET_INFORMATION*) ((char*) info + info->Size);
      }
    }
    else {
      /* When CPU Set APIs are not available, a process is limited to a single
       * processor group.  */
      SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* info = __builtin_alloca(2560);
      DWORD info_cb = 2560;
      if(!GetLogicalProcessorInformationEx(RelationProcessorCore, info, &info_cb)) {
        if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
          return nullptr;

        info = __builtin_alloca(info_cb);
        if(!GetLogicalProcessorInformationEx(RelationProcessorCore, info, &info_cb))
          return nullptr;
      }

      DWORD_PTR proc_mask, sys_mask;
      if(!GetProcessAffinityMask(NtCurrentProcess(), &proc_mask, &sys_mask))
        return nullptr;

      PROCESSOR_NUMBER proc_num;
      GetCurrentProcessorNumberEx(&proc_num);

      coll = __MCF_malloc_0(offsetof(_MCF_cpu_collection, __data)
                            + __MCF_PTR_BITS * sizeof(__MCF_cpu_element));
      if(!coll)
        return __MCF_win32_error_p(ERROR_NOT_ENOUGH_MEMORY, nullptr);

      DWORD_PTR mask_reg = proc_mask;
      while(mask_reg != 0) {
        /* Each active logical processor is indicated by a bit in `proc_mask`
         * in the current process group. This ensures that CPU identifiers are
         * sorted.  */
        DWORD bit_index;
        __MCF_64_32(_BitScanForward64, _BitScanForward) (&bit_index, mask_reg);

        /* Add CPU into the end. CPU identifiers start from 256 like CPU Set APIs.  */
        uint32_t pos = coll->__size ++;
        coll->__data[pos].__id = 256 + bit_index;
        coll->__data[pos].__group = proc_num.Group;
        coll->__data[pos].__core_idx = (BYTE) bit_index;

        mask_reg = mask_reg & (mask_reg - 1);
      }

      SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* end_of_info = (void*) ((char*) info + info_cb);
      while(info != end_of_info) {
        if(info->Relationship == RelationProcessorCore) {
          /* Microsoft documentation says that 'if the `PROCESSOR_RELATIONSHIP`
           * structure represents a processor core, the `GroupCount` member is
           * always 1.' In this code path, a process can't straddle multiple
           * process groups, so only cores of the current group are returned.  */
          if(info->Processor.GroupMask[0].Group == proc_num.Group) {
            DWORD core_index;
            __MCF_64_32(_BitScanForward64, _BitScanForward) (&core_index,
                                                             info->Processor.GroupMask[0].Mask);

            for(uint32_t k = 0;  k < coll->__size;  ++k)
              if(info->Processor.GroupMask[0].Mask & ((DWORD_PTR) 1 << (coll->__data[k].__id - 256))) {
                coll->__data[k].__core_idx = (BYTE) core_index;
                coll->__data[k].__effic_cls = info->Processor.EfficiencyClass;
              }
          }
        }

        info = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*) ((char*) info + info->Size);
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

    /* Return the index in `__data`.  */
    return (uint32_t) (elem - coll->__data);
  }

__MCF_DLLEXPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_group(const _MCF_cpu_collection* coll, uint32_t id)
  {
    const __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return UINT32_MAX;

    /* Return the processor group index.  */
    return elem->__group;
  }

__MCF_DLLEXPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_core(const _MCF_cpu_collection* coll, uint32_t id)
  {
    const __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return UINT32_MAX;

    /* Compose the physical core identifier.  */
    return elem->__group * 256U + elem->__core_idx;
  }

__MCF_DLLEXPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_efficiency_class(const _MCF_cpu_collection* coll, uint32_t id)
  {
    const __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return UINT32_MAX;

    /* Return the efficiency class. The lower the value, the more efficient
     * the core. Shouldn't this be called 'performance class'? I don't know.  */
    return elem->__effic_cls;
  }

__MCF_DLLEXPORT __MCF_FN_PURE
uint32_t
_MCF_cpu_collection_get_scheduling_class(const _MCF_cpu_collection* coll, uint32_t id)
  {
    const __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return UINT32_MAX;

    /* Return the scheduling class. This value is > 0 for a P-core and = 0 for
     * an E-core.  */
    return elem->__sched_cls;
  }

__MCF_DLLEXPORT __MCF_FN_PURE
bool
_MCF_cpu_collection_get_selection(const _MCF_cpu_collection* coll, uint32_t id)
  {
    const __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return false;

    /* Return the current state.  */
    return elem->__selected;
  }

__MCF_DLLEXPORT
int
_MCF_cpu_collection_set_selection(_MCF_cpu_collection* coll, uint32_t id, bool selected)
  {
    __MCF_cpu_element* elem = do_find_cpu_opt(coll, id);
    if(!elem)
      return -1;

    /* Return the previous state.  */
    int old_selected = elem->__selected;
    elem->__selected = selected;
    return old_selected;
  }
