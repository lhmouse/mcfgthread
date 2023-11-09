/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/cond.h"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#include <assert.h>
#include <stdio.h>

static _MCF_cond cond;

int
main(void)
  {
    double now, delta;
    int r;

    _MCF_thread_set_priority(NULL, _MCF_thread_priority_above_normal);

    now = _MCF_perf_counter();
    r = _MCF_cond_wait(&cond, NULL, NULL, 0, (const int64_t[]){ (int64_t) _MCF_hires_utc_now() + 1100 });  /* absolute  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);

    now = _MCF_perf_counter();
    r = _MCF_cond_wait(&cond, NULL, NULL, 0, (const int64_t[]){ -1100 });  /* relative  */
    assert(r == -1);
    delta = _MCF_perf_counter() - now;
    printf("delta = %.6f\n", delta);
    assert(delta >= 1100 - 40);
    assert(delta <= 1200);
  }
