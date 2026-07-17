/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/clock.h"
#include <windows.h>
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    for(uint32_t i = 0;  i < 200;  i++) {
      Sleep(10);
      ULONGLONG t1 = GetTickCount64();
      ULONGLONG t2 = (ULONGLONG) _MCF_tick_count();

      assert(t1 <= t2);
      assert(t2 - t1 <= 100);
    }

    for(uint32_t i = 0;  i < 200;  i++) {
      Sleep(10);
      ULONGLONG t1 = (ULONGLONG) _MCF_tick_count();
      ULONGLONG t2 = (ULONGLONG) _MCF_hires_tick_count();

      assert(t1 <= t2);
      assert(t2 - t1 <= 10);
    }
  }
