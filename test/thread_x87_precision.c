/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/thread.h"
#include <windows.h>
#include <assert.h>
#include <stdio.h>

static
void
thread_proc(_MCF_thread* self)
  {
#if defined __i386__ || defined __amd64__
    static volatile long double eps = 0x0.000000000000001p0L;
    static volatile long double ep1 = 0x1.000000000000001p0L;

    assert(ep1 != 1);
    assert(eps + 1 == ep1);
#endif  // x87

    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

int
main(void)
  {
    // Check we are running on a real x86 machine, not emulated on ARM64.
    if(sizeof(long double) == sizeof(double))
      return 77;

    HMODULE kernel32 = GetModuleHandleW(L"KERNEL32.DLL");
    if(kernel32) {
      typedef BOOL WINAPI IsWow64Process2_t(HANDLE, USHORT*, USHORT*);
      IsWow64Process2_t* pIsWow64Process2 = __MCF_CAST_PTR(IsWow64Process2_t, GetProcAddress(kernel32, "IsWow64Process2"));
      USHORT wProcess, wNative;
      if(pIsWow64Process2 && pIsWow64Process2(GetCurrentProcess(), &wProcess, &wNative)) {
        fprintf(stderr, "IsWow64Process2: host machine = 0x%.4X\n", wNative);

        // The x87 that is emulated by ARM64 supports up to 53-bit double
        // precision and not the x87 extended precision, so skip this test.
        if((wNative != IMAGE_FILE_MACHINE_I386) && (wNative != IMAGE_FILE_MACHINE_AMD64))
          return 77;
      }
    }

    _MCF_thread* thrd = _MCF_thread_new(thread_proc, __MCF_nullptr, 0);
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    _MCF_thread_wait(thrd, __MCF_nullptr);
    fprintf(stderr, "main wait finished\n");

    _MCF_thread_wait(thrd, __MCF_nullptr);
  }
