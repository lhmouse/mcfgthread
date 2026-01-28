/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#if !defined __SEH__ && !defined _MSC_VER
int main(void) { return 77;  }
#else  // __SEH__

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
once_do_it(void* add)
  {
    /* Perform initialization.  */
    int old = resource;
    NS::this_thread::sleep_for(NS::chrono::milliseconds(20));
    resource = old + (int)(intptr_t) add;

    NS::this_thread::sleep_for(NS::chrono::milliseconds(10));
    ::fprintf(stderr, "thread %d done\n", (int) ::_MCF_thread_self_tid());

#if defined __MCF_M_X8664_ASM
      register int64_t dummy __asm__("rsi") = 0x123456789abcdef;
      __asm__ volatile ("" : "+r"(dummy));
#elif defined __MCF_M_ARM64_ASM
      register int64_t dummy __asm__("x25") = 0x123456789abcdef;
      __asm__ volatile ("" : "+r"(dummy));
#endif
    throw 42;
  }

static
void
thread_proc()
  {
    ::_MCF_sem_wait(&start, nullptr);

    try {
#if defined __MCF_M_X8664_ASM
      register int64_t dummy __asm__("rsi") = 0x123456789abcdef;
      __asm__ volatile ("" : "+r"(dummy));
#elif defined __MCF_M_ARM64_ASM
      register int64_t dummy __asm__("x25") = 0x123456789abcdef;
      __asm__ volatile ("" : "+r"(dummy));
#endif
      __MCF_gthr_call_once_seh(&once, once_do_it, (void*) 1);
      ::std::terminate();
    }
    catch(...) {  }

    ::fprintf(stderr, "thread %d quitting\n", (int) ::_MCF_thread_self_tid());
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

    assert(resource == NTHREADS);
  }

#endif  // __SEH__
