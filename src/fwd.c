/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DYNCONST
#include "fwd.h"
#include "thread.h"
#include "mutex.h"
#include "dtor_queue.h"
#include "xwin32.i"

uint32_t __MCF_win32_tls_index;
double __MCF_perf_frequency_reciprocal;
_MCF_thread __MCF_main_thread;

_MCF_mutex __MCF_cxa_atexit_mutex;
__MCF_dtor_queue __MCF_cxa_atexit_queue;
_MCF_mutex __MCF_cxa_at_quick_exit_mutex;
__MCF_dtor_queue __MCF_cxa_at_quick_exit_queue;
uint8_t __MCF_mutex_spin_field[];

__MCF_DLLEXPORT
uint32_t
_MCF_get_win32_error(void)
  {
    return GetLastError();
  }

__MCF_DLLEXPORT
int
__MCF_win32_error_i(uint32_t code, int val)
  {
    SetLastError(code);
    return val;
  }

__MCF_DLLEXPORT
void*
__MCF_win32_error_p(uint32_t code, void* ptr)
  {
    SetLastError(code);
    return ptr;
  }

__MCF_DLLEXPORT
void
__MCF_dll_callback_on_process_attach(void)
  {
    /* Allocate a TLS slot for this library.  */
    __MCF_win32_tls_index = TlsAlloc();
    __MCFGTHREAD_CHECK(__MCF_win32_tls_index != TLS_OUT_OF_INDEXES);

    /* Get the performance counter resolution.  */
    LARGE_INTEGER freq;
    __MCFGTHREAD_CHECK(QueryPerformanceFrequency(&freq));
    __MCF_perf_frequency_reciprocal = 1000 / (double) freq.QuadPart;

    /* Attach the main thread.  */
    __MCF_main_thread.__tid = GetCurrentThreadId();
    __MCFGTHREAD_CHECK(NT_SUCCESS(NtDuplicateObject(GetCurrentProcess(),
          GetCurrentThread(), GetCurrentProcess(), &(__MCF_main_thread.__handle),
          0, 0, DUPLICATE_SAME_ACCESS)));

    __MCFGTHREAD_CHECK(__MCF_main_thread.__handle);
    _MCF_atomic_store_32_rel(__MCF_main_thread.__nref, 1);
    __MCFGTHREAD_CHECK(TlsSetValue(__MCF_win32_tls_index, &__MCF_main_thread));
  }

__MCF_DLLEXPORT
void
__MCF_dll_callback_on_thread_detach(void)
  {
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
