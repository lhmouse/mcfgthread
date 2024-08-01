/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/gthr.h"
#include <assert.h>

int
main(void)
  {
#define NKEYS  1000U
    __gthread_key_t keys[NKEYS];
    for(size_t k = 0;  k != NKEYS;  ++k) {
      int r = __gthread_key_create(&keys[k], __MCF_nullptr);
      assert(r == 0);
      assert(keys[k]);
    }

#define NVALS  10000U
    for(size_t v = 0;  v != NVALS;  ++v) {
      for(size_t k = 0;  k != NKEYS;  ++k) {
        int r = __gthread_setspecific(keys[k], (void*) v);
        assert(r == 0);
      }

      for(size_t k = 0;  k != NKEYS;  ++k) {
        size_t p = (size_t) __gthread_getspecific(keys[k]);
        assert(p == v);
      }
    }
  }
