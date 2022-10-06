/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_XGLOBALS(...)  __MCF_DLLEXPORT
#include "xglobals.i"
#include "thread.h"
#include "mutex.h"
#include "cond.h"
#include "dtor_queue.h"

__MCF_DLLEXPORT
EXCEPTION_DISPOSITION
__cdecl
__MCF_seh_top(EXCEPTION_RECORD* rec, void* estab_frame, CONTEXT* ctx, void* disp_ctx)
  {
    (void) estab_frame;
    (void) ctx;
    (void) disp_ctx;

    /* Check for uncaught C++ exceptions.  */
    DWORD r = rec->ExceptionFlags & EXCEPTION_NONCONTINUABLE;
    r |= (rec->ExceptionCode & 0x20FFFFFFU) - 0x20474343U;  /* (1 << 29) | 'GCC'  */
    return r ? ExceptionContinueSearch : ExceptionContinueExecution;
  }

__MCF_DLLEXPORT
void
__MCF_initialize_winnt_timeout_v2(__MCF_winnt_timeout* to, const int64_t* int64_opt)
  {
    /* Initialize it to an infinite value.  */
    to->__li->QuadPart = INT64_MAX;

    /* If no timeout is given, wait indefinitely.  */
    if(!int64_opt)
      return;

    if(*int64_opt > 0) {
      /* If `*int64_opt` is positive, it denotes the number of milliseconds
       * since 1970-01-01T00:00:00Z, and has to be converted into the number of
       * 100 nanoseconds since the 1601-01-01T00:00:00Z.  */
      if(*int64_opt > 910692730085477)
        return;

      to->__li->QuadPart = (11644473600000 + *int64_opt) * 10000;
      to->__since = 0;
    }
    else if(*int64_opt < 0) {
      /* If `*int64_opt` is negative, it denotes the number of milliseconds
       * to wait, relatively.  */
      if(*int64_opt < -922337203685477)
        return;

      to->__li->QuadPart = *int64_opt * 10000;
      to->__since = GetTickCount64();
    }
    else
      to->__li->QuadPart = 0;
  }

__MCF_DLLEXPORT
void
__MCF_adjust_winnt_timeout_v2(__MCF_winnt_timeout* to)
  {
    /* Absolute timeouts need no adjustment.  */
    int64_t temp = to->__li->QuadPart;
    if(temp >= 0)
      return;

    /* Add the number of 100 nanoseconds that have elapsed so far, to the
     * timeout which is negative, using saturation arithmetic.  */
    uint64_t now = GetTickCount64();
    temp += (int64_t) (now - to->__since) * 10000;
    to->__li->QuadPart = temp & (temp >> 63);
    to->__since = now;
  }

__MCF_DLLEXPORT
size_t
__MCF_batch_release_common(const void* key, size_t count)
  {
    /* A call to `ExitProcess()` terminates all the other threads, even if
     * they are waiting. We don't release the keyed event in this case, as it
     * blocks the calling thread infinitely if there is no thread to wake up.
     * See <https://github.com/lhmouse/mcfgthread/issues/21>.  */
    if(RtlDllShutdownInProgress())
      return 0;

    for(size_t k = 0;  k != count;  ++k)
      __MCF_keyed_event_signal(key, NULL);  /* infinite timeout  */

    /* Return the number of threads that have been woken.  */
    return count;
  }

__MCF_DLLEXPORT
int
__MCF_win32_error_i(DWORD code, int val)
  {
    SetLastError(code);
    return val;
  }

__MCF_DLLEXPORT
void*
__MCF_win32_error_p(DWORD code, void* ptr)
  {
    SetLastError(code);
    return ptr;
  }

__MCF_DLLEXPORT
void
__MCF_run_dtors_at_quick_exit(void)
  {
    __MCF_dtor_queue_finalize(&__MCF_cxa_at_quick_exit_queue, &__MCF_cxa_at_quick_exit_mutex, NULL);
  }

__MCF_DLLEXPORT
void
__MCF_run_dtors_atexit(void)
  {
    __MCF_dtor_queue_finalize(&__MCF_cxa_atexit_queue, &__MCF_cxa_atexit_mutex, NULL);
  }

__MCF_DLLEXPORT
void
__MCF_finalize_on_exit(void)
  {
    _MCF_thread* self = TlsGetValue(__MCF_win32_tls_index);
    if(!self)
      return __MCF_run_dtors_atexit();

    /* Call destructors for thread-local objects before static ones, in
     * accordance with the C++ standard. See [basic.start.term]/2.
     * Thread-local destructors are not called according to POSIX.  */
    __MCF_dtor_queue_finalize(&(self->__atexit_queue), NULL, NULL);
    __MCF_run_dtors_atexit();

    TlsSetValue(__MCF_win32_tls_index, NULL);
    _MCF_thread_drop_ref_nonnull(self);
  }

