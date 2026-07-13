/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cpu_collection.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

extern HMODULE __MCF_crt_ntdll;
extern void* __MCF_crt_GetSystemCpuSetInformation_opt;

int
main(void)
  {
    if(!__MCF_crt_GetSystemCpuSetInformation_opt)
      return 77;

    if(GetActiveProcessorGroupCount() != 1)
      return 77;

    // https://gitlab.winehq.org/wine/wine/-/wikis/Developer-FAQ#how-can-i-detect-wine
    bool is_on_wine = GetProcAddress(__MCF_crt_ntdll, "wine_get_version") != NULL;

    // use new API
    _MCF_cpu_collection* collx = _MCF_cpu_collection_new();
    assert(collx);

    fprintf(stderr, "number of CPUs: %u\n", _MCF_cpu_collection_get_size(collx));
    for(uint32_t k = 0;  k < _MCF_cpu_collection_get_size(collx);  k++)
      fprintf(stderr, "  CPU %u: ID %u, Core %u, Efficiency %u, Scheduling %u, Group %u\n", k,
          _MCF_cpu_collection_get_id_by_index(collx, k),
          _MCF_cpu_collection_get_core_by_index(collx, k),
          _MCF_cpu_collection_get_efficiency_class_by_index(collx, k),
          _MCF_cpu_collection_get_scheduling_class_by_index(collx, k),
          _MCF_cpu_collection_get_group_by_index(collx, k));

    // use old API
    __MCF_crt_GetSystemCpuSetInformation_opt = NULL;
    _MCF_cpu_collection* colly = _MCF_cpu_collection_new();
    assert(colly);

    fprintf(stderr, "number of CPUs: %u\n", _MCF_cpu_collection_get_size(colly));
    for(uint32_t k = 0;  k < _MCF_cpu_collection_get_size(colly);  k++)
      fprintf(stderr, "  CPU %u: ID %u, Core %u, Efficiency %u, Scheduling %u, Group %u\n", k,
          _MCF_cpu_collection_get_id_by_index(colly, k),
          _MCF_cpu_collection_get_core_by_index(colly, k),
          _MCF_cpu_collection_get_efficiency_class_by_index(colly, k),
          _MCF_cpu_collection_get_scheduling_class_by_index(colly, k),
          _MCF_cpu_collection_get_group_by_index(colly, k));

    // compare
    assert(_MCF_cpu_collection_get_size(collx) == _MCF_cpu_collection_get_size(colly));
    for(uint32_t k = 0;  k < _MCF_cpu_collection_get_size(collx);  k++) {
      assert(_MCF_cpu_collection_get_id_by_index(collx, k)
             == _MCF_cpu_collection_get_id_by_index(colly, k));

      if(!is_on_wine)
        assert(_MCF_cpu_collection_get_core_by_index(collx, k)
               == _MCF_cpu_collection_get_core_by_index(colly, k));

      assert(_MCF_cpu_collection_get_efficiency_class_by_index(collx, k)
             == _MCF_cpu_collection_get_efficiency_class_by_index(colly, k));
    }

    _MCF_cpu_collection_drop_ref(collx);
    _MCF_cpu_collection_drop_ref(colly);
  }
