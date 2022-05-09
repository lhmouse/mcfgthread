/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_WINNT_EXTERN_INLINE  __MCF_DLLEXPORT
#include "xwinnt.i"

__MCF_DLLEXPORT
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

__MCF_DLLEXPORT
void
__MCF_initialize_timeout_v2(__MCF_winnt_timeout* to, const int64_t* int64_opt)
  {
    /* Initialize it to an infinite value.  */
    to->__li->QuadPart = INT64_MIN;

    /* If no timeout is given, wait indefinitely.  */
    if(!int64_opt)
      return;

    if(*int64_opt > 0) {
      /* If `*int64_opt` is positive, it denotes the number of milliseconds
       * since 1970-01-01T00:00:00Z, and has to be converted into the number of
       * 100 nanoseconds since the 1601-01-01T00:00:00Z.  */
      if(*int64_opt > 910692730085477)
        return;

      to->__li->QuadPart = (11644473600000 + *int64_opt) * 10000;
      to->__since = 0;
    }
    else if(*int64_opt < 0) {
      /* If `*int64_opt` is negative, it denotes the number of milliseconds
       * to wait, relatively.  */
      if(*int64_opt < -922337203685477)
        return;

      to->__li->QuadPart = *int64_opt * 10000;
      to->__since = GetTickCount64();
    }
    else
      to->__li->QuadPart = 0;
  }

__MCF_DLLEXPORT
void
__MCF_adjust_timeout_v2(__MCF_winnt_timeout* to)
  {
    /* Absolute timeouts need no adjustment.  */
    int64_t temp = to->__li->QuadPart;
    if(temp >= 0)
      return;

    /* Add the number of 100 nanoseconds that have elapsed so far, to the
     * timeout which is negative, using saturation arithmetic.  */
    uint64_t now = GetTickCount64();
    temp += (int64_t) (now - to->__since) * 10000;
    to->__li->QuadPart = temp & (temp >> 63);
    to->__since = now;
  }

__MCF_DLLEXPORT
size_t
__MCF_batch_release_common(const void* key, size_t count)
  {
    /* A call to `ExitProcess()` terminates all the other threads, even if
     * they are waiting. We don't release the keyed event in this case, as it
     * blocks the calling thread infinitely if there is no thread to wake up.
     * See <https://github.com/lhmouse/mcfgthread/issues/21>.  */
    if(RtlDllShutdownInProgress())
      return 0;

    for(size_t k = 0;  k != count;  ++k)
      __MCF_keyed_event_signal(key, NULL);  /* infinite timeout  */

    /* Return the number of threads that have been woken.  */
    return count;
  }
