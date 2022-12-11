/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

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
