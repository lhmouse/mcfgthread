/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/sem.h"
#include <assert.h>
#include <stdio.h>
#include <deque>

#ifdef TEST_STD
namespace NS = std;
#else
namespace NS = ::_MCF;
#endif

int
main()
  {
#ifdef TEST_STD
    return 77;  // skipped
#else
    std::deque<NS::thread_specific_ptr<int>> keys;

    constexpr std::size_t NKEYS = 1000U;
    while(keys.size() < NKEYS) {
      keys.emplace_back(nullptr);  // no destructor
      assert(!keys.back());
      assert(keys.back().get() == nullptr);
    }

    constexpr std::size_t NVALS = 10000U;
    for(std::size_t v = 0;  v != NVALS;  ++v) {
      // set and get
      for(std::size_t k = 0;  k != NKEYS;  ++k)
        keys.at(k).reset((int*) v);

      for(std::size_t k = 0;  k != NKEYS;  ++k)
        assert(keys.at(k).get() == (int*) v);
    }
#endif
  }
