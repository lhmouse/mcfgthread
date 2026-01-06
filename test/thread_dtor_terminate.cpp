/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
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

static
void
my_terminate()
  {
    ::__MCF__Exit(0);
  }

int
main(void)
  {
    ::std::set_terminate(my_terminate);

    NS::thread thr(
      [] {
        NS::this_thread::sleep_for(NS::chrono::hours(1));
      });

    return 1;
  }
