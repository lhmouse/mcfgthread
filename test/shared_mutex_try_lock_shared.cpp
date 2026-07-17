/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <vector>
#include <atomic>

#ifdef TEST_STD
#  include <shared_mutex>
#  include <mutex>
#  include <thread>
#  include <chrono>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

constexpr std::size_t NTHREADS = 64U;
static std::vector<NS::thread> threads(NTHREADS);
static NS::shared_mutex mutex;
static ::_MCF_sem start = __MCF_SEM_INIT(0);
static std::atomic<int> resource(0);

static
void
thread_proc()
  {
    ::_MCF_sem_wait(&start, nullptr);

    for(;;) {
      NS::shared_lock<NS::shared_mutex> xlk(mutex, NS::try_to_lock);
      if(xlk) {
        // Add a resource.
        int old = resource.load();
        NS::this_thread::sleep_for(NS::chrono::milliseconds(10));
        resource.store(old + 1);
        break;
      }
      else
        NS::this_thread::sleep_for(NS::chrono::milliseconds(10));
    }

    ::fprintf(stderr, "thread %d quitting\n", ::__MCF_tid());
  }

int
main(void)
  {
    for(auto& thr : threads)
      thr = NS::thread(thread_proc);

    ::fprintf(stderr, "main waiting\n");
    ::_MCF_sem_signal_some(&start, NTHREADS);

    for(auto& thr : threads)
      thr.join();

    assert(resource.load() < (int) NTHREADS);
  }
