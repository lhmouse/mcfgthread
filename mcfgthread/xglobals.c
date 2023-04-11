/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_XGLOBALS_IMPORT  __MCF_DLLEXPORT
#define __MCF_XGLOBALS_INLINE  __MCF_DLLEXPORT
#define __MCF_XGLOBALS_READONLY
#include "xglobals.i"

__MCF_DLLEXPORT
EXCEPTION_DISPOSITION
__cdecl
__MCF_seh_top(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx)
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
    to->__i64 = INT64_MAX;

    /* If no timeout is given, wait indefinitely.  */
    if(!int64_opt)
      return;

    if(*int64_opt > 0) {
      /* If `*int64_opt` is positive, it denotes the number of milliseconds
       * since 1970-01-01T00:00:00Z, and has to be converted into the number of
       * 100 nanoseconds since the 1601-01-01T00:00:00Z.  */
      if(*int64_opt > 910692730085477)
        return;

      to->__since = 0;
      to->__i64 = (11644473600000 + *int64_opt) * 10000;
    }
    else if(*int64_opt < 0) {
      /* If `*int64_opt` is negative, it denotes the number of milliseconds
       * to wait, relatively.  */
      if(*int64_opt < -922337203685477)
        return;

      to->__since = GetTickCount64();
      to->__i64 = *int64_opt * 10000;
    }
    else
      to->__i64 = 0;
  }

__MCF_DLLEXPORT
void
__MCF_adjust_winnt_timeout_v2(__MCF_winnt_timeout* to)
  {
    /* Absolute timeouts need no adjustment.  */
    if(to->__i64 >= 0)
      return;

    /* Add the number of 100 nanoseconds that have elapsed so far, to the
     * timeout which is negative, using saturation arithmetic.  */
    uint64_t last_since = to->__since;
    to->__since = GetTickCount64();
    to->__i64 += (int64_t) (to->__since - last_since) * 10000;
    to->__i64 &= to->__i64 >> 63;
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
    __MCF_dtor_queue_finalize(__MCF_g->__cxa_at_quick_exit_queue, __MCF_g->__cxa_at_quick_exit_mtx, NULL);
  }

__MCF_DLLEXPORT
void
__MCF_run_dtors_atexit(void)
  {
    __MCF_dtor_queue_finalize(__MCF_g->__cxa_atexit_queue, __MCF_g->__cxa_atexit_mtx, NULL);
  }

__MCF_DLLEXPORT
void
__MCF_finalize_on_exit(void)
  {
    _MCF_thread* self = TlsGetValue(__MCF_g->__tls_index);
    if(!self)
      return __MCF_run_dtors_atexit();

    /* Call destructors for thread-local objects before static ones, in
     * accordance with the C++ standard. See [basic.start.term]/2.
     * Thread-local destructors are not called according to POSIX.  */
    __MCF_dtor_queue_finalize(self->__atexit_queue, NULL, NULL);
    __MCF_run_dtors_atexit();

    TlsSetValue(__MCF_g->__tls_index, NULL);
    _MCF_thread_drop_ref_nonnull(self);
  }

static inline
void
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

