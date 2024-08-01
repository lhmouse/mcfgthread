/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cxx11.hpp"

#ifdef __EXCEPTIONS
#  error -fno-exceptions
#endif

#ifdef __GXX_RTTI
#  error -fno-rtti
#endif

int
main(void)
  {
    return 0;
  }
