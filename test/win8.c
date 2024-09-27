/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/xglobals.h"
#include <assert.h>
#include <windows.h>
#include <versionhelpers.h>

int
main(void)
  {
#if defined __CYGWIN__
    return 77;
#else
    if(!IsWindows8OrGreater())
      return 77;  // skip

    HMODULE kernelbase = GetModuleHandleW(L"KERNELBASE.DLL");
    assert(kernelbase);

    FARPROC fn = GetProcAddress(kernelbase, "GetSystemTimePreciseAsFileTime");
    assert(fn);
    void* gf = __MCF_G_FIELD_OPT(__f_GetSystemTimePreciseAsFileTime);
    assert(gf);
    assert((INT_PTR) fn == *(INT_PTR*) gf);
#endif
  }
