/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/cxx11.hpp"
#include "../src/clock.h"
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
    NS::unique_lock<NS::mutex> lock(mutex);

    now = ::_MCF_perf_counter();
    r = cond.wait_for(lock, NS::chrono::milliseconds(1100));
    assert(r == NS::cv_status::timeout);
    delta = ::_MCF_perf_counter() - now;
    assert(delta >= 1000);
    assert(delta <= 1200);

    assert(lock);
    try
     { assert(lock.try_lock() == false);  }
    catch(std::system_error& e)
     { assert(e.code() == std::errc::resource_deadlock_would_occur);  }
  }
