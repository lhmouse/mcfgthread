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

int
main(void)
  {
    NS::thread thr(
      [] {
        NS::this_thread::sleep_for(NS::chrono::seconds(1));
      });

    thr.join();

    thr = NS::thread(
      [] {
        NS::this_thread::sleep_for(NS::chrono::seconds(1));
      });

    thr.join();
  }
