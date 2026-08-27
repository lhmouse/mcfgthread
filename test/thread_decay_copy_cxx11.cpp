/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#if defined _MSC_VER  // MSVC standard library doesn't compile as C++11.
int main(void) { return 77;  }
#else  // MSVC

#include "../mcfgthread/fwd.h"

#if 0 __MCF_CXX11(+1) __MCF_CXX14(+2) != 1
#  warning Please compile this file as C++11.
#endif

#include "thread_decay_copy.cpp"

#endif  // MSVC
