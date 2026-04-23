/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "xprecompiled.h"
#define __MCF_XGLOBALS_IMPORT  __MCF_DLLEXPORT
#define __MCF_XGLOBALS_INLINE  __MCF_DLLEXPORT
#define __MCF_XGLOBALS_READONLY
#include "xglobals.h"
#include "../once.h"
#include <ntstatus.h>
#include <libloaderapi.h>

static inline
void
do_append_string(WCHAR** sp, const WCHAR* end_of_buffer, WCHAR c)
  {
    if(*sp != end_of_buffer)
      *((*sp) ++) = c;
  }

static
ULONG
do_format_message(ULONG code, WCHAR* outptr, const WCHAR* end_of_buffer)
  {
    return FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          outptr, (ULONG) (end_of_buffer - outptr), nullptr);
  }

__MCF_DLLEXPORT
void
__MCF_runtime_failure(const char* where)
  {
    ULONG last_error = GetLastError();
    WCHAR buffer[1536];
    WCHAR* sptr = buffer;
    WCHAR* end_of_buffer = buffer + ARRAYSIZE(buffer);

    /* Get a piece of localized text for the caption of the message box.  */
    UNICODE_STRING caption;
    caption.Buffer = sptr;
    caption.Length = 0;
    caption.MaximumLength = (USHORT) ((UINT_PTR) end_of_buffer - (UINT_PTR) sptr);

    /* #define ERROR_UNHANDLED_EXCEPTION   574L
     * {Application Error}
     * The exception %s (0x%08lx) occurred in the application at location 0x%08lx.  */
    WCHAR* lptr = end_of_buffer - 127;
    ULONG outlen = do_format_message(574, lptr, end_of_buffer);
    if((outlen != 0) && (*lptr == L'{')) {
      lptr ++;

      while((*lptr != 0) && (*lptr != L'}'))
        do_append_string(&sptr, end_of_buffer, *(lptr ++));

      caption.Length = (USHORT) ((UINT_PTR) sptr - (UINT_PTR) caption.Buffer);
    }

    /* Get a piece of localized text for the text of the message box.  */
    UNICODE_STRING text;
    text.Buffer = sptr;
    text.Length = 0;
    text.MaximumLength = (USHORT) ((UINT_PTR) end_of_buffer - (UINT_PTR) sptr);

    /* Get the file name of the executable.  */
    outlen = GetModuleFileNameW(NULL, sptr, (ULONG) (end_of_buffer - sptr));
    if(outlen != 0) {
      sptr += outlen;

      do_append_string(&sptr, end_of_buffer, L'\r');
      do_append_string(&sptr, end_of_buffer, L'\n');
      do_append_string(&sptr, end_of_buffer, L'\r');
      do_append_string(&sptr, end_of_buffer, L'\n');

      text.Length = (USHORT) ((UINT_PTR) sptr - (UINT_PTR) text.Buffer);
    }

    /* #define ERROR_DEBUG_ATTACH_FAILED   590L
     * {Unexpected Failure in DebugActiveProcess}
     * An unexpected failure occurred while processing a DebugActiveProcess API
     * request. You may choose OK to terminate the process, or Cancel to ignore
     * the error.  */
    lptr = end_of_buffer - 127;
    outlen = do_format_message(590, lptr, end_of_buffer);
    if((outlen != 0) && (*lptr == L'{')) {
      lptr ++;

      while((*lptr != 0) && (*lptr != L'}'))
        if((*lptr == L'D') && __MCF_mequal(lptr, L"DebugActiveProcess", 18 * sizeof(WCHAR))) {
          lptr += 18;
          do_append_string(&sptr, end_of_buffer, L'`');
          for(const char* pwh = where;  *pwh;  ++pwh)
            do_append_string(&sptr, end_of_buffer, (unsigned char) *pwh);
          do_append_string(&sptr, end_of_buffer, L'`');
        } else
          do_append_string(&sptr, end_of_buffer, *(lptr ++));

      do_append_string(&sptr, end_of_buffer, L':');
      do_append_string(&sptr, end_of_buffer, L' ');

      outlen = do_format_message(last_error, sptr, end_of_buffer);
      sptr += outlen;

      text.Length = (USHORT) ((UINT_PTR) sptr - (UINT_PTR) text.Buffer);
    }

    /* If this process has a console, write the message directly into it.
     * Errors are ignored.  */
    HANDLE console = GetStdHandle(STD_ERROR_HANDLE);
    if((console != INVALID_HANDLE_VALUE) && (console != NULL)) {
      DWORD nwritten;
      WriteConsoleW(console, text.Buffer, text.Length / sizeof(WCHAR), &nwritten, nullptr);
      (void) nwritten;
    }

    /* If we are in a DLL entry-point function or a TLS callback, it is not safe
     * to call `MessageBoxW()` from USER32.DLL, so request CSRSS.EXE to display
     * the message box for us.  */
    ULONG rhe_resp = 0;
    ULONG_PTR rhe_params[4] = { (ULONG_PTR) &text, (ULONG_PTR) &caption, MB_OK | MB_ICONSTOP, 0 };
    NtRaiseHardError(STATUS_SERVICE_NOTIFICATION, ARRAYSIZE(rhe_params),
                     0b0011, /* parameters 0 and 1 are `UNICODE_STRING*` */
                     rhe_params, 1 /* OptionOk */, &rhe_resp);
    (void) rhe_resp;

    /* Terminate the current process.  */
    EXCEPTION_RECORD record = { .ExceptionCode = (ULONG) STATUS_FAIL_FAST_EXCEPTION,
                                .ExceptionFlags = EXCEPTION_NONCONTINUABLE,
                                .ExceptionAddress = __builtin_return_address(0) };
    RaiseFailFastException(&record, nullptr, 0);
    __builtin_trap();
  }

