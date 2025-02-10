/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/shared_mutex.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    int r;
    _MCF_shared_mutex mtx = __MCF_0_INIT;
    assert(mtx.__nshare == 0);
    assert(mtx.__nsleep == 0);

    for(unsigned i = 1;  i <= __MCF_SHARED_MUTEX_MAX_SHARE;  ++i) {
      r = _MCF_shared_mutex_lock_shared(&mtx, &(int64_t) { 0 });
      assert(r == 0);
      assert(mtx.__nshare == i);
      assert(mtx.__nsleep == 0);
    }

    r = _MCF_shared_mutex_lock_shared(&mtx, &(int64_t) { 0 });
    assert(r == -1);
    assert(mtx.__nshare == __MCF_SHARED_MUTEX_MAX_SHARE);
    assert(mtx.__nsleep == 0);
  }
