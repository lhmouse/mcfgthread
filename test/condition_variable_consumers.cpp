/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <vector>

#ifdef TEST_STD
#  include <mutex>
#  include <condition_variable>
#  include <thread>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

constexpr std::size_t NTHREADS = 64U;
constexpr std::size_t NTICKS = 100000U;

static std::vector<NS::thread> threads(NTHREADS);
static NS::mutex mutex;
static NS::condition_variable cond_produced, cond_consumed;
static int value;
static std::vector<int> consumed(NTHREADS);

static
void
thread_proc(int& my_consumed)
  {
    for(;;) {
      // Wait for value.
      NS::unique_lock<NS::mutex> xlk(mutex);
      cond_produced.wait(xlk, []{ return value != 0;  });

      // Consume it.
      int value_got = value;
      //::fprintf(stderr, "thread %d got %d\n", ::__MCF_tid(), value_got);
      if(value_got > 0)
        value = 0;

      cond_consumed.notify_one();
      xlk.unlock();

      if(value_got < 0)
        break;

      // Accumulate it.
      my_consumed += value_got;
    }

    ::fprintf(stderr, "thread %d quitting\n", ::__MCF_tid());
  }

int
main(void)
  {
    for(std::size_t k = 0;  k < NTHREADS;  ++k)
      threads.at(k) = NS::thread(thread_proc, std::ref(consumed.at(k)));

    NS::this_thread::sleep_for(NS::chrono::milliseconds(500));
    NS::unique_lock<NS::mutex> xlk(mutex);

    for(std::size_t k = 0;  k < NTICKS;  ++k) {
      // Wait for consumption
      cond_consumed.wait(xlk, []{ return value == 0;  });

      // Produce one.
      value = 1;
      //::fprintf(stderr, "main set %d\n", value);

      cond_produced.notify_one();
    }

    cond_consumed.wait(xlk, []{ return value == 0;  });

    // Inform end of input
    value = -1;
    ::fprintf(stderr, "main set end of input\n");

    cond_produced.notify_all();
    xlk.unlock();

    // Wait and sum all values
    int total = 0;
    ::fprintf(stderr, "main waiting\n");

    for(std::size_t k = 0;  k < NTHREADS;  ++k) {
      threads.at(k).join();
      ::fprintf(stderr, "main wait finished: %d, consumed %d\n", (int) k, consumed.at(k));
      total += consumed.at(k);
    }

    assert(total == NTICKS);
  }
