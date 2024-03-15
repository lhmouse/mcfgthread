/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/sem.h"
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
static _MCF_once once;
static ::_MCF_sem start = __MCF_SEM_INIT(NTHREADS);
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
  }

static
void
thread_proc()
  {
    ::_MCF_sem_wait(&start, nullptr);

    ::__MCF_gthr_call_once_seh(&once, (void (*)(void*))(intptr_t) once_do_it, (void*) 1);
    ::printf("thread %d done\n", (int) ::_MCF_thread_self_tid());

    ::printf("thread %d quitting\n", (int) ::_MCF_thread_self_tid());
  }

int
main(void)
  {
    for(auto& thr : threads)
      thr = NS::thread(thread_proc);

    ::printf("main waiting\n");
    ::_MCF_sem_signal_some(&start, NTHREADS);

    for(auto& thr : threads)
      thr.join();

    assert(resource == 1);
  }
