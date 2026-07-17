/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/shared_mutex.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    int r;
    _MCF_shared_mutex mtx = { 0 };
    assert(mtx.__nshare == 0);
    assert(mtx.__nsleep == 0);

    for(unsigned i = 1;  i <= __MCF_SHARED_MUTEX_MAX_SHARE;  ++i) {
      r = _MCF_shared_mutex_lock_shared(&mtx, &(int64_t){ 0 });
      assert(r == 0);
      assert(mtx.__nshare == i);
      assert(mtx.__nsleep == 0);
    }

    r = _MCF_shared_mutex_lock_shared(&mtx, &(int64_t){ 0 });
    assert(r == -1);
    assert(mtx.__nshare == __MCF_SHARED_MUTEX_MAX_SHARE);
    assert(mtx.__nsleep == 0);
  }