/* Define global data last to ensure that all of them have all been
 * forward-declared properly.  */
DWORD __MCF_win32_tls_index;
double __MCF_perf_frequency_reciprocal;
_MCF_thread __MCF_main_thread;

_MCF_mutex __MCF_cxa_atexit_mutex;
__MCF_dtor_queue __MCF_cxa_atexit_queue;
_MCF_mutex __MCF_cxa_at_quick_exit_mutex;
__MCF_dtor_queue __MCF_cxa_at_quick_exit_queue;
_MCF_cond __MCF_interrupt_cond;
BYTE __MCF_mutex_spin_field[];

static
void
do_on_process_attach(void)
  {
    /* Allocate a TLS slot for this library.  */
    __MCF_win32_tls_index = TlsAlloc();
    __MCF_CHECK(__MCF_win32_tls_index != UINT32_MAX);

    /* Get the performance counter resolution.  */
    LARGE_INTEGER freq;
    __MCF_CHECK(QueryPerformanceFrequency(&freq));
    __MCF_perf_frequency_reciprocal = 1000 / (double) freq.QuadPart;

    /* Attach the main thread.  */
    __MCF_main_thread.__tid = _MCF_thread_self_tid();
    __MCF_CHECK_NT(NtDuplicateObject(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &(__MCF_main_thread.__handle), 0, 0, DUPLICATE_SAME_ACCESS));
    __MCF_CHECK(__MCF_main_thread.__handle);
    _MCF_atomic_store_32_rel(__MCF_main_thread.__nref, 1);
    __MCF_CHECK(TlsSetValue(__MCF_win32_tls_index, &__MCF_main_thread));
  }

static
void
do_on_thread_detach(void)
  {
    /* Ignore foreign threads.  */
    _MCF_thread* self = TlsGetValue(__MCF_win32_tls_index);
    if(!self)
      return;

    /* Per-thread atexit callbacks may use TLS, so call them before
     * destructors of thread-local objects.  */
    __MCF_dtor_queue_finalize(&(self->__atexit_queue), NULL, NULL);
    __MCF_tls_table_finalize(&(self->__tls_table));

    TlsSetValue(__MCF_win32_tls_index, NULL);
    _MCF_thread_drop_ref_nonnull(self);
  }

static
void
__stdcall
do_image_tls_callback(PVOID module, DWORD reason, LPVOID reserved)
  {
    (void) module;
    (void) reserved;

    /* Perform global initialization and per-thread cleanup, as needed.
     * Note, upon `DLL_PROCESS_DETACH` we don't perform any cleanup, because
     * other DLLs might have been unloaded and we would be referencing unmapped
     * memory. User code should call `__cxa_finalize(NULL)` before exiting from
     * a process.  */
    if(reason == DLL_PROCESS_ATTACH)
      do_on_process_attach();
    else if(reason == DLL_THREAD_DETACH)
      do_on_thread_detach();
  }

#ifdef DLL_EXPORT

/* When building the shared library, invoke the common routine from the DLL
 * entry point callback. The decorated name is fabricated such that it remains
 * the same on both x86 and x86-64.  */
int
__stdcall
__MCF_dll_startup(PVOID instance, DWORD reason, PVOID reserved)
  __asm__("__MCF_dll_startup@@Z");

int
__stdcall
__MCF_dll_startup(PVOID instance, DWORD reason, PVOID reserved)
  {
    /* Prevent this DLL from being unloaded.  */
    if(reason == DLL_PROCESS_ATTACH)
      __MCF_CHECK_NT(LdrAddRefDll(1, instance));

    /* Call the common routine. This will not fail.  */
    do_image_tls_callback(instance, reason, reserved);
    return 1;
  }

#else  /* DLL_EXPORT  */

/* When building the static library, invoke the common routine from a TLS
 * callback. This requires the main executable be linked with 'tlssup.o'.
 * `__xl_d` is being used by mingw-w64, so we use `__xl_e` here.  */
extern const PIMAGE_TLS_CALLBACK __MCF_xl_e;

const PIMAGE_TLS_CALLBACK __MCF_xl_e
  __attribute__((__section__(".CRT$XLE"), __used__)) = do_image_tls_callback;

#endif  /* DLL_EXPORT  */
