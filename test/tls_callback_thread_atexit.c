/* This file is licensed under CC0 for illustrative purposes. You can
 * do whatever you like with this piece of code. Any warranty, explicit
 * or implicit, is disclaimed.  */

#include "../mcfgthread/cxa.h"
#include "../mcfgthread/thread.h"
#undef NDEBUG
#include <assert.h>
#include <windows.h>
#include <stdio.h>

static _MCF_tls_key* tls_key;
static int thread_atexit_proc_executed;
static int thread_tls_dtor_executed;

static
void
thread_atexit_proc(void* arg)
  {
    (void) arg;
    thread_atexit_proc_executed = 12345;
  }

static
void
thread_tls_dtor(void* arg)
  {
    (void) arg;
    thread_tls_dtor_executed = 12345;
  }

static
void
thread_proc(_MCF_thread* self)
  {
    _MCF_sleep_noninterruptible(&(int64_t){ -1001 });
    fprintf(stderr, "thread %d quitting\n", self->__tid);
  }

static
void
__stdcall
do_tls_callback(PVOID module, ULONG reason, PVOID reserved)
  {
    (void) module;
    (void) reserved;

    if(reason == DLL_THREAD_ATTACH) {
      assert(__MCF_cxa_thread_atexit(thread_atexit_proc, NULL, NULL) == 0);
      assert(_MCF_tls_set(tls_key, (void*) -1) == 0);
    }
  }

extern const IMAGE_TLS_DIRECTORY _tls_used;
static __MCF_SECTION(".rdata") const LPCVOID refptr_tls_used = &_tls_used;
static __MCF_SECTION(".CRT$XLY") const PIMAGE_TLS_CALLBACK test_xly = do_tls_callback;

#if defined __CYGWIN__
/** The Cygwin/MSYS2 runtime does not provide a TLS directory, so a local one has
 * to be defined. Although Cygwin/MSYS2 executables do not use native TLS, this
 * facility is fully functional and might be useful in the future.  */
DWORD _tls_index = UINT32_MAX;
__MCF_SECTION(".tls") PCHAR _tls_start = NULL;
__MCF_SECTION(".tls$ZZZ") PCHAR _tls_end = NULL;
__MCF_SECTION(".CRT$XLA") const PIMAGE_TLS_CALLBACK __xl_a = NULL;
__MCF_SECTION(".CRT$XLZ") const PIMAGE_TLS_CALLBACK __xl_z = NULL;

__MCF_SECTION(".rdata") const IMAGE_TLS_DIRECTORY _tls_used =
  {
    .AddressOfIndex = (ULONG_PTR) &_tls_index,
    .StartAddressOfRawData = (ULONG_PTR) &_tls_start,
    .EndAddressOfRawData = (ULONG_PTR) &_tls_end,
    .AddressOfCallBacks = (ULONG_PTR) (&__xl_a + 1),
  };
#endif

int
main(void)
  {
    tls_key = _MCF_tls_key_new(thread_tls_dtor);
    assert(tls_key);

    _MCF_thread* thrd = _MCF_thread_new(thread_proc, NULL, sizeof(int));
    assert(thrd);

    fprintf(stderr, "main waiting\n");
    _MCF_thread_wait(thrd, NULL);
    fprintf(stderr, "main wait finished\n");

    assert(thread_atexit_proc_executed == 12345);
    assert(thread_tls_dtor_executed == 12345);
  }
