/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
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

#if defined __MCF_M_X8664 || defined __MCF_M_ARM64EC
    assert(__readgsdword(0x40) == GetCurrentProcessId());
    assert(__readgsdword(0x48) == GetCurrentThreadId());

    assert(TlsSetValue(tls_index, (void*) 0x0123456789ABCDEF));
    assert(__readgsqword(0x1480 + tls_index * 8) == 0x0123456789ABCDEF);
    assert(__readgsdword(0x1480 + tls_index * 8) == 0x89ABCDEF);
    assert(__readgsdword(0x1480 + tls_index * 8 + 4) == 0x01234567);
#endif

#if defined _MSC_VER && (defined __MCF_M_ARM64 || defined __MCF_M_ARM64EC)
    assert(__readx18dword(0x40) == GetCurrentProcessId());
    assert(__readx18dword(0x48) == GetCurrentThreadId());

    assert(TlsSetValue(tls_index, (void*) 0x0123456789ABCDEF));
    assert(__readx18qword(0x1480 + tls_index * 8) == 0x0123456789ABCDEF);
    assert(__readx18dword(0x1480 + tls_index * 8) == 0x89ABCDEF);
    assert(__readx18dword(0x1480 + tls_index * 8 + 4) == 0x01234567);
#endif

#if defined __MCF_M_X8632
    assert(__readfsdword(0x20) == GetCurrentProcessId());
    assert(__readfsdword(0x24) == GetCurrentThreadId());

    assert(TlsSetValue(tls_index, (void*) 0x89ABCDEF));
    assert(__readfsdword(0x0E10 + tls_index * 4) == 0x89ABCDEF);
#endif
  }
