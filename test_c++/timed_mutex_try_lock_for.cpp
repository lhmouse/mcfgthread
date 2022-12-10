/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/cxx11.hpp"
#include "../src/clock.h"
#include <assert.h>
#include <stdio.h>

#ifdef TEST_STD
#  include <mutex>
#  include <chrono>
#  include <thread>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

static NS::timed_mutex mutex;

int
main(void)
  {
    double now, delta;
    bool r;

    now = ::_MCF_perf_counter();
    r = mutex.try_lock_for(NS::chrono::milliseconds(1100));
    assert(r == true);
    delta = ::_MCF_perf_counter() - now;
    ::printf("delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    NS::thread(
     [&] {
       now = ::_MCF_perf_counter();
       r = mutex.try_lock_for(NS::chrono::milliseconds(1100));
       assert(r == false);
       delta = ::_MCF_perf_counter() - now;
       ::printf("delta = %.6f\n", delta);
       assert(delta >= 1100);
       assert(delta <= 1200);
     })
     .join();
  }
