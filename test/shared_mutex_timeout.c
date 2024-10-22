/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/shared_mutex.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static _MCF_shared_mutex mutex;

int
main(void)
  {
    double now, delta;
    int r;

    _MCF_thread_set_priority(__MCF_nullptr, _MCF_thread_priority_above_normal);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_shared(&mutex, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_exclusive(&mutex, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_exclusive(&mutex, (const int64_t[]){ -1116 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_shared(&mutex, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_shared(&mutex, (const int64_t[]){ -1116 });  /* relative  */
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    _MCF_shared_mutex_unlock(&mutex);
    _MCF_shared_mutex_unlock(&mutex);
    _MCF_shared_mutex_unlock(&mutex);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_exclusive(&mutex, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_exclusive(&mutex, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_exclusive(&mutex, (const int64_t[]){ -1116 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_shared(&mutex, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_shared_mutex_lock_shared(&mutex, (const int64_t[]){ -1116 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);
  }
