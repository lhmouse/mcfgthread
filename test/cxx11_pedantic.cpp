/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#if defined _MSC_VER  // MSVC standard library doesn't compile as C++11.
int main(void) { return 77;  }
#else  // MSVC

#include "../mcfgthread/cxx11.hpp"

#if 0 __MCF_CXX11(+1) __MCF_CXX14(+2) != 1
#  warning Please compile this file as C++11.
#endif

__MCF_STATIC_ASSERT(__MCF_PTR_BITS == sizeof(void*) * 8);
__MCF_STATIC_ASSERT(__MCF_INTPTR_MIN == INTPTR_MIN);
__MCF_STATIC_ASSERT(sizeof(__MCF_INTPTR_MIN) == sizeof(intptr_t));
__MCF_STATIC_ASSERT(__MCF_INTPTR_0 == 0);
__MCF_STATIC_ASSERT(sizeof(__MCF_INTPTR_0) == sizeof(intptr_t));
__MCF_STATIC_ASSERT(__MCF_INTPTR_MAX == INTPTR_MAX);
__MCF_STATIC_ASSERT(sizeof(__MCF_INTPTR_MAX) == sizeof(intptr_t));
__MCF_STATIC_ASSERT(__MCF_UINTPTR_0 == 0);
__MCF_STATIC_ASSERT(sizeof(__MCF_UINTPTR_0) == sizeof(intptr_t));
__MCF_STATIC_ASSERT(__MCF_UINTPTR_MAX == UINTPTR_MAX);
__MCF_STATIC_ASSERT(sizeof(__MCF_UINTPTR_MAX) == sizeof(intptr_t));

int
main(void)
  {
    return 0;
  }

#endif  // MSVC
