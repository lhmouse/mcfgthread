/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>
#include <deque>

#ifdef TEST_STD
#  include <thread>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

int
main()
  {
#ifdef TEST_STD
    return 77;  // skipped
#else
    NS::thread_specific_ptr<int> tss_ptr;  // no destructor

    assert(tss_ptr.get() == nullptr);
    tss_ptr.reset(new int());
    assert(tss_ptr.get() != nullptr);
    assert(*tss_ptr == 0);
    *tss_ptr = 42;
    assert(*tss_ptr == 42);

    NS::thread(
      [&] {
        assert(tss_ptr.get() == nullptr);
        tss_ptr.reset(new int());
        assert(tss_ptr.get() != nullptr);
        assert(*tss_ptr == 0);
        *tss_ptr = 56;
        assert(*tss_ptr == 56);
      })
      .join();

    assert(*tss_ptr == 42);
    tss_ptr.reset();
    assert(tss_ptr.get() == nullptr);
#endif
  }
