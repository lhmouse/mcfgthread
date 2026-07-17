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
#  include <mutex>
#  include <condition_variable>
#  include <chrono>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

static NS::mutex mutex;
static NS::condition_variable cond;

int
main(void)
  {
    double now, delta;
    NS::cv_status r;

    ::_MCF_thread_set_priority(nullptr, ::_MCF_thread_priority_above_normal);
    NS::unique_lock<NS::mutex> xlk(mutex);

    // Round the time up.
    int64_t sleep_until = (int64_t) ::time(nullptr) * 1000 + 2000;
    ::_MCF_sleep_noninterruptible(&sleep_until);

    now = ::_MCF_perf_counter();
    r = cond.wait_until(xlk, NS::chrono::system_clock::now() + NS::chrono::milliseconds(1116));  // relaxed
    assert(r == NS::cv_status::timeout);
    delta = ::_MCF_perf_counter() - now;
    assert(delta >= 1100);
    assert(delta <= 1200);

    assert(xlk);
    try
     { assert(xlk.try_lock() == false);  }
    catch(std::system_error& e)
     { assert(e.code() == std::errc::resource_deadlock_would_occur);  }
  }
