/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/cxx11.hpp"
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
  }
