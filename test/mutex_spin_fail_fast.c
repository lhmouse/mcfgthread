/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCF_EXPAND_INLINE_DEFINITIONS
#  define __MCF_EXPAND_INLINE_DEFINITIONS 1
#endif
#include "../mcfgthread/mutex.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_mutex mutex;

int
main(void)
  {
    assert(mutex.__locked == 0);
    assert(mutex.__sp_mask == 0);
    assert(mutex.__sp_nfail == 0);

    assert(_MCF_mutex_lock(&mutex, &(int64_t){ -100 }) == 0);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_nfail == 0);

    for(int count = 1;  count <= 15;  ++count) {
      fprintf(stderr, "try failing: %d\n", count);
      assert(_MCF_mutex_lock(&mutex, &(int64_t){ -100 }) == -1);
      assert(mutex.__locked == 1);
      assert(mutex.__sp_nfail == count);
    }

    fprintf(stderr, "try failing: final\n");
    assert(_MCF_mutex_lock(&mutex, &(int64_t){ -100 }) == -1);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_nfail == 15);

    for(int count = 15;  count >= 1;  --count) {
      fprintf(stderr, "try succeeding: %d\n", count);
      _MCF_mutex_unlock(&mutex);
      assert(mutex.__locked == 0);
      assert(_MCF_mutex_lock(&mutex, &(int64_t){ -100 }) == 0);
      assert(mutex.__sp_nfail == count - 1);
    }

    fprintf(stderr, "try succeeding: final\n");
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(_MCF_mutex_lock(&mutex, &(int64_t){ -100 }) == 0);
    assert(mutex.__sp_nfail == 0);
  }
