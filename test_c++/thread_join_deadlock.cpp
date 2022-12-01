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
      [&] {
        try {
          thr.join();

          // unreachable
          ::std::terminate();
        }
        catch(::std::system_error& e) {
          // XXX: unknown?
          ::printf("error = %s\n", e.what());
        }
      });

    thr.join();
  }
