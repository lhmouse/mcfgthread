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
