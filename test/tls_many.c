/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include <assert.h>

int
main(void)
  {
#define NKEYS  1000U
#define NVALS  10000U
    _MCF_tls_key* keys[NKEYS];

    for(size_t k = 0;  k != NKEYS;  ++k) {
      keys[k] = _MCF_tls_key_new(__MCF_nullptr);
      assert(keys[k]);
      assert(_MCF_tls_key_get_destructor(keys[k]) == __MCF_nullptr);
    }

    for(size_t k = 0;  k != NKEYS;  ++k) {
      void* p = _MCF_tls_get(keys[k]);
      assert(p == __MCF_nullptr);

      for(size_t v = 0;  v != NVALS;  ++v) {
        int r = _MCF_tls_set(keys[k], (void*) (v + k));
        assert(r == 0);

        p = _MCF_tls_get(keys[k]);
        assert(p == (void*) (v + k));
      }
    }

    for(size_t v = 0;  v != NVALS;  ++v) {
      for(size_t k = 0;  k != NKEYS;  ++k) {
        int r = _MCF_tls_set(keys[k], (void*) (v + k));
        assert(r == 0);
      }

      for(size_t k = 0;  k != NKEYS;  ++k) {
        void* p = _MCF_tls_get(keys[k]);
        assert(p == (void*) (v + k));
      }
    }
  }
