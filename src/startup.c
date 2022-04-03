// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCF_STARTUP  1
#include "mcfgthread.h"
#include "mcfwin.h"

int __cdecl
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  __asm__("__MCF_startup");

extern char crt_image_base
  __asm__("__image_base__");

void* const __MCF_crt_module = &crt_image_base;
uint32_t __MCF_tls_index = TLS_OUT_OF_INDEXES;
__MCF_mopthread_t __MCF_main_thread = { 0 };


int __cdecl
__MCF_startup(HANDLE instance, DWORD reason, LPVOID reserved)
  {
    return TRUE;
  }
