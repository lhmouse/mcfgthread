/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/mutex.h"
#include <assert.h>
#include <stdio.h>

static _MCF_mutex mutex;

int
main(void)
  {
    assert(mutex.__locked == 0);
    assert(mutex.__sp_mask == 0);
    assert(mutex.__sp_nfail == 0);

    assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == 0);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_nfail == 0);

    for(size_t count = 1;  count <= 15;  ++count) {
      fprintf(stderr, "try failing: %d\n", (int) count);
      assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == -1);
      assert(mutex.__locked == 1);
      assert(mutex.__sp_nfail == count);
    }

    fprintf(stderr, "try failing: final\n");
    assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == -1);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_nfail == 15);

    for(size_t count = 15;  count >= 1;  --count) {
      fprintf(stderr, "try succeeding: %d\n", (int) count);
      _MCF_mutex_unlock(&mutex);
      assert(mutex.__locked == 0);
      assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 >> count }) == 0);
      assert(mutex.__sp_nfail == count - 1);
    }

    fprintf(stderr, "try succeeding: final\n");
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == 0);
    assert(mutex.__sp_nfail == 0);
  }
