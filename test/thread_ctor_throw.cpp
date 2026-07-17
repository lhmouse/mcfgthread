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
