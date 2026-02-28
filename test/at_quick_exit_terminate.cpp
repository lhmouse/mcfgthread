/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/exit.h"
#include <assert.h>
#include <stdio.h>
#include <exception>

int
main(void)
  {
#ifndef __MINGW32__
    return 77;
#endif

    ::std::set_terminate(
      +[] {
        ::fprintf(stderr, "terminating\n");
        ::__MCF__Exit(0);
      });

    ::__MCF_cxa_at_quick_exit(
      +[](void* ptr) {
        ::fprintf(stderr, "throwing %p\n", ptr);
        throw ptr;
      },
      (void*) 42,
      nullptr);

    try {
      ::__MCF_quick_exit(1);
      ::fprintf(stderr, "unreachable\n");
    }
    catch(void* ptr) {
      ::fprintf(stderr, "caught %p\n", ptr);
      return 1;
    }
  }
