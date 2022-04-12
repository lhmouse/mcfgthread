// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_EXIT_C_  1
#include "exit.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "win32.h"

void
_exit(int status)
  __attribute__((__alias__("__MCF__Exit")));

void
_Exit(int status)
  __attribute__((__alias__("__MCF__Exit")));

int
at_quick_exit(_MCF_atexit_function* at_func)
  __attribute__((__alias__("__MCF_at_quick_exit")));

void
quick_exit(int status)
  __attribute__((__alias__("__MCF_quick_exit")));

void
__MCF__Exit(int status)
  {
    TerminateProcess(GetCurrentProcess(), (DWORD) status);
    __MCF_UNREACHABLE;
  }

int
__MCF_at_quick_exit(_MCF_atexit_function* at_func)
  {
    // Push the element to the global queue.
    _MCF_mutex_lock(&__MCF_cxa_at_quick_exit_mutex, NULL);
    __MCF_dtor_element elem = { (__MCF_cxa_dtor_cdecl*)(intptr_t) at_func, NULL, NULL };
    int err = __MCF_dtor_queue_push(&__MCF_cxa_at_quick_exit_queue, &elem);
    _MCF_mutex_unlock(&__MCF_cxa_at_quick_exit_mutex);
    return err;
  }

void
__MCF_quick_exit(int status)
  {
    __MCF_dtor_queue_finalize(&__MCF_cxa_at_quick_exit_queue, &__MCF_cxa_at_quick_exit_mutex, NULL);
    __MCF__Exit(status);
  }
