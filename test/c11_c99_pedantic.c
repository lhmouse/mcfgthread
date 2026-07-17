/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/c11.h"

#if 0 __MCF_C99(+1) __MCF_C11(+2) != 1
#  warning Please compile this file as C99.
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
