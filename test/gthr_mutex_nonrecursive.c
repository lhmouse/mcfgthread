/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/gthr.h"
#include "../mcfgthread/clock.h"
#include <assert.h>
#include <stdio.h>

static __gthread_mutex_t mutex = __GTHREAD_MUTEX_INIT;

int
main(void)
  {
    int r;

    r = __gthread_mutex_trylock(&mutex);
    assert(r == 0);

    r = __gthread_mutex_trylock(&mutex);
    assert(r == -1);

    r = __gthread_mutex_unlock(&mutex);
    assert(r == 0);

    r = __gthread_mutex_trylock(&mutex);
    assert(r == 0);
  }