__MCF_DLLEXPORT __MCF_FN_PURE
uint32_t
_MCF_get_win32_error(void)
  {
    return GetLastError();
  }

__MCF_DLLEXPORT __MCF_FN_CONST
size_t
_MCF_get_page_size(void)
  {
    return __MCF_crt_sysinfo.dwPageSize;
  }

__MCF_DLLEXPORT __MCF_FN_CONST
size_t
_MCF_get_processor_count(void)
  {
    return __MCF_crt_sysinfo.dwNumberOfProcessors;
  }

__MCF_DLLEXPORT __MCF_FN_CONST
uintptr_t
_MCF_get_active_processor_mask(void)
  {
    return __MCF_crt_sysinfo.dwActiveProcessorMask;
  }

static
EXCEPTION_DISPOSITION
do_call_once_seh_unwind(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx);

__MCF_DLLEXPORT
void
__MCF_gthr_call_once_seh_take_over(_MCF_once* once, __MCF_cxa_dtor_any_ init_proc, void* arg)
  {
#ifdef __MCF_M_X8632
    __MCF_USING_SEH_HANDLER(do_call_once_seh_unwind, (DWORD) once);
    _MCF_once* saved_once = once;
#  define do_seh_once_reg(frm, disp)  (((DWORD**) (frm))[2])
#else
    __MCF_USING_SEH_HANDLER(do_call_once_seh_unwind);
#  if defined __MCF_M_X8664_ASM
    register _MCF_once* saved_once __asm__("rsi") = once;
#  elif defined __MCF_M_ARM64_ASM
    register _MCF_once* saved_once __asm__("x25") = once;
#  endif
    __asm__ volatile ("" : "+r"(saved_once));
#  if defined __MCF_M_X8664
#    define do_seh_once_reg(frm, disp)  (((DISPATCHER_CONTEXT*) (disp))->ContextRecord->Rsi)
#  elif defined __MCF_M_ARM64
#    define do_seh_once_reg(frm, disp)  (((DISPATCHER_CONTEXT*) (disp))->ContextRecord->X25)
#  endif
#endif

    /* Do initialization. This is the normal path.  */
    __MCF_invoke_cxa_dtor(init_proc, arg);

    /* Disarm the once flag with a tail call.  */
    _MCF_once_release(saved_once);
  }

static
EXCEPTION_DISPOSITION
do_call_once_seh_unwind(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx)
  {
    (void) estab_frame;
    (void) ctx;
    (void) disp_ctx;

    /* If the stack is being unwound, reset the once flag.  */
    if(rec->ExceptionFlags & EXCEPTION_UNWINDING)
      _MCF_once_abort((_MCF_once*) do_seh_once_reg(estab_frame, disp_ctx));

    /* Continue unwinding.  */
    return ExceptionContinueSearch;
  }

__MCF_DLLEXPORT
EXCEPTION_DISPOSITION
__MCF_seh_top(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx)
  {
    (void) estab_frame;
    (void) ctx;
    (void) disp_ctx;

    /* GCC raises `0x20474343` to search for an exception handler. If the
     * control flow resumes after `RaiseException()`, `std::terminate()` will
     * be called.  */
    if((rec->ExceptionCode == 0x20474343) && !(rec->ExceptionFlags & EXCEPTION_NONCONTINUABLE))
      return ExceptionContinueExecution;

    /* In all the other cases, terminate the process. For MSVC we should have
     * called `__std_terminate()`, but we are not linking against their CRT so
     * it's unavailable.  */
    RaiseFailFastException(rec, ctx, 0);
    __builtin_trap();
  }

