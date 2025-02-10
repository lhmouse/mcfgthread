/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>
#include <vector>

#ifdef TEST_STD
#  include <thread>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

#ifndef TEST_STD
static
void
tls_destructor(int* ptr)
  {
    ::fprintf(stderr, "thread %d tls_destructor\n", (int) ::_MCF_thread_self_tid());
    _MCF_atomic_xadd_32_rlx(ptr, 1);
  }

static NS::thread_specific_ptr<int> tss_ptr(tls_destructor);

constexpr std::size_t NTHREADS = 64U;
static std::vector<NS::thread> threads(NTHREADS);
static ::_MCF_sem start = __MCF_SEM_INIT(NTHREADS);
static int resource = 0;

static
void
thread_proc()
  {
    ::_MCF_sem_wait(&start, nullptr);

    // Add a resource.
    tss_ptr.reset(&resource);

    ::fprintf(stderr, "thread %d quitting\n", (int) ::_MCF_thread_self_tid());
  }
#endif

int
main(void)
  {
#ifdef TEST_STD
    return 77;  // skipped
#else
    for(auto& thr : threads)
      thr = NS::thread(thread_proc);

    ::fprintf(stderr, "main waiting\n");
    ::_MCF_sem_signal_some(&start, NTHREADS);

    for(auto& thr : threads)
      thr.join();

    assert(resource == NTHREADS);
#endif
  }
