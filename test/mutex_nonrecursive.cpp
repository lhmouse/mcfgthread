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

int
main(void)
  {
    NS::mutex m;
    assert(m.try_lock() == true);

    try
      { assert(m.try_lock() == false);  }
    catch(std::system_error& e)
      { assert(e.code() == std::errc::resource_deadlock_would_occur);  }

    m.unlock();
    assert(m.try_lock() == true);
  }
