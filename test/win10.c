/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#if defined __CYGWIN__
int main(void) { return 77;  }
#else  // __CYGWIN__

#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>
#include <versionhelpers.h>
#include "../mcfgthread/xglobals.h"
#include <assert.h>

int
main(void)
  {
    if(!IsWindows10OrGreater())
      return 77;  // skip

    HMODULE kernelbase = GetModuleHandleW(L"KERNELBASE.DLL");
    assert(kernelbase);

    FARPROC fn = GetProcAddress(kernelbase, "QueryInterruptTime");
    assert(fn);
    void* gf = __MCF_G_FIELD_OPT(__f_QueryInterruptTime);
    assert(gf);
    assert((INT_PTR) fn == *(INT_PTR*) gf);
  }

#endif  // __CYGWIN__
