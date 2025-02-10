/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

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
    __MCF_cxa_at_quick_exit(atexit_second, &value, __MCF_nullptr);
    __MCF_cxa_at_quick_exit(atexit_first, &value, __MCF_nullptr);
    __MCF_quick_exit(1);
  }
