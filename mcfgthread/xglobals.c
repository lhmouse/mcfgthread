/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_XGLOBALS_IMPORT  __MCF_DLLEXPORT
#define __MCF_XGLOBALS_INLINE  __MCF_DLLEXPORT
#define __MCF_XGLOBALS_READONLY
#include "xglobals.h"

__MCF_DLLEXPORT __attribute__((__used__))
EXCEPTION_DISPOSITION
__cdecl
__MCF_seh_top(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx)
  {
    (void) estab_frame;
    (void) ctx;
    (void) disp_ctx;

    /* Check for uncaught C++ exceptions.  */
    ULONG r = rec->ExceptionFlags & EXCEPTION_NONCONTINUABLE;
    r |= (rec->ExceptionCode & 0x20FFFFFFU) - 0x20474343U;  /* (1 << 29) | 'GCC'  */
    return r ? ExceptionContinueSearch : ExceptionContinueExecution;
  }

__MCF_DLLEXPORT
void
__MCF_initialize_winnt_timeout_v3(__MCF_winnt_timeout* to, const int64_t* ms_opt)
  {
    /* Initialize it to an infinite value.  */
    to->__li.QuadPart = INT64_MAX;
    to->__since = 0;

    /* If no timeout is given, wait indefinitely.  */
    if(!ms_opt)
      return;

    if(*ms_opt > 0) {
      /* If `*ms_opt` is positive, it denotes the number of milliseconds since
       * 1970-01-01T00:00:00Z, and has to be converted into the number of 100
       * nanoseconds since the 1601-01-01T00:00:00Z.  */
      if(*ms_opt > 910692730085477)
        return;

      to->__li.QuadPart = (11644473600000 + *ms_opt) * 10000;
    }
    else if(*ms_opt < 0) {
      /* If `*ms_opt` is negative, it denotes the number of milliseconds to
       * wait, relatively.  */
      if(*ms_opt < -922337203685477)
        return;

      to->__li.QuadPart = *ms_opt * 10000;
      QueryUnbiasedInterruptTime(&(to->__since));
    }
    else
      to->__li.QuadPart = 0;
  }

