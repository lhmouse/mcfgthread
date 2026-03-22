/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/clock.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    for(uint32_t i = 0;  i < 1000000;  i++) {
      ULONGLONG t1 = GetTickCount64();
      ULONGLONG t2 = (ULONGLONG) _MCF_tick_count();

      assert(t1 <= t2);
      assert(t2 - t1 <= 20000);
    }
  }
