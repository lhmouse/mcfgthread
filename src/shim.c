// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#define __MCFGTHREAD_SHIM_C_  1
#include "cxa.h"
#include "exit.h"

#ifdef DLL_EXPORT
#  error This file shall be compiled as a static library.
#endif

// This is defined forfunctions that imply a `__dso_handle` argument.
// See the Itanium C++ ABI for details.
char __dso_handle __attribute__((__weak__));

// `at_quick_exit`: The at_quick_exit function, ISO C
int
at_quick_exit(__MCF_atexit_callback* func);

int
at_quick_exit(__MCF_atexit_callback* func)
  {
    return __MCF_cxa_at_quick_exit((__MCF_cxa_dtor_cdecl*)(intptr_t) func, NULL, &__dso_handle);
  }

// `atexit`: The atexit function, ISO C
int
atexit(__MCF_atexit_callback* func);

int
atexit(__MCF_atexit_callback* func)
  {
    return __MCF_cxa_atexit((__MCF_cxa_dtor_cdecl*)(intptr_t) func, NULL, &__dso_handle);
  }

// `_Exit`: The _Exit function, ISO C
void
_Exit(int status)
  __attribute__((__noreturn__));

void
_Exit(int status)
  {
    __MCF__Exit(status);
  }

// `_exit`: POSIX
void
_exit(int status)
  __attribute__((__noreturn__));

void
_exit(int status)
  {
    __MCF__Exit(status);
  }

// `quick_exit`: the quick_exit function, ISO C
void
quick_exit(int status)
  __attribute__((__noreturn__));

void
quick_exit(int status)
  {
    __MCF_quick_exit(status);
  }

// `exit`: the exit function, ISO C
void
exit(int status)
  __attribute__((__noreturn__));

void
exit(int status)
  {
    __MCF_exit(status);
  }

// `__cxa_guard_{acquire,release,abort}`: One-time Construction API, Itanium ABI
int
__cxa_guard_acquire(int64_t* guard);

int
__cxa_guard_acquire(int64_t* guard)
  {
    return __MCF_cxa_guard_acquire(guard);
  }

void
__cxa_guard_release(int64_t* guard);

void
__cxa_guard_release(int64_t* guard)
  {
    __MCF_cxa_guard_release(guard);
  }

void
__cxa_guard_abort(int64_t* guard);

void
__cxa_guard_abort(int64_t* guard)
  {
    __MCF_cxa_guard_abort(guard);
  }

// `__cxa_at_quick_exit`: GNU C library extension
int
__cxa_at_quick_exit(__MCF_cxa_dtor_union dtor, void* this, void* dso);

int
__cxa_at_quick_exit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    (void) dso;
    return __MCF_cxa_at_quick_exit(dtor, this, &__dso_handle);
  }

// `__cxa_atexit`: Runtime API, Itanium ABI
int
__cxa_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso);

int
__cxa_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    (void) dso;
    return __MCF_cxa_atexit(dtor, this, &__dso_handle);
  }

// `__cxa_thread_atexit`: GCC libstdc++ extension
int
__cxa_thread_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso);

int
__cxa_thread_atexit(__MCF_cxa_dtor_union dtor, void* this, void* dso)
  {
    (void) dso;
    return __MCF_cxa_thread_atexit(dtor, this, &__dso_handle);
  }

// `__cxa_finalize`: Runtime API, Itanium ABI
void
__cxa_finalize(void* dso);

void
__cxa_finalize(void* dso)
  {
    __MCF_cxa_finalize(dso);
  }
