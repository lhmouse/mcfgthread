// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_EXIT_C_  1
#include "exit.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "win32.h"

int
at_quick_exit(_MCF_atexit_function* at_func)
  __attribute__((__alias__("__MCF_at_quick_exit")));

void
_exit(int status)
  __attribute__((__alias__("__MCF__Exit")));

void
_Exit(int status)
  __attribute__((__alias__("__MCF__Exit")));

void
quick_exit(int status)
  __attribute__((__alias__("__MCF_quick_exit")));

int
__MCF_at_quick_exit(_MCF_atexit_function* at_func)
  {
    // Push the element to the global queue.
    _MCF_mutex_lock(&__MCF_at_quick_exit_mutex, NULL);
    __MCF_dtor_element elem = { (__MCF_dtor_generic*) at_func, NULL, NULL };
    int err = __MCF_dtor_queue_push(&__MCF_at_quick_exit_queue, &elem);
    _MCF_mutex_unlock(&__MCF_at_quick_exit_mutex);
    return err;
  }

static int
do_pop_at_quick_exit(__MCF_dtor_element* elem)
  {
    // Try popping an element.
    _MCF_mutex_lock(&__MCF_at_quick_exit_mutex, NULL);
    int err = __MCF_dtor_queue_pop(elem, &__MCF_at_quick_exit_queue, NULL);
    _MCF_mutex_unlock(&__MCF_at_quick_exit_mutex);
    return err;
  }

void
__MCF__Exit(int status)
  {
    TerminateProcess(GetCurrentProcess(), (DWORD) status);
    __MCF_UNREACHABLE;
  }

void
__MCF_quick_exit(int status)
  {
    __MCF_dtor_element elem;
    while(do_pop_at_quick_exit(&elem) == 0)
      __MCF_dtor_element_execute(&elem);

    __MCF__Exit(status);
  }
