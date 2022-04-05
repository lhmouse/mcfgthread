// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_WIN32_C_  1
#include "win32.h"

EXCEPTION_DISPOSITION __cdecl
__MCF_SEH_top_dispatcher(EXCEPTION_RECORD* record, void* frame, CONTEXT* ctx, void* disp_ctx)
  {
    (void)frame;
    (void)ctx;
    (void)disp_ctx;

    // Check for uncaught exceptions.
    // This is necessary for `std::terminate()` to be functional under such
    // circumstances.
    if(record->ExceptionFlags & EXCEPTION_NONCONTINUABLE)
      return ExceptionContinueSearch;

    if((record->ExceptionCode & 0x20FFFFFF) != 0x20474343)  // ('GCC' | (1 << 29))
      return ExceptionContinueSearch;

    return ExceptionContinueExecution;
  }
