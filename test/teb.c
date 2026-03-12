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
    assert(__MCF_teb_load_8(0x1480 + tls_index * 8 + 0) == (int8_t) 0xEF);
    assert(__MCF_teb_load_8(0x1480 + tls_index * 8 + 1) == (int8_t) 0xCD);
    assert(__MCF_teb_load_8(0x1480 + tls_index * 8 + 2) == (int8_t) 0xAB);
    assert(__MCF_teb_load_8(0x1480 + tls_index * 8 + 3) == (int8_t) 0x89);
    assert(__MCF_teb_load_8(0x1480 + tls_index * 8 + 4) == (int8_t) 0x67);
    assert(__MCF_teb_load_8(0x1480 + tls_index * 8 + 5) == (int8_t) 0x45);
    assert(__MCF_teb_load_8(0x1480 + tls_index * 8 + 6) == (int8_t) 0x23);
    assert(__MCF_teb_load_8(0x1480 + tls_index * 8 + 7) == (int8_t) 0x01);
    assert(__MCF_teb_load_16(0x1480 + tls_index * 8 + 0) == (int16_t) 0xCDEF);
    assert(__MCF_teb_load_16(0x1480 + tls_index * 8 + 2) == (int16_t) 0x89AB);
    assert(__MCF_teb_load_16(0x1480 + tls_index * 8 + 4) == (int16_t) 0x4567);
    assert(__MCF_teb_load_16(0x1480 + tls_index * 8 + 6) == (int16_t) 0x0123);
    assert(__MCF_teb_load_32(0x1480 + tls_index * 8 + 0) == (int32_t) 0x89ABCDEF);
    assert(__MCF_teb_load_32(0x1480 + tls_index * 8 + 4) == (int32_t) 0x01234567);

    __MCF_teb_store_ptr(0x1480 + tls_index * 8, (void*) 0x13577531AFFADCCD);
    assert(TlsGetValue(tls_index) == (void*) 0x13577531AFFADCCD);

    __MCF_teb_store_8(0x1480 + tls_index * 8 + 7, (int8_t) 0xA5);
    __MCF_teb_store_8(0x1480 + tls_index * 8 + 6, (int8_t) 0xC1);
    __MCF_teb_store_8(0x1480 + tls_index * 8 + 5, (int8_t) 0xBF);
    __MCF_teb_store_8(0x1480 + tls_index * 8 + 4, (int8_t) 0x0E);
    __MCF_teb_store_8(0x1480 + tls_index * 8 + 3, (int8_t) 0x4E);
    __MCF_teb_store_8(0x1480 + tls_index * 8 + 2, (int8_t) 0x6E);
    __MCF_teb_store_8(0x1480 + tls_index * 8 + 1, (int8_t) 0xCA);
    __MCF_teb_store_8(0x1480 + tls_index * 8 + 0, (int8_t) 0x54);
    assert(TlsGetValue(tls_index) == (void*) 0xA5C1BF0E4E6ECA54);

    __MCF_teb_store_16(0x1480 + tls_index * 8 + 6, (int16_t) 0xD378);
    __MCF_teb_store_16(0x1480 + tls_index * 8 + 4, (int16_t) 0xE169);
    __MCF_teb_store_16(0x1480 + tls_index * 8 + 2, (int16_t) 0xD919);
    __MCF_teb_store_16(0x1480 + tls_index * 8 + 0, (int16_t) 0x4F46);
    assert(TlsGetValue(tls_index) == (void*) 0xD378E169D9194F46);

    __MCF_teb_store_32(0x1480 + tls_index * 8 + 4, (int32_t) 0xA9CBED0F);
    __MCF_teb_store_32(0x1480 + tls_index * 8 + 0, (int32_t) 0x21436587);
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
    assert(__MCF_teb_load_8(0x0E10 + tls_index * 4 + 3) == (int8_t) 0x89);
    assert(__MCF_teb_load_8(0x0E10 + tls_index * 4 + 2) == (int8_t) 0xAB);
    assert(__MCF_teb_load_8(0x0E10 + tls_index * 4 + 1) == (int8_t) 0xCD);
    assert(__MCF_teb_load_8(0x0E10 + tls_index * 4 + 0) == (int8_t) 0xEF);
    assert(__MCF_teb_load_16(0x0E10 + tls_index * 4 + 2) == (int16_t) 0x89AB);
    assert(__MCF_teb_load_16(0x0E10 + tls_index * 4 + 0) == (int16_t) 0xCDEF);
    assert(__MCF_teb_load_32(0x0E10 + tls_index * 4) == (int32_t) 0x89ABCDEF);

    __MCF_teb_store_ptr(0x0E10 + tls_index * 4, (void*) 0x13577531);
    assert(TlsGetValue(tls_index) == (void*) 0x13577531);

    __MCF_teb_store_8(0x0E10 + tls_index * 4 + 3, (int8_t) 0x4E);
    __MCF_teb_store_8(0x0E10 + tls_index * 4 + 2, (int8_t) 0x6E);
    __MCF_teb_store_8(0x0E10 + tls_index * 4 + 1, (int8_t) 0xCA);
    __MCF_teb_store_8(0x0E10 + tls_index * 4 + 0, (int8_t) 0x54);
    assert(TlsGetValue(tls_index) == (void*) 0x4E6ECA54);

    __MCF_teb_store_16(0x0E10 + tls_index * 4 + 2, (int16_t) 0xD919);
    __MCF_teb_store_16(0x0E10 + tls_index * 4 + 0, (int16_t) 0x4F46);
    assert(TlsGetValue(tls_index) == (void*) 0xD9194F46);

    __MCF_teb_store_32(0x0E10 + tls_index * 4, (int32_t) 0x21436587);
    assert(TlsGetValue(tls_index) == (void*) 0x21436587);

    void* rtl_user_process_parameters = *(void**) ((char*) __MCF_peb() + 0x10);
    assert(rtl_user_process_parameters != NULL);
    assert(GetStdHandle(STD_INPUT_HANDLE) == *(HANDLE*) ((char*) rtl_user_process_parameters + 0x18));
    assert(GetStdHandle(STD_OUTPUT_HANDLE) == *(HANDLE*) ((char*) rtl_user_process_parameters + 0x1C));
    assert(GetStdHandle(STD_ERROR_HANDLE) == *(HANDLE*) ((char*) rtl_user_process_parameters + 0x20));
#endif
  }
