/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#if defined _MSC_VER  // MSVC standard library doesn't compile as C++11.
int main(void) { return 77;  }
#else  // MSVC

#include "../mcfgthread/cxx11.hpp"

#if 0 __MCF_CXX11(+1) __MCF_CXX14(+2) != 1
#  warning Please compile this file as C++11.
#endif

int
main(void)
  {
    return 0;
  }

#endif  // MSVC
