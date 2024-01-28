/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/cxx11.hpp"
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
