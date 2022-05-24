/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/c11.h"
#include "../src/clock.h"
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

    mutex.__owner = 42;  /* don't expose the deadlock  */
    r = mtx_trylock(&mutex);
    assert(r == thrd_busy);

    mutex.__owner = _MCF_thread_self_tid();
    r = mtx_unlock(&mutex);
    assert(r == thrd_success);

    r = mtx_trylock(&mutex);
    assert(r == thrd_success);
  }
