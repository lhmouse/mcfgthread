/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/clock.h"
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

    mutex.__rc_mtx[0].__owner[0] = _MCF_thread_self_tid();
    r = mtx_unlock(&mutex);
    assert(r == thrd_success);

    r = mtx_trylock(&mutex);
    assert(r == thrd_success);
  }
