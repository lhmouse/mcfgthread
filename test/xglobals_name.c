/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <stdio.h>
#include <assert.h>

extern HANDLE __MCF_crt_ntdll;
extern HANDLE __MCF_crt_kernelbase;
extern HANDLE __MCF_crt_kernel32;
extern void* __MCF_g;

int
main(void)
  {
    if(!__MCF_crt_ntdll || !__MCF_crt_kernelbase || !__MCF_crt_kernel32)
      return 77;

    WCHAR gname_str[128] = L"Local\\__MCF_crt_xglobals_";
    WCHAR* eptr = gname_str + wcslen(gname_str);
    UINT32 pid = GetCurrentProcessId();
    for(UINT i = 0; i != 8; ++i)
      *(eptr ++) = L"0123456789ABCDEF"[(pid >> (28 - i * 4)) & 0x0FU];
    *(eptr ++) = '_';
    UINT64 cookie = (UINT64) pid << 32 | pid;
    cookie = (UINT_PTR) EncodePointer((PVOID)(UINT_PTR) ~cookie) * 0x9E3779B97F4A7C15ULL;
    for(UINT i = 0; i != 16; ++i)
      *(eptr ++) = L"GHJKLMNPQRSTUWXY"[(cookie >> (60 - i * 4)) & 0x0FU];
    *eptr = 0;
    fprintf(stderr, "gname_str = %ls\n", gname_str);

    UNICODE_STRING gname = { .Length = (USHORT) ((eptr - gname_str) * 2),
                             .MaximumLength = sizeof(gname_str),
                             .Buffer = gname_str };

    OBJECT_ATTRIBUTES attrs = { .Length = sizeof(OBJECT_ATTRIBUTES),
                                .ObjectName = &gname };

    typedef NTSTATUS NTAPI BaseGetNamedObjectDirectory_t(HANDLE*);
    BaseGetNamedObjectDirectory_t* pfnBaseGetNamedObjectDirectory =
        (BaseGetNamedObjectDirectory_t*)(INT_PTR) GetProcAddress(
             __MCF_crt_kernel32, "BaseGetNamedObjectDirectory");
    assert(pfnBaseGetNamedObjectDirectory);
    assert(pfnBaseGetNamedObjectDirectory(&attrs.RootDirectory) == STATUS_SUCCESS);

    typedef NTSTATUS NTAPI NtOpenSection_t(HANDLE*, ACCESS_MASK, OBJECT_ATTRIBUTES*);
    NtOpenSection_t* pfnNtOpenSection =
        (NtOpenSection_t*)(INT_PTR) GetProcAddress(
             __MCF_crt_ntdll, "NtOpenSection");
    assert(pfnNtOpenSection);
    HANDLE hmap;
    assert(pfnNtOpenSection(&hmap, SECTION_ALL_ACCESS, &attrs) == STATUS_ACCESS_DENIED);
    attrs.Attributes = OBJ_EXCLUSIVE;
    assert(pfnNtOpenSection(&hmap, SECTION_ALL_ACCESS, &attrs) == STATUS_SUCCESS);

    char* gmem = MapViewOfFile(hmap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    assert(gmem);
    assert(*(void**) gmem == __MCF_g);
  }
