/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../mcfgthread/fwd.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int
main(void)
  {
    static const char env_name[] = "_MCF_RUNTIME_FAILURE_TEST_ERROR";

    fprintf(stderr,
            "To run this test, set the environment variable `%s` to a Windows error code.\n",
            env_name);

    const char* env_val = getenv(env_name);
    if(!env_val || !*env_val)
      return 77;  // skip

    SetLastError((DWORD) strtol(env_val, __MCF_nullptr, 10));
    __MCF_CHECK(false);
  }
