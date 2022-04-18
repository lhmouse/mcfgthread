/* This file is part of MCF gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/thread.h"
#include <assert.h>
#include <windows.h>

int
main(void)
  {
#define NKEYS  1000U
    _MCF_tls_key* keys[NKEYS];
    for(size_t k = 0;  k != NKEYS;  ++k) {
      keys[k] = _MCF_tls_key_new(NULL);
      assert(keys[k]);
    }

#define NVALS  10000U
    for(size_t v = 0;  v != NVALS;  ++v) {
      for(size_t k = 0;  k != NKEYS;  ++k) {
        int r = _MCF_tls_set(keys[k], (void*) v);
        assert(r == 0);
      }

      for(size_t k = 0;  k != NKEYS;  ++k) {
        size_t p = (size_t) _MCF_tls_get(keys[k]);
        assert(p == v);
      }
    }
  }
