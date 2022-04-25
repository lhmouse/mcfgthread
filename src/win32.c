/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#define __MCF_WIN32_EXTERN_INLINE
#include "win32.h"

EXCEPTION_DISPOSITION __cdecl
__MCF_seh_top(EXCEPTION_RECORD* record, void* frame, CONTEXT* ctx, void* disp_ctx)
  {
    UNREFERENCED_PARAMETER(frame);
    UNREFERENCED_PARAMETER(ctx);
    UNREFERENCED_PARAMETER(disp_ctx);

    /* Check for uncaught C++ exceptions.  */
    if(record->ExceptionFlags & EXCEPTION_NONCONTINUABLE)
      return ExceptionContinueSearch;

    if((record->ExceptionCode & 0x20FFFFFF) != 0x20474343)  /* (1 << 29) | 'GCC'  */
      return ExceptionContinueSearch;

    /* Cause the C++ runtime to invoke the terminate handler.
     * By default this is `std::terminate()`.  */
    return ExceptionContinueExecution;
  }

void* __cdecl
memcpy(void* dst, const void* src, size_t size)
  __attribute__((__alias__("__MCF_mcopy")));

void* __cdecl
memmove(void* dst, const void* src, size_t size)
  __attribute__((__alias__("__MCF_mcopy")));

void* __cdecl
memset(void* dst, int val, size_t size)
  __attribute__((__alias__("__MCF_mfill")));
