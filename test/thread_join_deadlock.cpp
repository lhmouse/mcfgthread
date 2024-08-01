/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
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
