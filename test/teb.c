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

#if __MCF_64_32(64, 32) != 32
    /* 64-bit */
    assert(__MCF_teb_load_32(0x40) == (int32_t) GetCurrentProcessId());
    assert(__MCF_teb_load_32(0x48) == (int32_t) GetCurrentThreadId());

    assert(TlsSetValue(tls_index, (void*) 0x0123456789ABCDEF));
    assert(__MCF_teb_load_ptr(0x1480 + tls_index * 8) == (void*) 0x0123456789ABCDEF);
    assert(__MCF_teb_load_32(0x1480 + tls_index * 8) == (int32_t) 0x89ABCDEF);
    assert(__MCF_teb_load_32(0x1480 + tls_index * 8 + 4) == (int32_t) 0x01234567);

    __MCF_teb_store_ptr(0x1480 + tls_index * 8, (void*) 0x13577531AFFADCCD);
    assert(TlsGetValue(tls_index) == (void*) 0x13577531AFFADCCD);

    __MCF_teb_store_32(0x1480 + tls_index * 8, (int32_t) 0x21436587);
    __MCF_teb_store_32(0x1480 + tls_index * 8 + 4, (int32_t) 0xA9CBED0F);
    assert(TlsGetValue(tls_index) == (void*) 0xA9CBED0F21436587);

    void* rtl_user_process_parameters = *(void**) ((char*) __MCF_peb() + 0x20);
    assert(rtl_user_process_parameters != NULL);
    assert(GetStdHandle(STD_INPUT_HANDLE) == *(HANDLE*) ((char*) rtl_user_process_parameters + 0x20));
    assert(GetStdHandle(STD_OUTPUT_HANDLE) == *(HANDLE*) ((char*) rtl_user_process_parameters + 0x28));
    assert(GetStdHandle(STD_ERROR_HANDLE) == *(HANDLE*) ((char*) rtl_user_process_parameters + 0x30));
#endif

#if __MCF_64_32(64, 32) != 64
    /* 32-bit */
    assert(__MCF_teb_load_32(0x20) == (int32_t) GetCurrentProcessId());
    assert(__MCF_teb_load_32(0x24) == (int32_t) GetCurrentThreadId());

    assert(TlsSetValue(tls_index, (void*) 0x89ABCDEF));
    assert(__MCF_teb_load_ptr(0x0E10 + tls_index * 4) == (void*) 0x89ABCDEF);
    assert(__MCF_teb_load_32(0x0E10 + tls_index * 4) == (int32_t) 0x89ABCDEF);

    __MCF_teb_store_ptr(0x0E10 + tls_index * 4, (void*) 0x13577531);
    assert(TlsGetValue(tls_index) == (void*) 0x13577531);

    __MCF_teb_store_32(0x0E10 + tls_index * 4, (int32_t) 0x21436587);
    assert(TlsGetValue(tls_index) == (void*) 0x21436587);

    void* rtl_user_process_parameters = *(void**) ((char*) __MCF_peb() + 0x10);
    assert(rtl_user_process_parameters != NULL);
    assert(GetStdHandle(STD_INPUT_HANDLE) == *(HANDLE*) ((char*) rtl_user_process_parameters + 0x18));
    assert(GetStdHandle(STD_OUTPUT_HANDLE) == *(HANDLE*) ((char*) rtl_user_process_parameters + 0x1C));
    assert(GetStdHandle(STD_ERROR_HANDLE) == *(HANDLE*) ((char*) rtl_user_process_parameters + 0x20));
#endif
  }