static
void
do_on_process_attach(void)
  {
    /* Initialize static global constants.  */
    GetSystemInfo(&__MCF_crt_sysinfo);

    __MCF_crt_heap = GetProcessHeap();
    __MCF_CHECK(__MCF_crt_heap);

    LARGE_INTEGER pfreq;
    __MCF_CHECK(QueryPerformanceFrequency(&pfreq));
    __MCF_crt_pf_recip = 1000 / (double) pfreq.QuadPart;

    /* Initialize dynamic global shared data.  */
    static WCHAR gnbuffer[] = L"Local\\__MCF_crt_xglobals_*?pid???_#?cookie????????";
    static UNICODE_STRING gname = { .Buffer = gnbuffer, .Length = sizeof(gnbuffer) - sizeof(WCHAR), .MaximumLength = sizeof(gnbuffer) };
    static OBJECT_ATTRIBUTES gattrs = { .Length = sizeof(OBJECT_ATTRIBUTES), .ObjectName = &gname, .Attributes = OBJ_OPENIF | OBJ_EXCLUSIVE };
    static LARGE_INTEGER gsize = { .QuadPart = sizeof(__MCF_crt_xglobals) };

    /* Generate the unique name for this process.  */
    DWORD pid = GetCurrentProcessId();
    UINT64 cookie = (UINT_PTR) EncodePointer((PVOID) ~(UINT_PTR) (pid * 0x100000001ULL)) * 0x9E3779B97F4A7C15ULL;

    __MCF_ASSERT(gnbuffer[25] == L'*');
    do_encode_numeric_field(gnbuffer + 25, 8, pid, L"0123456789ABCDEF");
    __MCF_ASSERT(gnbuffer[34] == L'#');
    do_encode_numeric_field(gnbuffer + 34, 16, cookie, L"GHJKLMNPQRSTUWXY");
    __MCF_ASSERT(gnbuffer[50] == 0);

    __MCF_CHECK_NT(BaseGetNamedObjectDirectory(&(gattrs.RootDirectory)));
    __MCF_ASSERT(gattrs.RootDirectory);

    /* Allocate or open storage for global data.
     * We are in the DLL main routine, so locking is unnecessary.  */
    HANDLE gfile;
    __MCF_CHECK_NT(NtCreateSection(&gfile, STANDARD_RIGHTS_REQUIRED | SECTION_MAP_READ | SECTION_MAP_WRITE, &gattrs, &gsize, PAGE_READWRITE, SEC_COMMIT, NULL));
    __MCF_ASSERT(gfile);

    /* Get a pointer to this named region. Unlike `CreateFileMappingW()`,
     * the view shall not be inherited by child processes.  */
    PVOID gmem_base = NULL;
    SIZE_T gmem_size = 0;
    __MCF_CHECK_NT(NtMapViewOfSection(gfile, GetCurrentProcess(), &gmem_base, 0, 0, NULL, &gmem_size, 2, 0, PAGE_READWRITE));
    __MCF_ASSERT(gmem_base);
    __MCF_ASSERT(gmem_size >= sizeof(__MCF_crt_xglobals));
    __MCF_g = gmem_base;

    if(__MCF_g->__self_ptr) {
      /* Reuse the existent region and close excess handles.  */
      __MCF_g = __MCF_g->__self_ptr;
      __MCF_CHECK(__MCF_g->__self_size >= sizeof(__MCF_crt_xglobals));

      NtUnmapViewOfSection(GetCurrentProcess(), gmem_base);
      __MCF_close_handle(gfile);
      return;
    }

    __MCF_g->__self_ptr = __MCF_g;
    __MCF_g->__self_size = sizeof(__MCF_crt_xglobals);

    /* Allocate a TLS slot for this library.  */
    __MCF_g->__tls_index = TlsAlloc();
    __MCF_CHECK(__MCF_g->__tls_index != UINT32_MAX);

    /* Attach the main thread. The structure should be all zeroes so no
     * initialization is necessary.  */
    __MCF_thread_attach_foreign(__MCF_g->__main_thread);
  }

static
void
do_on_thread_detach(void)
  {
    /* Ignore foreign threads.  */
    _MCF_thread* self = TlsGetValue(__MCF_g->__tls_index);
    if(!self)
      return;

    /* Per-thread atexit callbacks may use TLS, so call them before
     * destructors of thread-local objects.  */
    __MCF_dtor_queue_finalize(self->__atexit_queue, NULL, NULL);
    __MCF_tls_table_finalize(self->__tls_table);

    TlsSetValue(__MCF_g->__tls_index, NULL);
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
    /* Call the common routine. This will not fail.  */
    do_image_tls_callback(instance, reason, reserved);

    if(reason != DLL_PROCESS_ATTACH)
      return 1;

    /* Freeze the `.data` section.  */
    PVOID base = &__MCF_g;
    SIZE_T size = sizeof(__MCF_g);
    DWORD dummy;
    __MCF_CHECK_NT(NtProtectVirtualMemory(GetCurrentProcess(), &base, &size, PAGE_READONLY, &dummy));

    /* Prevent this DLL from being unloaded.  */
    __MCF_CHECK_NT(LdrAddRefDll(1, instance));
    return 1;
  }

#else  /* DLL_EXPORT  */

/* When building the static library, invoke the common routine from a TLS
 * callback. This requires the main executable be linked with 'tlssup.o'.
 * Such initialization should happen as early as possible.  */
extern const PIMAGE_TLS_CALLBACK __MCF_xl_b;

const PIMAGE_TLS_CALLBACK __MCF_xl_b __attribute__((__section__(".CRT$XLB"), __used__)) = do_image_tls_callback;

#endif  /* DLL_EXPORT  */

/* These are constants that have to be initialized at load time. The
 * initializers prevent them from being placed into the`.bss` section.  */
HANDLE __MCF_crt_heap = (void*) -1;
double __MCF_crt_pf_recip = 1;
SYSTEM_INFO __MCF_crt_sysinfo = { .dwPageSize = 1 };

/* This is a pointer to global data. If this library is linked statically,
 * all instances of this pointer in the same process should point to the
 * same memory. The initializer prevents it from being placed into the
 * `.bss` section.  */
__MCF_crt_xglobals* __MCF_g = (void*) -1;
