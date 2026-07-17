/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#ifdef TEST_STD
#  include <chrono>
#  include <thread>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

int
main(void)
  {
    double now, delta;

    ::_MCF_thread_set_priority(nullptr, ::_MCF_thread_priority_above_normal);

    // Round the time up.
    int64_t sleep_until = (int64_t) ::time(nullptr) * 1000 + 2000;
    ::_MCF_sleep_noninterruptible(&sleep_until);

    now = ::_MCF_perf_counter();
    NS::this_thread::sleep_until(NS::chrono::system_clock::now() + NS::chrono::milliseconds(1116));  // relaxed
    delta = ::_MCF_perf_counter() - now;
    ::fprintf(stderr, "delta = %.6f\n", delta);
    assert(delta >= 1100);
    assert(delta <= 1200);
  }
