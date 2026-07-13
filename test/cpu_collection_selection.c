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

int
main(void)
  {
    _MCF_cpu_collection* coll = _MCF_cpu_collection_new();
    assert(coll);

    fprintf(stderr, "number of CPUs: %u\n", _MCF_cpu_collection_get_size(coll));
    for(uint32_t k = 0;  k < _MCF_cpu_collection_get_size(coll);  k++)
      fprintf(stderr, "  CPU %u: ID %u, Core %u, Efficiency %u, Scheduling %u, Group %u\n", k,
          _MCF_cpu_collection_get_id_by_index(coll, k),
          _MCF_cpu_collection_get_core_by_index(coll, k),
          _MCF_cpu_collection_get_efficiency_class_by_index(coll, k),
          _MCF_cpu_collection_get_scheduling_class_by_index(coll, k),
          _MCF_cpu_collection_get_group_by_index(coll, k));

    uint32_t first_cpu_id = _MCF_cpu_collection_get_id_by_index(coll, 0);
    assert(first_cpu_id >= 256);
    assert(_MCF_cpu_collection_get_selection(coll, first_cpu_id) == 0);

    assert(_MCF_cpu_collection_set_selection(coll, first_cpu_id, 1) == 0);
    assert(_MCF_cpu_collection_get_selection(coll, first_cpu_id) == 1);
    assert(_MCF_cpu_collection_set_selection(coll, first_cpu_id, 1) == 1);
    assert(_MCF_cpu_collection_get_selection(coll, first_cpu_id) == 1);
    assert(_MCF_cpu_collection_set_selection(coll, first_cpu_id, 0) == 1);
    assert(_MCF_cpu_collection_get_selection(coll, first_cpu_id) == 0);
    assert(_MCF_cpu_collection_set_selection(coll, first_cpu_id, 0) == 0);
    assert(_MCF_cpu_collection_get_selection(coll, first_cpu_id) == 0);

    _MCF_cpu_collection_set_all_selections(coll, 1);
    assert(_MCF_cpu_collection_get_selection(coll, first_cpu_id) == 1);
    _MCF_cpu_collection_set_all_selections(coll, 0);
    assert(_MCF_cpu_collection_get_selection(coll, first_cpu_id) == 0);

    assert(_MCF_cpu_collection_get_selection(coll, 0xDEADBEEF) == 0);
    assert(_MCF_cpu_collection_set_selection(coll, 0xDEADBEEF, 1) == -1);
    assert(_MCF_cpu_collection_set_selection(coll, 0xDEADBEEF, 0) == -1);

    _MCF_cpu_collection_drop_ref(coll);
  }
