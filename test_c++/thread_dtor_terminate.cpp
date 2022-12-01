/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/cxx11.hpp"
#include "../src/exit.h"
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
