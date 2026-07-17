/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"
#include "../mcfgthread/sem.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <deque>

#ifdef TEST_STD
#  include <thread>
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
    NS::thread_specific_ptr<int> tss_ptr;  // no destructor

    assert(tss_ptr.get() == nullptr);
    tss_ptr.reset(new int());
    assert(tss_ptr.get() != nullptr);
    assert(*tss_ptr == 0);
    *tss_ptr = 42;
    assert(*tss_ptr == 42);

    NS::thread(
      [&] {
        assert(tss_ptr.get() == nullptr);
        tss_ptr.reset(new int());
        assert(tss_ptr.get() != nullptr);
        assert(*tss_ptr == 0);
        *tss_ptr = 56;
        assert(*tss_ptr == 56);
      })
      .join();

    assert(*tss_ptr == 42);
    tss_ptr.reset();
    assert(tss_ptr.get() == nullptr);
#endif
  }