__MCF_DLLEXPORT
void
__MCF_initialize_winnt_timeout_v3(__MCF_winnt_timeout* to, const int64_t* ms_opt)
  {
    /* Initialize it to an infinite value.  */
    to->li.QuadPart = INT64_MAX;
    to->since = 0;

    /* If no timeout is given, wait indefinitely.  */
    if(!ms_opt)
      return;

    if(*ms_opt > 0) {
      /* If `*ms_opt` is positive, it denotes the number of milliseconds since
       * 1970-01-01T00:00:00Z, and has to be converted into the number of 100
       * nanoseconds since the 1601-01-01T00:00:00Z.  */
      if(*ms_opt > 910692730085477)
        return;

      to->li.QuadPart = (11644473600000 + *ms_opt) * 10000;
      __MCF_ASSERT(to->li.QuadPart > 0);
    }
    else if(*ms_opt < 0) {
      /* If `*ms_opt` is negative, it denotes the number of milliseconds to
       * wait, relatively.  */
      if(*ms_opt < -922337203685477)
        return;

      to->li.QuadPart = *ms_opt * 10000;
      __MCF_ASSERT(to->li.QuadPart < 0);
      QueryUnbiasedInterruptTime(&(to->since));
    }
    else
      to->li.QuadPart = 0;
  }

__MCF_DLLEXPORT
void
__MCF_adjust_winnt_timeout_v3(__MCF_winnt_timeout* to)
  {
    /* Absolute timeouts need no adjustment.  */
    if(to->li.QuadPart >= 0)
      return;

    /* Add the number of 100 nanoseconds that have elapsed so far, to the
     * timeout which is negative, using saturation arithmetic.  */
    ULONGLONG old_since = to->since;
    QueryUnbiasedInterruptTime(&(to->since));
    to->li.QuadPart += (LONGLONG) (to->since - old_since);
    to->li.QuadPart &= to->li.QuadPart >> 63;
  }

