/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/xglobals.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

int
main(void)
  {
#if defined __CYGWIN__
    return 77;
#else
    DWORD ver = GetVersion();
    DWORD os_ver = (ver & 0xFF) * 100 + (ver >> 8 & 0xFF);

    printf("os_ver = %d\n", (int) os_ver);
    if(os_ver < 1000)
      return 77;  // skip

    HMODULE kernelbase = GetModuleHandleW(L"KERNELBASE.DLL");
    assert(kernelbase);

    FARPROC fn = GetProcAddress(kernelbase, "QueryInterruptTime");
    assert(fn);
    void* gf = __MCF_G_FIELD_OPT(__f_QueryInterruptTime);
    assert(gf);
    assert((INT_PTR) fn == *(INT_PTR*) gf);
#endif
  }
