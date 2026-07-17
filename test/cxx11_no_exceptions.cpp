/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"

#if defined __cpp_exceptions || defined __EXCEPTIONS || defined _CPPUNWIND
#  error -fno-exceptions
#endif

#if defined __cpp_rtti || defined __GXX_RTTI || defined _CPPRTTI
#  error -fno-rtti
#endif

int
main(void)
  {
    return 0;
  }
