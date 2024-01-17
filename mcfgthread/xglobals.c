/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022 - 2024, LH_Mouse. All wrongs reserved.  */

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
__MCF_initialize_winnt_timeout_v3(__MCF_winnt_timeout* to, const int64_t* ms_opt)
  {
    /* Initialize it to an infinite value.  */
    to->__li->QuadPart = INT64_MAX;
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

      to->__li->QuadPart = (11644473600000 + *ms_opt) * 10000;
    }
    else if(*ms_opt < 0) {
      /* If `*ms_opt` is negative, it denotes the number of milliseconds to
       * wait, relatively.  */
      if(*ms_opt < -922337203685477)
        return;

      to->__li->QuadPart = *ms_opt * 10000;
      QueryUnbiasedInterruptTime(&(to->__since));
    }
    else
      to->__li->QuadPart = 0;
  }

__MCF_DLLEXPORT
void
__MCF_adjust_winnt_timeout_v3(__MCF_winnt_timeout* to)
  {
    /* Absolute timeouts need no adjustment.  */
    if(to->__li->QuadPart >= 0)
      return;

    /* Add the number of 100 nanoseconds that have elapsed so far, to the
     * timeout which is negative, using saturation arithmetic.  */
    ULONGLONG old_since = to->__since;
    QueryUnbiasedInterruptTime(&(to->__since));
    to->__li->QuadPart += (LONGLONG) (to->__since - old_since);
    to->__li->QuadPart &= to->__li->QuadPart >> 63;
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

static
int
do_static_dtor_queue_pop(__MCF_dtor_element* elem, _MCF_mutex* mtx, __MCF_dtor_queue* queue, void* dso)
  {
    _MCF_mutex_lock(mtx, NULL);
    int err = __MCF_dtor_queue_pop(elem, queue, dso);
    _MCF_mutex_unlock(mtx);
    return err;
  }

__MCF_DLLEXPORT
void
__MCF_run_dtors_at_quick_exit(void* dso)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    __MCF_dtor_element elem;

    while(do_static_dtor_queue_pop(&elem, __MCF_g->__cxa_at_quick_exit_mtx, __MCF_g->__cxa_at_quick_exit_queue, dso) == 0)
      __MCF_invoke_cxa_dtor(elem.__dtor, elem.__this);
  }

__MCF_DLLEXPORT
void
__MCF_run_dtors_atexit(void* dso)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    __MCF_dtor_element elem;

    while(do_static_dtor_queue_pop(&elem, __MCF_g->__cxa_atexit_mtx, __MCF_g->__cxa_atexit_queue, dso) == 0)
      __MCF_invoke_cxa_dtor(elem.__dtor, elem.__this);
  }

static
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

    /* Perform lazy binding on some functions.  */
#define do_set_lazy_binding_(dll, proc)  \
    do {  \
      decltype_##proc** pp_##proc = __MCF_G_FIELD_OPT(__f_##proc);  \
      if(!pp_##proc)  \
        break;  \
      HMODULE mod_##proc = GetModuleHandleW(L##dll ".DLL");  \
      if(!mod_##proc)  \
        break;  \
      *pp_##proc = (decltype_##proc*)(INT_PTR) GetProcAddress(mod_##proc, #proc);  \
    }  \
    while(false)  /* no semicolon  */

    /* Window 8  */
    do_set_lazy_binding_("KERNEL32", GetSystemTimePreciseAsFileTime);

    /* Attach the main thread. The structure should be all zeroes so no
     * initialization is necessary.  */
    __MCF_thread_attach_foreign(__MCF_g->__main_thread);
  }

static
void
do_on_thread_detach(void)
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
    while(__MCF_dtor_queue_pop(&elem, self->__atexit_queue, NULL) == 0)
      __MCF_invoke_cxa_dtor(elem.__dtor, elem.__this);

    /* Call destructors of TLS keys. The TLS table may be modified by
     * destructors, so swap it out first.  */
    while(self->__tls_table->__begin) {
      __builtin_memcpy(&tls, self->__tls_table, sizeof(__MCF_tls_table));
      __builtin_memset(self->__tls_table, 0, sizeof(__MCF_tls_table));

      while(tls.__begin != tls.__end) {
        tls.__end --;

        /* Skip empty buckets.  */
        _MCF_tls_key* tkey = tls.__end->__key_opt;
        if(!tkey)
          continue;

        /* Call the destructor only if the value is not a null pointer, as
         * per POSIX.  */
        if(tls.__end->__value_opt && tkey->__dtor_opt && !_MCF_atomic_load_8_rlx(tkey->__deleted))
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

static
void
__stdcall
do_image_tls_callback(PVOID module, DWORD reason, LPVOID reserved)
  {
    (void) module;
    (void) reserved;

    /* Perform global initialization and per-thread cleanup as needed.
     * Note, upon `DLL_PROCESS_DETACH`, no cleanup is performed, because
     * other DLLs might have been unloaded and we would be referencing
     * unmapped memory. User code should call `__cxa_finalize(NULL)` before
     * exiting from a process.  */
    if(reason == DLL_PROCESS_ATTACH)
      do_on_process_attach();
    else if(reason == DLL_THREAD_DETACH)
      do_on_thread_detach();
  }

#ifdef DLL_EXPORT

/* When building the shared library, invoke the common routine from the DLL
 * entry point callback. The decorated name is fabricated such that it
 * remains the same on both x86 and x86-64.  */
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
HANDLE __MCF_crt_heap = __MCF_BAD_PTR;
double __MCF_crt_pf_recip = 1;
SYSTEM_INFO __MCF_crt_sysinfo = { .dwPageSize = 1 };

/* This is a pointer to global data. If this library is linked statically,
 * all instances of this pointer in the same process should point to the
 * same memory. The initializer prevents it from being placed into the
 * `.bss` section.  */
__MCF_crt_xglobals* __MCF_g = __MCF_BAD_PTR;
