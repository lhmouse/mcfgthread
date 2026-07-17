/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

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

    for(uint32_t k = 0;  k < _MCF_cpu_collection_get_size(coll);  k++) {
      _MCF_cpu_collection_set_all_selections(coll, false);
      _MCF_cpu_collection_set_selection_by_index(coll, k, true);
      assert(_MCF_thread_set_affinity(_MCF_thread_self(), coll) == 0);
    }

    for(uint32_t k = 0;  k < _MCF_cpu_collection_get_size(coll);  k++) {
      _MCF_cpu_collection_set_all_selections(coll, false);
      if(_MCF_cpu_collection_get_size(coll) > 2)
        _MCF_cpu_collection_set_selection_by_index(coll, 2, true);
      _MCF_cpu_collection_set_selection_by_index(coll, k, true);
      assert(_MCF_thread_set_affinity(_MCF_thread_self(), coll) == 0);
    }

    _MCF_cpu_collection_drop_ref(coll);
  }
