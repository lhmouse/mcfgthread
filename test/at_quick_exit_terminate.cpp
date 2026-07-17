/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/exit.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <exception>

int
main(void)
  {
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