__MCF_DLLEXPORT
void
__MCF_adjust_winnt_timeout_v3(__MCF_winnt_timeout* to)
  {
    /* Absolute timeouts need no adjustment.  */
    if(to->__li.QuadPart >= 0)
      return;

    /* Add the number of 100 nanoseconds that have elapsed so far, to the
     * timeout which is negative, using saturation arithmetic.  */
    ULONGLONG old_since = to->__since;
    QueryUnbiasedInterruptTime(&(to->__since));
    to->__li.QuadPart += (LONGLONG) (to->__since - old_since);
    to->__li.QuadPart &= to->__li.QuadPart >> 63;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
size_t
__MCF_batch_release_common(const void* key, size_t count)
  {
    /* A call to `ExitProcess()` terminates all the other threads, even if
     * they are waiting. We don't release the keyed event in this case, as it
     * blocks the calling thread infinitely if there is no thread to wake up.
     * See <https://github.com/lhmouse/mcfgthread/issues/21>.  */
    if(__MCF_is_process_shutting_down())
      return 0;

    for(size_t k = 0;  k != count;  ++k)
      __MCF_keyed_event_signal(key, __MCF_nullptr);  /* infinite timeout  */

    /* Return the number of threads that have been woken.  */
    return count;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
int
__MCF_win32_error_i(ULONG code, int val)
  {
    SetLastError(code);
    return val;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void*
__MCF_win32_error_p(ULONG code, void* ptr)
  {
    SetLastError(code);
    return ptr;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void*
__MCF_win32_ntstatus_p(NTSTATUS status, void* ptr)
  {
    SetLastError(RtlNtStatusToDosError(status));
    return ptr;
  }

static
int
__fastcall
do_pop_static_dtor(__MCF_dtor_element* elem, _MCF_mutex* mtx, __MCF_dtor_queue* queue, void* dso)
  {
    _MCF_mutex_lock(mtx, __MCF_nullptr);
    int err = __MCF_dtor_queue_pop(elem, queue, dso);
    _MCF_mutex_unlock(mtx);
    return err;
  }

__MCF_DLLEXPORT
void
__MCF_run_static_dtors(_MCF_mutex* mtx, __MCF_dtor_queue* queue, void* dso)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    __MCF_dtor_element elem;

    while(do_pop_static_dtor(&elem, mtx, queue, dso) == 0)
      __MCF_invoke_cxa_dtor(elem.__dtor, elem.__this);
  }

static
void
__fastcall
do_encode_numeric_field(wchar_t* ptr, size_t width, uint64_t value, const wchar_t* digits)
  {
    wchar_t* eptr = ptr + width;
    uint64_t reg = value;

    /* Write significant figures backwards.  */
    while((ptr != eptr) && (reg != 0)) {
      uint32_t d = reg & 0x0F;
      reg >>= 4;
      *--eptr = digits[d];
    }

    /* Fill the rest with zeroes.  */
    while(ptr != eptr)
      *--eptr = digits[0];
  }

__MCF_DLLEXPORT
void
__MCF_gthread_initialize_globals(void)
  {
    /* Initialize static global constants.  */
    GetSystemInfo(&__MCF_crt_sysinfo);

    __MCF_crt_heap = GetProcessHeap();
    __MCF_CHECK(__MCF_crt_heap);

    LARGE_INTEGER pfreq;
    __MCF_CHECK(QueryPerformanceFrequency(&pfreq));
    __MCF_crt_pf_recip = 1000 / (double) pfreq.QuadPart;

    __MCF_CHECK(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"KERNELBASE.DLL", &__MCF_crt_kernelbase));
    __MCF_CHECK(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"NTDLL.DLL", &__MCF_crt_ntdll));

    /* Generate the unique name for this process.  */
    static WCHAR gnbuffer[] = L"Local\\__MCF_crt_xglobals_*?pid???_#?cookie????????";
    UNICODE_STRING gname = RTL_CONSTANT_STRING(gnbuffer);
    __MCF_ASSERT(gnbuffer[25] == L'*');
    ULONG pid = GetCurrentProcessId();
    do_encode_numeric_field(gnbuffer + 25, 8, pid, L"0123456789ABCDEF");
    __MCF_ASSERT(gnbuffer[34] == L'#');
    UINT64 cookie = (UINT_PTR) EncodePointer((PVOID) ~(UINT_PTR) (pid * 0x100000001ULL)) * 0x9E3779B97F4A7C15ULL;
    do_encode_numeric_field(gnbuffer + 34, 16, cookie, L"GHJKLMNPQRSTUWXY");
    __MCF_ASSERT(gnbuffer[50] == 0);

    /* Allocate or open storage for global data. We are in the DLL main routine,
     * so locking is not necessary.  */
    OBJECT_ATTRIBUTES gattrs;
    HANDLE gdir = __MCF_get_directory_for_named_objects();
    __MCF_ASSERT(gdir);
    InitializeObjectAttributes(&gattrs, &gname, OBJ_OPENIF | OBJ_EXCLUSIVE, gdir, __MCF_nullptr);
    HANDLE gfile = __MCF_create_named_section(&gattrs, sizeof(__MCF_crt_xglobals));
    __MCF_CHECK(gfile);

    /* Get a pointer to this named region. Unlike `CreateFileMappingW()`, the
     * view shall not be inherited by child processes.  */
    void* gmem_base = __MCF_nullptr;
    size_t gmem_size = 0;
    __MCF_map_view_of_section(gfile, &gmem_base, &gmem_size, false);
    __MCF_ASSERT(gmem_base);
    __MCF_g = gmem_base;

    if(__MCF_g->__self_ptr) {
      /* Reuse the existent region and close excess handles.  */
      __MCF_g = __MCF_g->__self_ptr;
      __MCF_unmap_view_of_section(gmem_base);
      __MCF_close_handle(gfile);
      return;
    }

    /* The region is new, so initialize it.  */
    __MCF_CHECK(gmem_size >= sizeof(__MCF_crt_xglobals));
    __MCF_g->__self_ptr = __MCF_g;
    __MCF_g->__self_size = sizeof(__MCF_crt_xglobals);

    /* Allocate a TLS slot for this library.  */
    __MCF_g->__tls_index = TlsAlloc();
    __MCF_CHECK(__MCF_g->__tls_index != UINT32_MAX);

    /* Perform lazy binding for newer functions.  */
    __MCF_G_INITIALIZE_LAZY(__MCF_crt_kernelbase, GetSystemTimePreciseAsFileTime);  /* win8 */
    __MCF_G_INITIALIZE_LAZY(__MCF_crt_kernelbase, QueryInterruptTime);  /* win10 */

    /* Attach the main thread. The structure should be all zeroes so no
     * initialization is necessary.  */
    __MCF_thread_attach_foreign(__MCF_g->__main_thread);
  }

__MCF_DLLEXPORT
void
__MCF_gthread_on_thread_exit(void)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    __MCF_dtor_element elem;
    __MCF_tls_table tls;

    _MCF_thread* self = TlsGetValue(__MCF_g->__tls_index);
    if(!self)
      return;

    /* When there are multiple statically linked copies, this function can
     * be called multiple times upon a thread's exit. For subsequent calls,
     * the thread control structure may have been deallocated, so don't do
     * anything.  */
    if(self == __MCF_BAD_PTR)
      return;

    /* Per-thread atexit callbacks may use TLS, so call them before
     * destructors of thread-local objects.  */
    while(__MCF_dtor_queue_pop(&elem, self->__atexit_queue, __MCF_nullptr) == 0)
      __MCF_invoke_cxa_dtor(elem.__dtor, elem.__this);

    /* Call destructors of TLS keys. The TLS table may be modified by
     * destructors, so swap it out first.  */
    while(self->__tls_table->__begin) {
      __MCF_mcopy(&tls, self->__tls_table, sizeof(__MCF_tls_table));
      __MCF_mzero(self->__tls_table, sizeof(__MCF_tls_table));

      while(tls.__begin != tls.__end) {
        tls.__end --;

        /* Skip empty buckets.  */
        _MCF_tls_key* tkey = tls.__end->__key_opt;
        if(!tkey)
          continue;

        /* Call the destructor only if the value is not a null pointer, as
         * per POSIX.  */
        if(!_MCF_atomic_load_8_rlx(tkey->__deleted) && tkey->__dtor_opt && tls.__end->__value_opt)
          __MCF_invoke_cxa_dtor(tkey->__dtor_opt, tls.__end->__value_opt);

        /* Deallocate the key.  */
        _MCF_tls_key_drop_ref_nonnull(tkey);
      }

      /* Deallocate the table which should be empty now.  */
      __MCF_mfree(tls.__begin);
    }

    /* Poison this value.  */
    TlsSetValue(__MCF_g->__tls_index, __MCF_BAD_PTR);
    _MCF_thread_drop_ref_nonnull(self);
  }

