/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2024, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/xglobals.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

int
main(void)
  {
    DWORD ver = GetVersion();
    DWORD os_ver = (ver & 0xFF) * 100 + (ver >> 8 & 0xFF);

    printf("os_ver = %ld\n", os_ver);
    if(os_ver < 602)
      return 77;  // skip

    HMODULE kernel32 = GetModuleHandleW(L"KERNEL32");
    assert(kernel32);

    FARPROC fn = GetProcAddress(kernel32, "GetSystemTimePreciseAsFileTime");
    assert(fn);
    void* gf = __MCF_G_FIELD_OPT(__f_GetSystemTimePreciseAsFileTime);
    assert(gf);
    assert((INT_PTR) fn == *(INT_PTR*) gf);
  }
