/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/exit.h"
#include <assert.h>
#include <stdio.h>

static
void
atexit_second(void* ptr)
  {
    __MCF__Exit(*(int*) ptr);
  }

static
void
atexit_first(void* ptr)
  {
    *(int*) ptr = 0;
  }

int
main(void)
  {
    static int value = 42;
    __MCF_cxa_at_quick_exit(atexit_second, &value, NULL);
    __MCF_cxa_at_quick_exit(atexit_first, &value, NULL);
    __MCF_quick_exit(1);
  }
