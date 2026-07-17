/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"
#undef NDEBUG
#include <assert.h>

int
main(void)
  {
#define NKEYS  1000U
    tss_t keys[NKEYS];
    for(size_t k = 0;  k != NKEYS;  ++k) {
      int r = tss_create(&keys[k], NULL);
      assert(r == thrd_success);
      assert(keys[k]);
    }

#define NVALS  10000U
    for(size_t v = 0;  v != NVALS;  ++v) {
      for(size_t k = 0;  k != NKEYS;  ++k) {
        int r = tss_set(keys[k], (void*) v);
        assert(r == thrd_success);
      }

      for(size_t k = 0;  k != NKEYS;  ++k) {
        size_t p = (size_t) tss_get(keys[k]);
        assert(p == v);
      }
    }
  }