#ifdef __MCF_BUILDING_DLL

/* Declare the DLL entry point function. This has the same signature as
 * `DllMain()`.The decorated name is fabricated such that is remains the
 * same on both x86 and x86-64.  */
int
__stdcall
__MCF_dll_startup(PVOID instance, ULONG reason, PVOID reserved)
  __asm__("__MCF_dll_startup@@Z");

/* When building the shared library, invoke common routines from the DLL
 * entry point callback.  */
__MCF_REALIGN_SP
int
__stdcall
__MCF_dll_startup(PVOID instance, ULONG reason, PVOID reserved)
  {
    ULONG dummy1;
    HMODULE dummy2 = reserved;

    switch(reason)
      {
      case DLL_PROCESS_ATTACH:
        __MCF_gthread_initialize_globals();
        __MCF_CHECK(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, instance, &dummy2));
        __MCF_CHECK(VirtualProtect(&__MCF_g, sizeof(__MCF_g), PAGE_READONLY, &dummy1));
        break;

      case DLL_THREAD_DETACH:
        __MCF_gthread_on_thread_exit();
        break;
      }

    return 1;
  }

#else  /* __MCF_BUILDING_DLL  */

/* When building the static library, invoke common routines from a TLS
 * callback.  */
static __MCF_REALIGN_SP
void
__stdcall
__MCF_tls_callback(PVOID module, ULONG reason, LPVOID reserved)
  {
    (void) module;
    (void) reserved;

    switch(reason)
      {
      case DLL_PROCESS_ATTACH:
        __MCF_gthread_initialize_globals();
        break;

      case DLL_THREAD_DETACH:
        __MCF_gthread_on_thread_exit();
        break;
      }
  }

/* This requires the main executable be linked with 'tlssup.o'. Such
 * initialization shall happen as early as possible.  */
static PIMAGE_TLS_CALLBACK __MCF__xl_b __MCF__CRT_ALLOC(".CRT$XLB") = __MCF_tls_callback;

#endif  /* __MCF_BUILDING_DLL  */

/* These are constants that have to be initialized at load time. The
 * initializers prevent them from being placed into the`.bss` section.  */
HANDLE __MCF_crt_heap = __MCF_BAD_PTR;
double __MCF_crt_pf_recip = 1;
SYSTEM_INFO __MCF_crt_sysinfo = { .dwPageSize = 1 };
HMODULE __MCF_crt_kernelbase = __MCF_BAD_PTR;
HMODULE __MCF_crt_ntdll = __MCF_BAD_PTR;

/* This is a pointer to global data. If this library is linked statically,
 * all instances of this pointer in the same process should point to the
 * same memory. The initializer prevents it from being placed into the
 * `.bss` section.  */
__MCF_crt_xglobals* __MCF_g = __MCF_BAD_PTR;
