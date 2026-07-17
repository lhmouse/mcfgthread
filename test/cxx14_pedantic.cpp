/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxx11.hpp"

#if 0 __MCF_CXX11(+1) __MCF_CXX14(+2) __MCF_CXX17(+4) != 3
#  warning Please compile this file as C++14.
#endif

int
main(void)
  {
    return 0;
  }
