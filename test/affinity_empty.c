/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    _MCF_cpu_collection* coll = _MCF_cpu_collection_new();
    assert(coll);

    assert(_MCF_thread_get_affinity(NULL, coll) == 0);

    fprintf(stderr, "current affinity: %u\n", _MCF_cpu_collection_get_size(coll));
    for(uint32_t k = 0;  k < _MCF_cpu_collection_get_size(coll);  k++)
      fprintf(stderr, "  CPU %u: ID %u, Group %u\n", k,
          _MCF_cpu_collection_get_id_by_index(coll, k),
          _MCF_cpu_collection_get_group_by_index(coll, k));

    _MCF_cpu_collection_set_all_selections(coll, false);
    assert(_MCF_thread_set_affinity(_MCF_thread_self(), coll) == -1);

    _MCF_cpu_collection_drop_ref(coll);
  }
