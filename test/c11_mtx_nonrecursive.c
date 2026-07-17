/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static mtx_t mutex;

int
main(void)
  {
    int r;

    r = mtx_init(&mutex, mtx_plain);
    assert(r == thrd_success);

    r = mtx_trylock(&mutex);
    assert(r == thrd_success);

    mutex.__rc_mtx[0].__owner[0] = 42;  /* don't expose the deadlock  */
    r = mtx_trylock(&mutex);
    assert(r == thrd_busy);

    mutex.__rc_mtx[0].__owner[0] = __MCF_tid();
    r = mtx_unlock(&mutex);
    assert(r == thrd_success);

    r = mtx_trylock(&mutex);
    assert(r == thrd_success);
  }
