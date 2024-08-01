/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
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

    mutex.__locked = 1;
    mutex.__sp_nfail = 0;
    assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == -1);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_mask == 1);

    mutex.__locked = 1;
    mutex.__sp_nfail = 0;
    assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == -1);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_mask == 3);

    mutex.__locked = 1;
    mutex.__sp_nfail = 0;
    assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == -1);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_mask == 7);

    mutex.__locked = 1;
    mutex.__sp_nfail = 0;
    assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == -1);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_mask == 15);

    mutex.__locked = 1;
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(mutex.__sp_mask == 14);

    mutex.__locked = 1;
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(mutex.__sp_mask == 12);

    mutex.__locked = 1;
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(mutex.__sp_mask == 8);

    mutex.__locked = 1;
    mutex.__sp_nfail = 0;
    assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == -1);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_mask == 9);

    mutex.__locked = 1;
    mutex.__sp_nfail = 0;
    assert(_MCF_mutex_lock(&mutex, (const int64_t[]){ -100 }) == -1);
    assert(mutex.__locked == 1);
    assert(mutex.__sp_mask == 11);

    mutex.__locked = 1;
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(mutex.__sp_mask == 10);

    mutex.__locked = 1;
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(mutex.__sp_mask == 8);

    mutex.__locked = 1;
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(mutex.__sp_mask == 0);

    mutex.__locked = 1;
    _MCF_mutex_unlock(&mutex);
    assert(mutex.__locked == 0);
    assert(mutex.__sp_mask == 0);
  }
