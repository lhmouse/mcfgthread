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
#  define NS_THREAD_IS_MCF
#endif

int
main(void)
  {
    const auto main_tid = NS::this_thread::get_id();
#ifdef NS_THREAD_IS_MCF
    assert(main_tid._M_tid == ::_MCF_thread_self_tid());
#endif

    NS::thread thr(
      [&] {
        const auto my_tid = NS::this_thread::get_id();
        assert(my_tid == thr.get_id());
        assert(my_tid != main_tid);
        assert(my_tid < main_tid || my_tid > main_tid);
      });

    thr.join();
    assert(thr.get_id() == NS::thread::id());
  }
