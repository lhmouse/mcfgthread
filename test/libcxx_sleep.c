/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/libcxx.h"
#include "../mcfgthread/clock.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    _MCF_thread_set_priority(NULL, _MCF_thread_priority_above_normal);

    double now = _MCF_perf_counter();
    __MCF_libcxx_thread_sleep_for_ns_count(1300000000);
    double delta = _MCF_perf_counter() - now;
    fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1300);
    assert(delta <= 1400);
  }
