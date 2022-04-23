/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "../src/cxa.h"
#include "../src/exit.h"
#include <assert.h>
#include <stdio.h>

static
void
atexit_second(void* ptr)
  {
    __MCF__Exit(*(int*) ptr);
  }

int
main(void)
  {
    static int value = 42;
    __MCF_cxa_atexit(atexit_second, &value, NULL);
    __MCF__Exit(0);
  }
