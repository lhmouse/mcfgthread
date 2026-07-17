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
      [&] {
        try {
          thr.join();

          // unreachable
          ::std::terminate();
        }
        catch(::std::system_error& e) {
          // XXX: unknown?
          ::fprintf(stderr, "error = %s\n", e.what());
        }
      });

    thr.join();
  }
