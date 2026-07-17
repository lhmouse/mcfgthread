/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#define WIN32_LEAN_AND_MEAN  1
#include <windows.h>
#include "../mcfgthread/exit.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

static __MCF_NEVER_RETURN
LONG
__stdcall
unhandled(EXCEPTION_POINTERS* except)
  {
    (void) except;
    __MCF__Exit(0);
  }

static __MCF_NEVER_INLINE
void
test_unhandled(void)
  {
    fprintf(stderr, "raise exception 2\n");
    RaiseException(0x20474343U, 0, 0, NULL);  // terminate
    fprintf(stderr, "continue 2\n");
    __MCF__Exit(41);
  }

static
DWORD
__stdcall
thread_proc(LPVOID param)
  {
    (void) param;
    test_unhandled();
    assert(false);
    return 1;
  }

int
main(void)
  {
    SetUnhandledExceptionFilter(unhandled);
    HANDLE thrd = CreateThread(NULL, 0, thread_proc, NULL, 0, NULL);
    assert(thrd);
    WaitForSingleObject(thrd, INFINITE);
    assert(false);
  }
