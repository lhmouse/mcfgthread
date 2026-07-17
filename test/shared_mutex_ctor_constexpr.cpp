/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"

int
main(void)
  {
#ifdef TEST_STD
    return 77;  // skip
#else
    constexpr _MCF::shared_mutex m1;
    constexpr _MCF::shared_mutex m2{};
    (void) m1;
    (void) m2;
#endif
  }