__MCF_DLLEXPORT __MCF_NEVER_INLINE
void
__MCF_batch_release_common(const void* key, size_t count)
  {
    /* A call to `ExitProcess()` terminates all the other threads, even if
     * they are waiting. We don't release the keyed event in this case, as it
     * blocks the calling thread infinitely if there is no thread to wake up.
     * See <https://github.com/lhmouse/mcfgthread/issues/21>.  */
    static const __MCF_winnt_timeout safety_timeout = { .li.QuadPart = -10000000 };
    size_t remaining = count;
    while((remaining != 0) && !RtlDllShutdownInProgress())
      if(__MCF_keyed_event_signal(key, &safety_timeout) == 0)
        remaining --;
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
do_pop_dtor(__MCF_dtor_element* elem, _MCF_mutex* mtx, __MCF_dtor_queue* queue, void* dso)
  {
    _MCF_mutex_lock_slow(mtx, nullptr);
    int err = __MCF_dtor_queue_pop(elem, queue, dso);
    _MCF_mutex_unlock_slow(mtx);
    return err;
  }

__MCF_DLLEXPORT
void
__MCF_run_static_dtors(_MCF_mutex* mtx, __MCF_dtor_queue* queue, void* dso)
  {
    __MCF_USING_SEH_HANDLER(__MCF_seh_top);
    __MCF_dtor_element elem;

    while(do_pop_dtor(&elem, mtx, queue, dso) == 0)
      __MCF_invoke_cxa_dtor(elem.__dtor, elem.__this);
  }

static
void
__fastcall
do_hex_encode(wchar_t* ptr, unsigned width, uint64_t value, const wchar_t* digits)
  {
    for(unsigned k = 0;  k != width;  ++k)
      ptr[k] = digits[(value >> (width - 1 - k) * 4) & 0x0FU];
  }

static
uint64_t
__fastcall
do_make_cookie(uint32_t seed)
  {
    uintptr_t xid = __MCF_64_32(seed * 0x100000001ULL, seed) ^ __MCF_UPTR_MAX;
    return (uintptr_t) EncodePointer((PVOID) xid) * 0x9E3779B97F4A7C15ULL;
  }

__MCF_DLLEXPORT
void
__MCF_gthread_initialize_globals(void)
  {
    GetSystemInfo(&__MCF_crt_sysinfo);

    LARGE_INTEGER pfreq;
    __MCF_CHECK(QueryPerformanceFrequency(&pfreq));
    __MCF_crt_pf_recip = 1000 / (double) pfreq.QuadPart;

    __MCF_crt_heap = GetProcessHeap();
    __MCF_CHECK(__MCF_crt_heap);
#ifdef __MCF_DEBUG
    HeapSetInformation(__MCF_crt_heap, HeapEnableTerminationOnCorruption, nullptr, 0);
#endif

    /* Load system DLLs. It's not necessary to call `FreeLibrary()`, as these
     * can't be unloaded.  */
    __MCF_crt_ntdll = LoadLibraryExW(L"NTDLL.DLL", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    __MCF_crt_kernel32 = LoadLibraryExW(L"KERNEL32.DLL", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    __MCF_crt_kernelbase = LoadLibraryExW(L"KERNELBASE.DLL", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);

    /* This function is available since Windows 11 24H2. It has the same
     * signature as `TlsGetValue()`, so the latter can be used as a backup.  */
    FARPROC dll_fn = GetProcAddress(__MCF_crt_kernel32, "TlsGetValue2");
    __MCF_crt_TlsGetValue = dll_fn ? __MCF_CAST_PTR(typeof_TlsGetValue2, dll_fn) : TlsGetValue;

    /* Generate the unique name for this process.  */
    static WCHAR gnbuffer[] = L"Local\\__MCF_crt_xglobals_*?pid???_#?cookie????????";
    static UNICODE_STRING gname = __MCF_NT_STRING_INIT(gnbuffer);
    static OBJECT_ATTRIBUTES gattrs = { .Length = sizeof(gattrs), .ObjectName = &gname,
                                        .Attributes = OBJ_OPENIF | OBJ_EXCLUSIVE };
    __MCF_set_directory_to_BaseNamedObject(&gattrs);

    const uint32_t pid = (uint32_t) __MCF_pid();
    __MCF_ASSERT(gnbuffer[25] == L'*');
    do_hex_encode(gnbuffer + 25, 8, pid, L"0123456789ABCDEF");
    __MCF_ASSERT(gnbuffer[34] == L'#');
    do_hex_encode(gnbuffer + 34, 16, do_make_cookie(pid), L"GHJKLMNPQRSTUWXY");
    __MCF_ASSERT(gnbuffer[50] == 0);

    /* Allocate or open storage for global data. We are in the DLL main routine,
     * so locking is not necessary. Unlike `CreateFileMappingW()`, the handle
     * and view shall not be inherited by child processes.  */
    HANDLE gfile = __MCF_create_named_section(&gattrs, sizeof(__MCF_crt_xglobals));
    __MCF_CHECK(gfile);
    __MCF_g = __MCF_map_view_of_section(gfile, false);
    __MCF_CHECK(__MCF_g);

    if(__MCF_g->self_ptr) {
      /* Reuse the existent region and close excess handles.  */
      void* new_base = __MCF_g;
      __MCF_g = __MCF_g->self_ptr;
      __MCF_unmap_view_of_section(new_base);
      __MCF_close_handle(gfile);
      return;
    }

    /* The region is new, so initialize it.  */
    __MCF_g->self_ptr = __MCF_g;
    __MCF_g->self_size = sizeof(__MCF_crt_xglobals);

    /* Allocate a TLS slot for this library.  */
    __MCF_G(tls_index) = TlsAlloc();
    __MCF_CHECK(__MCF_G(tls_index) != TLS_OUT_OF_INDEXES);

    /* This function is available since Windows 8.  */
    dll_fn = GetProcAddress(__MCF_crt_kernel32, "GetSystemTimePreciseAsFileTime");
    __MCF_G(imp_GetSystemTimePreciseAsFileTime) = (void*) dll_fn;

    /* This function is available since Windows 10. Microsoft documentation says
     * this is exported from KERNEL32.DLL, but it's really only exported from
     * KERNELBASE.DLL. However, we no longer call this function; the field is
     * initialized for backward compatibility.  */
    dll_fn = GetProcAddress(__MCF_crt_kernelbase, "QueryInterruptTime");
    __MCF_G(imp_QueryInterruptTime) = (void*) dll_fn;

    /* This function is available since Windows 10. Microsoft documentation says
     * this is exported from KERNEL32.DLL, but it's really only exported from
     * KERNELBASE.DLL. Strangely, `QueryUnbiasedInterruptTime` is exported from
     * KERNEL32.DLL since Windows 7, so there's no need to load it dynamically.  */
    dll_fn = GetProcAddress(__MCF_crt_kernelbase, "QueryUnbiasedInterruptTimePrecise");
    __MCF_G(imp_QueryUnbiasedInterruptTimePrecise) = (void*) dll_fn;

    /* This function is available since Windows 10. Microsoft documentation says
     * this is exported from KERNEL32.DLL, but it's really only exported from
     * KERNELBASE.DLL.  */
    dll_fn = GetProcAddress(__MCF_crt_kernelbase, "QueryInterruptTimePrecise");
    __MCF_G(imp_QueryInterruptTimePrecise) = (void*) dll_fn;

    /* Attach the main thread and make it joinable. The structure should
     * be all zeroes so no initialization is necessary.  */
    __MCF_thread_attach_foreign(__MCF_G(main_thread));
    _MCF_atomic_store_32_rel(__MCF_G(main_thread)->__nref, 2);
  }

__MCF_DLLEXPORT
void
__MCF_gthread_on_thread_exit(void)
  {
    __MCF_USING_SEH_HANDLER(__MCF_seh_top);
    _MCF_thread* self = __MCF_crt_TlsGetValue(__MCF_G(tls_index));
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
    __MCF_dtor_element elem;
    while(__MCF_dtor_queue_pop(&elem, self->__atexit_queue, nullptr) == 0)
      __MCF_invoke_cxa_dtor(elem.__dtor, elem.__this);

    while(self->__tls_table->__begin) {
      /* Call destructors of TLS keys. The TLS table may be modified by
       * destructors, so swap it out first.  */
      __MCF_tls_element* tls_begin = self->__tls_table->__begin;
      __MCF_tls_element* tls_end = self->__tls_table->__end;

      self->__tls_table->__begin = nullptr;
      self->__tls_table->__end = nullptr;
      self->__tls_table->__size_hint = 0;

      while(tls_begin != tls_end) {
        tls_end --;
        _MCF_tls_key* tkey = tls_end->__key_opt;
        if(!tkey)
          continue;

        /* POSIX requires that the destructor is called only when the key has
         * not been deleted and the value is not a null pointer.  */
        if(!_MCF_atomic_load_b_rlx(tkey->__deleted) && tkey->__dtor_opt && tls_end->__value_opt)
          __MCF_invoke_cxa_dtor(tkey->__dtor_opt, tls_end->__value_opt);

        _MCF_tls_key_drop_ref_nonnull(tkey);
      }

      /* Deallocate the table which should be empty now.  */
      __MCF_mfree_nonnull(tls_begin);
    }

    /* Poison this value.  */
    TlsSetValue(__MCF_G(tls_index), __MCF_BAD_PTR);
    _MCF_thread_drop_ref_nonnull(self);
  }

/* These are constants that have to be initialized at load time. The
 * initializers prevent them from being placed into the`.bss` section.  */
__MCF_BR(GUID) const __MCF_crt_gthread_guid = { __MCF_GUID(9FB2D15C,C5F2,4AE7,868D,2769591B8E92) };
SYSTEM_INFO __MCF_crt_sysinfo = { .dwPageSize = 1 };
double __MCF_crt_pf_recip = 1;
HANDLE __MCF_crt_heap = __MCF_BAD_PTR;
HMODULE __MCF_crt_ntdll = __MCF_BAD_PTR;
HMODULE __MCF_crt_kernel32 = __MCF_BAD_PTR;
HMODULE __MCF_crt_kernelbase = __MCF_BAD_PTR;
typeof_TlsGetValue2* __MCF_crt_TlsGetValue = __MCF_BAD_PTR;

/* This is a pointer to global data. If this library is linked statically,
 * all instances of this pointer in the same process should point to the
 * same memory. The initializer prevents it from being placed into the
 * `.bss` section.  */
__MCF_crt_xglobals* restrict __MCF_g = __MCF_BAD_PTR;

#ifdef __MCF_IN_DLL

/* When building the shared library, invoke common routines from the DLL
 * entry point callback. This has the same signature as `DllMain()`.  */
#if defined __MSYS__
#  define DllMainCRTStartup  _msys_dll_entry
#elif defined __CYGWIN__
#  define DllMainCRTStartup  _cygwin_dll_entry
#elif defined _MSC_VER
#  define DllMainCRTStartup  _DllMainCRTStartup
#endif

int
__stdcall
DllMainCRTStartup(PVOID instance, ULONG reason, PVOID reserved);

__MCF_REALIGN_SP
int
__stdcall
DllMainCRTStartup(PVOID instance, ULONG reason, PVOID reserved)
  {
    (void) instance;
    (void) reserved;

    switch(reason)
      {
      case DLL_PROCESS_ATTACH:
        __MCF_gthread_initialize_globals();
        VirtualProtect((void*) &__MCF_g, sizeof(__MCF_g), PAGE_READONLY, &(DWORD){ 0 } );
        return 1;

      case DLL_THREAD_DETACH:
        __MCF_gthread_on_thread_exit();
        return 1;

      default:
        return 1;
      }
  }

/* According to GCC documentation, these functions are required by a
 * freestanding implementation. When doing a non-emulative build, they are
 * also exported for reuse.  */
__MCF_DLLEXPORT __attribute__((__flatten__))
void*
memcpy(void* restrict dst, const void* restrict src, size_t size)
  {
    return __MCF_mcopy(dst, src, size);
  }

__MCF_DLLEXPORT __attribute__((__flatten__))
void*
memmove(void* dst, const void* src, size_t size)
  {
    return ((uintptr_t) dst - (uintptr_t) src >= size)
           ? __MCF_mcopy(dst, src, size)
           : __MCF_mcopy_backward(dst, src, size);
  }

__MCF_DLLEXPORT __attribute__((__flatten__))
void*
memset(void* dst, int val, size_t size)
  {
    return __MCF_mfill(dst, val, size);
  }

__MCF_DLLEXPORT __attribute__((__flatten__))
int
memcmp(const void* src, const void* dst, size_t size)
  {
    return __MCF_mcompare(src, dst, size);
  }

#if defined _MSC_VER
/* Microsoft LINK requires this for a reason.  */
const int _fltused __MCF_CRT_RDATA = 0x9875;
#endif

#if defined __MCF_M_X8632
/* On x86-32, the load config directory contains the address and size of the
 * exception handler table. Exception handlers that are not in this table
 * will be rejected by the system. `__MCF_i386_se_handler_table` points to an
 * array of RVAs to valid handlers, and the value of (not the value it points
 * to) `__MCF_i386_se_handler_count` is the number of handlers.  */
extern const ULONG __MCF_i386_se_handler_table[];
extern char __MCF_i386_se_handler_count[];
__asm__ (
"\n .section .rdata, \"dr\""
"\n   .p2align 2"
"\n ___MCF_i386_se_handler_table:"
"\n   .rva ___MCF_seh_top"
"\n   .rva _do_call_once_seh_unwind\n"
"\n .equiv ___MCF_i386_se_handler_count, (. - ___MCF_i386_se_handler_table) / 4"
"\n .text"
);
#endif

#if defined __MCF_M_ARM64EC
/* This section has been heavily modified from 'chpe.S' from mingw-w64. Only
 * symbols that are documented by Microsoft are kept. Original code is declared
 * to be in the Public Domain.  */
extern const ULONG __MCF_arm64ec_chpe_metadata[];
__asm__ (
"\n .section .rdata, \"dr\""
"\n   .p2align 2"
"\n __MCF_arm64ec_chpe_metadata:"
"\n   .long 1" /* Version */
"\n   .rva __hybrid_code_map"
"\n   .long __hybrid_code_map_count"
"\n   .rva __x64_code_ranges_to_entry_points"
"\n   .rva __arm64x_redirection_metadata"
"\n   .rva __os_arm64x_dispatch_call_no_redirect"
"\n   .rva __os_arm64x_dispatch_ret"
"\n   .rva __os_arm64x_check_call"
"\n   .rva __os_arm64x_check_icall"
"\n   .rva __os_arm64x_check_icall_cfg"
"\n   .rva __arm64x_native_entrypoint"
"\n   .rva __hybrid_auxiliary_iat"
"\n   .long __x64_code_ranges_to_entry_points_count"
"\n   .long __arm64x_redirection_metadata_count"
"\n   .rva __os_arm64x_get_x64_information"
"\n   .rva __os_arm64x_set_x64_information"
"\n   .rva __arm64x_extra_rfe_table"
"\n   .long __arm64x_extra_rfe_table_size"
"\n   .rva __os_arm64x_x64_jump"
"\n   .rva __hybrid_auxiliary_iat_copy"
"\n   .rva __hybrid_auxiliary_delayload_iat"
"\n   .rva __hybrid_auxiliary_delayload_iat_copy"
"\n   .long __hybrid_image_info_bitfield"
"\n   .rva __os_arm64x_helper3"
"\n   .rva __os_arm64x_helper4"
"\n   .rva __os_arm64x_helper5"
"\n   .rva __os_arm64x_helper6"
"\n   .rva __os_arm64x_helper7"
"\n   .rva __os_arm64x_helper8"
/* These are pointers to helper routines, which will be filled after the image
 * is loaded by the operating system.  */
"\n .section .00cfg, \"dr\""
"\n   .p2align 3"
"\n .globl __os_arm64x_dispatch_call_no_redirect"
"\n __os_arm64x_dispatch_call_no_redirect:"
"\n   .quad 0"
"\n .globl __os_arm64x_dispatch_ret"
"\n __os_arm64x_dispatch_ret:"
"\n   .quad 0"
"\n .globl __os_arm64x_check_call"
"\n __os_arm64x_check_call:"
"\n   .quad 0"
"\n .globl __os_arm64x_check_icall"
"\n __os_arm64x_check_icall:"
"\n   .quad 0"
"\n .globl __os_arm64x_check_icall_cfg"
"\n __os_arm64x_check_icall_cfg:"
"\n   .quad 0"
"\n .globl __os_arm64x_get_x64_information"
"\n __os_arm64x_get_x64_information:"
"\n   .quad 0"
"\n .globl __os_arm64x_set_x64_information"
"\n __os_arm64x_set_x64_information:"
"\n   .quad 0"
"\n .globl __os_arm64x_x64_jump"
"\n __os_arm64x_x64_jump:"
"\n   .quad 0"
"\n .globl __os_arm64x_helper3"
"\n __os_arm64x_helper3:"
"\n   .quad 0"
"\n .globl __os_arm64x_helper4"
"\n __os_arm64x_helper4:"
"\n   .quad 0"
"\n .globl __os_arm64x_helper5"
"\n __os_arm64x_helper5:"
"\n   .quad 0"
"\n .globl __os_arm64x_helper6"
"\n __os_arm64x_helper6:"
"\n   .quad 0"
"\n .globl __os_arm64x_helper7"
"\n __os_arm64x_helper7:"
"\n   .quad 0"
"\n .globl __os_arm64x_helper8"
"\n __os_arm64x_helper8:"
"\n   .quad 0"
/* This is the ARM64EC Adjustor Thunk. Calls to this function are synthesized
 * by the compiler.  */
"\n .section .text$__icall_helper_arm64ec, \"x\""
"\n   .p2align 2"
"\n .globl __icall_helper_arm64ec"
"\n .def __icall_helper_arm64ec; .scl 2; .type 32; .endef"
"\n __icall_helper_arm64ec:"
"\n .seh_proc __icall_helper_arm64ec"
"\n   stp fp, lr, [sp, -16]!"
"\n .seh_save_fplr_x 16"
"\n   mov fp, sp"
"\n .seh_set_fp"
"\n .seh_endprologue"
"\n   adrp x16, __os_arm64x_check_icall"
"\n   ldr x16, [x16, :lo12:__os_arm64x_check_icall]"
"\n   blr x16"
"\n .seh_startepilogue"
"\n   ldp fp, lr, [sp], 16"
"\n .seh_save_fplr_x 16"
"\n .seh_endepilogue"
"\n   br x11"
"\n .seh_endproc"
/* This is a common wrapper with an Exit Thunk for x86-64 callback functions
 * that return either values in RAX, or void.  */
"\n .globl __MCF_arm64ec_exit_thunk_p"
"\n .def __MCF_arm64ec_exit_thunk_p; .scl 2; .type 32; .endef"
"\n __MCF_arm64ec_exit_thunk_p:"
"\n .seh_proc __MCF_arm64ec_exit_thunk_p"
"\n   stp fp, lr, [sp, -16]!"
"\n .seh_save_fplr_x 16"
"\n   mov fp, sp"
"\n .seh_set_fp"
"\n   sub sp, sp, 32"
"\n .seh_stackalloc 32"
"\n .seh_endprologue"
"\n   adrp x16, __os_arm64x_dispatch_call_no_redirect"
"\n   ldr x16, [x16, :lo12:__os_arm64x_dispatch_call_no_redirect]"
"\n   blr x16"
"\n   mov x0, x8"
"\n .seh_startepilogue"
"\n   add sp, sp, 32"
"\n .seh_stackalloc 32"
"\n   ldp fp, lr, [sp], 16"
"\n .seh_save_fplr_x 16"
"\n .seh_endepilogue"
"\n   ret"
"\n .seh_endproc"
"\n .text"
);
#endif

/* If the image subsystem version is set to 6.3+, Windows requires that the
 * security cookie shall exist and shall be initialized to a constant value
 * when the image is loaded. Otherwise the system will reject the image with
 * `STATUS_INVALID_IMAGE_FORMAT`.  */
UINT_PTR __security_cookie = __MCF_64_32(0x2B992DDFA232, 0xBB40E64E);

struct _IMAGE_LOAD_CONFIG_DIRECTORY_10_0_26100_7175
  {
    DWORD Size;
    DWORD TimeDateStamp;
    WORD MajorVersion;
    WORD MinorVersion;
    DWORD GlobalFlagsClear;
    DWORD GlobalFlagsSet;
    DWORD CriticalSectionDefaultTimeout;
    ULONG_PTR DeCommitFreeBlockThreshold;
    ULONG_PTR DeCommitTotalFreeThreshold;
    ULONG_PTR LockPrefixTable;
    ULONG_PTR MaximumAllocationSize;
    ULONG_PTR VirtualMemoryThreshold;
    ULONG_PTR __MCF_64_32(ProcessAffinityMask, ProcessHeapFlags);
    DWORD __MCF_64_32(ProcessHeapFlags, ProcessAffinityMask);
    WORD CSDVersion;
    WORD DependentLoadFlags;
    ULONG_PTR EditList;
    ULONG_PTR SecurityCookie;
    ULONG_PTR SEHandlerTable;
    ULONG_PTR SEHandlerCount;
    ULONG_PTR GuardCFCheckFunctionPointer;
    ULONG_PTR GuardCFDispatchFunctionPointer;
    ULONG_PTR GuardCFFunctionTable;
    ULONG_PTR GuardCFFunctionCount;
    DWORD GuardFlags;
    WORD CodeIntegrity_Flags;
    WORD CodeIntegrity_Catalog;
    DWORD CodeIntegrity_CatalogOffset;
    DWORD CodeIntegrity_Reserved;
    ULONG_PTR GuardAddressTakenIatEntryTable;
    ULONG_PTR GuardAddressTakenIatEntryCount;
    ULONG_PTR GuardLongJumpTargetTable;
    ULONG_PTR GuardLongJumpTargetCount;
    ULONG_PTR DynamicValueRelocTable;
    ULONG_PTR CHPEMetadataPointer;
    ULONG_PTR GuardRFFailureRoutine;
    ULONG_PTR GuardRFFailureRoutineFunctionPointer;
    DWORD DynamicValueRelocTableOffset;
    WORD DynamicValueRelocTableSection;
    WORD Reserved2;
    ULONG_PTR GuardRFVerifyStackPointerFunctionPointer;
    DWORD HotPatchTableOffset;
    DWORD Reserved3;
    ULONG_PTR EnclaveConfigurationPointer;
    ULONG_PTR VolatileMetadataPointer;
    ULONG_PTR GuardEHContinuationTable;
    ULONG_PTR GuardEHContinuationCount;
    ULONG_PTR GuardXFGCheckFunctionPointer;
    ULONG_PTR GuardXFGDispatchFunctionPointer;
    ULONG_PTR GuardXFGTableDispatchFunctionPointer;
    ULONG_PTR CastGuardOsDeterminedFailureMode;
    ULONG_PTR GuardMemcpyFunctionPointer;
    ULONG_PTR UmaFunctionPointers;
  }
const _load_config_used __MCF_CRT_RDATA =
  {
    .Size = sizeof(_load_config_used),
    .DependentLoadFlags = LOAD_LIBRARY_SEARCH_SYSTEM32,
    .SecurityCookie = (ULONG_PTR) &__security_cookie,
#if defined __MCF_M_X8632
    .SEHandlerTable = (ULONG_PTR) __MCF_i386_se_handler_table,
    .SEHandlerCount = (ULONG_PTR) __MCF_i386_se_handler_count,
#endif
#if defined __MCF_M_ARM64EC
    .CHPEMetadataPointer = (ULONG_PTR) __MCF_arm64ec_chpe_metadata,
#endif
  };

#else  /* __MCF_IN_DLL  */

/* When building the static library, invoke common routines from a TLS
 * callback.  */
static __MCF_REALIGN_SP
void
__stdcall
do_tls_callback(PVOID module, ULONG reason, PVOID reserved)
  {
    (void) module;
    (void) reserved;

    switch(reason)
      {
      case DLL_PROCESS_ATTACH:
        __MCF_gthread_initialize_globals();
        return;

      case DLL_THREAD_DETACH:
        __MCF_gthread_on_thread_exit();
        return;

      default:
        return;
      }
  }

/* This requires the main executable be linked with 'tlssup.o'. Such
 * initialization shall happen as early as possible.  */
#if defined _MSC_VER
__pragma(comment(linker, "/include:" __MCF_USYM "_tls_used"))
__pragma(section(".CRT$XLB", read))
#endif
const PIMAGE_TLS_CALLBACK __MCF_crt_xl_b __MCF_CRT_XL(B) = do_tls_callback;

#if defined __MCF_M_X8632 && defined _MSC_VER
/* Register SEH handlers. In the DLL we build a handler table by hand which works
 * on all compilers. In the static library we use the `.safeseh` directive but it
 * is only supported by Microsoft LINK, or LLD in LINK mode.  */
__asm__ (
"\n.safeseh ___MCF_seh_top"
"\n.safeseh _do_call_once_seh_unwind"
);
#endif

#endif  /* __MCF_IN_DLL  */
