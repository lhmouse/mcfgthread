/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

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
