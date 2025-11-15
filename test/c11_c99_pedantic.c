/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/c11.h"

#if 0 __MCF_C99(+1) __MCF_C11(+2) != 1
#  warning Please compile this file as C99.
#endif

__MCF_STATIC_ASSERT(__MCF_PTR_BITS == sizeof(void*) * 8);
__MCF_STATIC_ASSERT(__MCF_IPTR_MIN == INTPTR_MIN);
__MCF_STATIC_ASSERT(sizeof(__MCF_IPTR_MIN) == sizeof(intptr_t));
__MCF_STATIC_ASSERT(__MCF_IPTR_0 == 0);
__MCF_STATIC_ASSERT(sizeof(__MCF_IPTR_0) == sizeof(intptr_t));
__MCF_STATIC_ASSERT(__MCF_IPTR_MAX == INTPTR_MAX);
__MCF_STATIC_ASSERT(sizeof(__MCF_IPTR_MAX) == sizeof(intptr_t));
__MCF_STATIC_ASSERT(__MCF_UPTR_0 == 0);
__MCF_STATIC_ASSERT(sizeof(__MCF_UPTR_0) == sizeof(intptr_t));
__MCF_STATIC_ASSERT(__MCF_UPTR_MAX == UINTPTR_MAX);
__MCF_STATIC_ASSERT(sizeof(__MCF_UPTR_MAX) == sizeof(intptr_t));

int
main(void)
  {
    return 0;
  }
