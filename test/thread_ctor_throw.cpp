/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/exit.h"
#include <assert.h>
#include <stdio.h>

#ifdef TEST_STD
#  include <thread>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

struct copy_will_throw
  {
    copy_will_throw() { }
    copy_will_throw(const copy_will_throw&) { throw 42;  }
    void operator()() { ::std::terminate();  }
  };

int
main(void)
  {
    try {
      copy_will_throw t;
      NS::thread thr(t);
      ::std::terminate();
    }
    catch(int e) {
      NS::this_thread::sleep_for(NS::chrono::seconds(3));
      assert(e == 42);
    }
  }
