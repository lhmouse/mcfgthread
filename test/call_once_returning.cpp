/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <vector>

#ifdef TEST_STD
#  include <mutex>
#  include <thread>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

constexpr std::size_t NTHREADS = 64U;
static std::vector<NS::thread> threads(NTHREADS);
static NS::once_flag once;
static ::_MCF_sem start = __MCF_SEM_INIT(0);
static int resource = 0;

static
void
once_do_it(int add)
  {
    /* Perform initialization.  */
    int old = resource;
    NS::this_thread::sleep_for(NS::chrono::milliseconds(200));
    resource = old + add;

    NS::this_thread::sleep_for(NS::chrono::milliseconds(100));
    ::fprintf(stderr, "thread %d done\n", ::__MCF_tid());
  }

static
void
thread_proc()
  {
    ::_MCF_sem_wait(&start, nullptr);

    NS::call_once(once, once_do_it, 1);
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

    assert(resource == 1);
  }
