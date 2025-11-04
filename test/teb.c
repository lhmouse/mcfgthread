/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>
#include "../mcfgthread/xglobals.h"
#include <assert.h>
#include <stdio.h>

int
main(void)
  {
    DWORD tls_index = TlsAlloc();
    assert(tls_index != TLS_OUT_OF_INDEXES);

    if(tls_index >= 64)
      return 77;

    uintptr_t uptr;
    uint32_t u32;

#if __MCF_64_32(64, 32) != 32
    /* 64-bit */
    __MCF_TEB_LDPTR_IMM(&uptr, 0x40);
    assert(uptr == GetCurrentProcessId());
    __MCF_TEB_LDPTR_IMM(&uptr, 0x48);
    assert(uptr == GetCurrentThreadId());

    __MCF_TEB_LD32_IMM(&u32, 0x40);
    assert(u32 == GetCurrentProcessId());
    __MCF_TEB_LD32_IMM(&u32, 0x48);
    assert(u32 == GetCurrentThreadId());

    assert(TlsSetValue(tls_index, (void*) 0x0123456789ABCDEF));
    __MCF_TEB_LDPTR_SIB(&uptr, 0x1480, tls_index);
    assert(uptr == 0x0123456789ABCDEF);

    __MCF_TEB_LD32_SIB(&u32, 0x1480, tls_index * 2);
    assert(u32 == 0x89ABCDEF);
    __MCF_TEB_LD32_SIB(&u32, 0x1480, tls_index * 2 + 1);
    assert(u32 == 0x01234567);

    __MCF_TEB_STPTR_SIB(0x1480, tls_index, 0x13577531AFFADCCD);
    assert(TlsGetValue(tls_index) == (void*) 0x13577531AFFADCCD);

    __MCF_TEB_ST32_SIB(0x1480, tls_index * 2, 0x21436587);
    __MCF_TEB_ST32_SIB(0x1480, tls_index * 2 + 1, 0xA9CBED0F);
    assert(TlsGetValue(tls_index) == (void*) 0xA9CBED0F21436587);
#endif

#if __MCF_64_32(64, 32) != 64
    /* 32-bit */
    __MCF_TEB_LDPTR_IMM(&uptr, 0x20);
    assert(uptr == GetCurrentProcessId());
    __MCF_TEB_LDPTR_IMM(&uptr, 0x24);
    assert(uptr == GetCurrentThreadId());

    __MCF_TEB_LD32_IMM(&u32, 0x20);
    assert(u32 == GetCurrentProcessId());
    __MCF_TEB_LD32_IMM(&u32, 0x24);
    assert(u32 == GetCurrentThreadId());

    assert(TlsSetValue(tls_index, (void*) 0x89ABCDEF));
    __MCF_TEB_LDPTR_SIB(&uptr, 0x0E10, tls_index);
    assert(uptr == 0x89ABCDEF);

    __MCF_TEB_LD32_SIB(&u32, 0x0E10, tls_index);
    assert(u32 == 0x89ABCDEF);

    __MCF_TEB_STPTR_SIB(0x0E10, tls_index, 0x13577531);
    assert(TlsGetValue(tls_index) == (void*) 0x13577531);

    __MCF_TEB_ST32_SIB(0x0E10, tls_index, 0x21436587);
    assert(TlsGetValue(tls_index) == (void*) 0x21436587);
#endif
  }
