/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_FWD_IMPORT  __MCF_DLLEXPORT
#define __MCF_FWD_INLINE  __MCF_DLLEXPORT
#include "fwd.h"
#include "xglobals.h"

__MCF_DLLEXPORT
void
__MCF_runtime_failure(const char* where)
  {
    /* `where` can be examined with a debugger.  */
    __asm__ volatile ("" : : "m"(where) : "memory");
    TerminateProcess(GetCurrentProcess(), STATUS_FATAL_APP_EXIT);
    __builtin_unreachable();
  }

__MCF_DLLEXPORT
uint32_t
_MCF_get_win32_error(void)
  {
    return GetLastError();
  }

__MCF_DLLEXPORT
size_t
_MCF_get_page_size(void)
  {
    return __MCF_crt_sysinfo.dwPageSize;
  }

__MCF_DLLEXPORT
size_t
_MCF_get_processor_count(void)
  {
    return __MCF_crt_sysinfo.dwNumberOfProcessors;
  }

__MCF_DLLEXPORT
uintptr_t
_MCF_get_active_processor_mask(void)
  {
    return __MCF_crt_sysinfo.dwActiveProcessorMask;
  }
