/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/exit.h"
#undef NDEBUG
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
