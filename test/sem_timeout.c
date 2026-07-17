/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/sem.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static _MCF_sem sem = __MCF_SEM_INIT(1);

int
main(void)
  {
    double now, delta;
    int r;

    _MCF_thread_set_priority(NULL, _MCF_thread_priority_above_normal);

    now = _MCF_perf_counter();
    r = _MCF_sem_wait(&sem, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == 0);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    now = _MCF_perf_counter();
    r = _MCF_sem_wait(&sem, &(int64_t){ (int64_t) _MCF_hires_utc_now() + 1116 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_sem_wait(&sem, &(int64_t){ -1116 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);
  }
