/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/fwd.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int
main(void)
  {
    static const char env_name[] = "_MCF_RUNTIME_FAILURE_TEST_ERROR";
    const char* env_val = getenv(env_name);
    if(!env_val || !*env_val) {
      fprintf(stderr,
              "To run this test, set the environment variable `%s` to a Windows error code.\n",
              env_name);
      return 77;  // skip
    }

    SetLastError((DWORD) strtol(env_val, NULL, 10));
    __MCF_CHECK(false);
  }
