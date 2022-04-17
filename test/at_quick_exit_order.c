// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#include "../src/cxa.h"
#include "../src/exit.h"
#include <assert.h>
#include <stdio.h>
#include <windows.h>

static void
atexit_second(void* ptr)
  {
    __MCF__Exit(*(int*) ptr);
  }

static void
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
  }
