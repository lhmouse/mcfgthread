/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#include "precompiled.i"
#define __MCF_DECLSPEC_XGLOBALS_IMPORT  __MCF_DLLEXPORT
#define __MCF_DECLSPEC_XGLOBALS_INLINE  __MCF_DLLEXPORT
#define __MCF_DECLSPEC_XGLOBALS_CONST
#include "xglobals.i"

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
    __MCF_dtor_queue_finalize(__MCF_g->__cxa_at_quick_exit_queue, __MCF_g->__cxa_at_quick_exit_mutex, NULL);
  }

__MCF_DLLEXPORT
void
__MCF_run_dtors_atexit(void)
  {
    __MCF_dtor_queue_finalize(__MCF_g->__cxa_atexit_queue, __MCF_g->__cxa_atexit_mutex, NULL);
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
do_encode_numeric_field(wchar_t* ptr, size_t width, uint64_t value)
  {
    for(size_t k = 0;  k != width;  ++k)
      ptr[k] = (wchar_t) (L'k' + (value >> (width + ~k) * 4) % 16U);
  }

static
void
do_on_process_attach(void)
  {
    /* Generate the unique name for this process.  */
    uint64_t pid = GetCurrentProcessId();
    uint64_t cookie = (uintptr_t) EncodePointer((PVOID)(uintptr_t) (pid << 32 | pid)) * 0x9E3779B97F4A7C15ULL;

    wchar_t gnbuffer[] __MCF_ALIGN(4) = L"Local\\__MCF_crt_xglobals_*?pid???_#?cookie????????";
    __MCF_ASSERT(gnbuffer[25] == L'*');
    do_encode_numeric_field(gnbuffer + 25, 8, pid);
    __MCF_ASSERT(gnbuffer[34] == L'#');
    do_encode_numeric_field(gnbuffer + 34, 16, cookie);
    __MCF_ASSERT(gnbuffer[50] == 0);

    /* Allocate or open storage for global data.
     * We are in the DLL main routine, so locking is unnecessary.  */
    UNICODE_STRING gname;
    gname.Length = (USHORT) (sizeof(gnbuffer) - sizeof(wchar_t));
    gname.MaximumLength = (USHORT) sizeof(gnbuffer);
    gname.Buffer = gnbuffer;

    OBJECT_ATTRIBUTES gattrs;
    InitializeObjectAttributes(&gattrs, &gname, OBJ_OPENIF | OBJ_EXCLUSIVE, NULL, NULL);
    __MCF_CHECK_NT(BaseGetNamedObjectDirectory(&(gattrs.RootDirectory)));
    __MCF_ASSERT(gattrs.RootDirectory);

    LARGE_INTEGER gsize;
    gsize.QuadPart = sizeof(__MCF_crt_xglobals);

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
      __MCF_g = __MCF_g->__self_ptr;
      __MCF_CHECK(__MCF_g->__self_size >= sizeof(__MCF_crt_xglobals));

      /* Reuse the existent region and close excess handles.  */
      NtUnmapViewOfSection(GetCurrentProcess(), gmem_base);
      NtClose(gfile);
      return;
    }

    /* Initialize it.  */
    __MCF_g->__self_ptr = __MCF_g;
    __MCF_g->__self_size = sizeof(__MCF_crt_xglobals);

    /* Allocate a TLS slot for this library.  */
    __MCF_g->__tls_index = TlsAlloc();
    __MCF_CHECK(__MCF_g->__tls_index != UINT32_MAX);

    /* Get the performance counter resolution.  */
    LARGE_INTEGER freq;
    __MCF_CHECK(QueryPerformanceFrequency(&freq));
    __MCF_g->__performance_frequency_reciprocal = 1000 / (double) freq.QuadPart;

    /* Attach the main thread.  */
    __MCF_g->__main_thread[0].__tid = _MCF_thread_self_tid();
    __MCF_CHECK_NT(NtDuplicateObject(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &(__MCF_g->__main_thread[0].__handle), 0, 0, DUPLICATE_SAME_ACCESS));
    __MCF_CHECK(__MCF_g->__main_thread[0].__handle);
    _MCF_atomic_store_32_rel(__MCF_g->__main_thread[0].__nref, 1);
    __MCF_CHECK(TlsSetValue(__MCF_g->__tls_index, __MCF_g->__main_thread));
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
 * Such initialization should happen as early as possible.  */
extern const PIMAGE_TLS_CALLBACK __MCF_xl_b;

const PIMAGE_TLS_CALLBACK __MCF_xl_b
  __attribute__((__section__(".CRT$XLB"), __used__)) = do_image_tls_callback;

#endif  /* DLL_EXPORT  */

/* This is a pointer to global data. If this library is linked statically,
 * all instances of this pointer in the same process should point to the
 * same memory.  */
__MCF_crt_xglobals* __MCF_g;
