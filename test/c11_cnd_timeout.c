/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static mtx_t mutex;
static cnd_t cond;

int
main(void)
  {
    double now, delta;
    struct timespec timeout;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    r = mtx_init(&mutex, mtx_timed);
    assert(r == thrd_success);
    r = cnd_init(&cond);
    assert(r == thrd_success);
    r = mtx_trylock(&mutex);
    assert(r == thrd_success);

    /* Round the time up. The `while` loop is necessary to work around a bug
     * in Wine, which physical Windows doesn't have.
     * See https://bugs.winehq.org/show_bug.cgi?id=57035  */
    int64_t sleep_until = (int64_t) time(__MCF_nullptr) * 1000 + 2000;
    while(time(__MCF_nullptr) * 1000 < sleep_until)
      _MCF_sleep(&sleep_until);

    now = _MCF_perf_counter();
    timeout.tv_sec = time(__MCF_nullptr) + 1;
    timeout.tv_nsec = 100000000;
    r = cnd_timedwait(&cond, &mutex, &timeout);
    assert(r == thrd_timedout);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);

    r = mtx_trylock(&mutex);
    assert(r == thrd_error);
  }
