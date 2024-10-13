/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

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
