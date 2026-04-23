/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#if defined __CYGWIN__
int main(void) { return 77;  }
#else  // __CYGWIN__

#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>
#include "../mcfgthread/src/xglobals.h"
#include <stdio.h>
#undef NDEBUG
#include <assert.h>

int
main(void)
  {
    HMODULE kernel32 = GetModuleHandleW(L"KERNEL32.DLL");
    if(!kernel32)
      return 77;  // skip

    HMODULE kernelbase = GetModuleHandleW(L"KERNELBASE.DLL");
    if(!kernelbase)
      return 77;  // skip

    // https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimepreciseasfiletime
    void* fn = (void*) __MCF_G_IMP_OPT(GetSystemTimePreciseAsFileTime);
    fprintf(stderr, "GetSystemTimePreciseAsFileTime = %p\n", fn);
    assert(fn == (void*) GetProcAddress(kernel32, "GetSystemTimePreciseAsFileTime"));

    // https://learn.microsoft.com/en-us/windows/win32/api/realtimeapiset/nf-realtimeapiset-queryinterrupttime
    fn = (void*) __MCF_G_IMP_OPT(QueryInterruptTime);
    fprintf(stderr, "QueryInterruptTime = %p\n", fn);
    assert(fn == (void*) GetProcAddress(kernelbase, "QueryInterruptTime"));

    // https://learn.microsoft.com/en-us/windows/win32/api/realtimeapiset/nf-realtimeapiset-queryinterrupttimeprecise
    fn = (void*) __MCF_G_IMP_OPT(QueryInterruptTimePrecise);
    fprintf(stderr, "QueryInterruptTimePrecise = %p\n", fn);
    assert(fn == (void*) GetProcAddress(kernelbase, "QueryInterruptTimePrecise"));

    // https://learn.microsoft.com/en-us/windows/win32/api/realtimeapiset/nf-realtimeapiset-queryunbiasedinterrupttimeprecise
    fn = (void*) __MCF_G_IMP_OPT(QueryUnbiasedInterruptTimePrecise);
    fprintf(stderr, "QueryUnbiasedInterruptTimePrecise = %p\n", fn);
    assert(fn == (void*) GetProcAddress(kernelbase, "QueryUnbiasedInterruptTimePrecise"));
  }

#endif  // __CYGWIN__
