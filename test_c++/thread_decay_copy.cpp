/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

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

struct probe
  {
    int value;

    explicit
    probe(int x)
      {
        this->value = x;
      }

    probe(const probe& r)
      {
        ::printf("copy ctor invoked; aborting: %d\n", r.value);
        ::abort();
      }

    probe(probe&& r) noexcept
      {
        this->value = r.value;
        r.value = -1;
      }

    probe& operator=(const probe& r)
      {
        ::printf("copy assignment invoked; aborting: %d\n", r.value);
        ::abort();
      }

    probe& operator=(probe&& r) noexcept
      {
        if(this == &r) {
          ::printf("move assignment to self invoked; aborting: %d\n", r.value);
          ::abort();
        }
        else {
          this->value = r.value;
          r.value = -1;
          return *this;
        }
      }

    void
    operator()(probe&& x, probe&& y, probe&& z)
      {
        ::printf("x, y, z = %d, %d, %d\n", x.value, y.value, z.value);
        assert(x.value == 1);
        assert(y.value == 2);
        assert(z.value == 3);
      }
  };

int
main(void)
  {
    NS::thread(probe(0), probe(1), probe(2), probe(3)).join();
  }
