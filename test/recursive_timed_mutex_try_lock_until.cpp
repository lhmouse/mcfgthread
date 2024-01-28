/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/clock.h"
#include "../mcfgthread/thread.h"
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

static NS::recursive_timed_mutex mutex;

int
main(void)
  {
    double now, delta;
    bool r;

    ::_MCF_thread_set_priority(nullptr, ::_MCF_thread_priority_above_normal);

    // Round the time up.
    int64_t sleep_until = (int64_t) ::time(NULL) * 1000 + 2000;
    ::_MCF_sleep(&sleep_until);

    now = ::_MCF_perf_counter();
    r = mutex.try_lock_until(NS::chrono::system_clock::now() + NS::chrono::milliseconds(1100));
    assert(r == true);
    delta = ::_MCF_perf_counter() - now;
    ::printf("delta = %.6f\n", delta);
    assert(delta >= 0);
    assert(delta <= 100);

    NS::thread(
     [&] {
       now = ::_MCF_perf_counter();
       r = mutex.try_lock_until(NS::chrono::system_clock::now() + NS::chrono::milliseconds(1100));
       assert(r == false);
       delta = ::_MCF_perf_counter() - now;
       ::printf("delta = %.6f\n", delta);
       assert(delta >= 1100 - 20);
       assert(delta <= 1200);
     })
     .join();
  }
