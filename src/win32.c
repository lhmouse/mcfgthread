/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "config.h"
#define __MCF_WIN32_EXTERN_INLINE
#include "xwin32.i"

EXCEPTION_DISPOSITION __cdecl
__MCF_seh_top(EXCEPTION_RECORD* record, void* estab_frame, CONTEXT* ctx, void* disp_ctx)
  {
    (void) estab_frame;
    (void) ctx;
    (void) disp_ctx;

    /* Check for uncaught C++ exceptions.  */
    if(record->ExceptionFlags & EXCEPTION_NONCONTINUABLE)
      return ExceptionContinueSearch;

    if((record->ExceptionCode & 0x20FFFFFF) != 0x20474343)  /* (1 << 29) | 'GCC'  */
      return ExceptionContinueSearch;

    /* Cause the C++ runtime to invoke the terminate handler.  */
    return ExceptionContinueExecution;
  }

size_t
__MCF_batch_release_common(const void* key, size_t count)
  {
    if(count == 0)
      return 0;

    /* A call to `ExitProcess()` terminates all the other threads, even if
     * they are waiting. We don't release the keyed event in this case, as it
     * blocks the calling thread infinitely if there is no thread to wake up.
     * See <https://github.com/lhmouse/mcfgthread/issues/21>.  */
    if(RtlDllShutdownInProgress())
      return 0;

    for(size_t k = 0;  k != count;  ++k)
      __MCF_keyed_event_signal(key, NULL);  /* infinite timeout  */

    return count;
  }
