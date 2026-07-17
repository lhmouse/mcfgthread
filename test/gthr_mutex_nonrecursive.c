/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/gthr.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
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
    assert(r == EBUSY);

    r = __gthread_mutex_unlock(&mutex);
    assert(r == 0);

    r = __gthread_mutex_trylock(&mutex);
    assert(r == 0);
  }
