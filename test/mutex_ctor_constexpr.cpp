/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#ifdef TEST_STD
#  include <mutex>
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

constexpr NS::mutex m1;
constexpr NS::mutex m2{};

int
main(void)
  {
    (void) m1;
    (void) m2;
  }
