/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
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

    /* GCC raises `0x20474343` to search for an exception handler, and raises
     * `0x21474343` to unwind the stack. If the control flow resumes after
     * `RaiseException()`, `std::terminate()` is called.  */
    ULONG nonmatch = rec->ExceptionFlags & EXCEPTION_NONCONTINUABLE;
    nonmatch |= (rec->ExceptionCode & 0x20FFFFFF) ^ 0x20474343;
    return nonmatch ? ExceptionContinueSearch : ExceptionContinueExecution;
  }

__asm__ (
#if defined __i386__ && defined __MCF_IN_DLL
".globl @__MCF_safeseh__seh_top  \n"
".equiv @__MCF_safeseh__seh_top, ___MCF_seh_top  \n"
#elif defined __i386__ && defined _MSC_VER
".safeseh ___MCF_seh_top  \n"
#endif
".globl @feat.00  \n"
".set @feat.00, 1  \n"
);

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
void
__MCF_batch_release_common(const void* key, size_t count)
  {
    /* A call to `ExitProcess()` terminates all the other threads, even if
     * they are waiting. We don't release the keyed event in this case, as it
     * blocks the calling thread infinitely if there is no thread to wake up.
     * See <https://github.com/lhmouse/mcfgthread/issues/21>.  */
    size_t woken_num = 0;
    while((woken_num != count) && !__MCF_is_process_shutting_down())
      if(__MCF_keyed_event_signal(key, &(__MCF_winnt_timeout) { .__li.QuadPart = -1000 }) == 0)
        woken_num ++;
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
    GetSystemInfo(&__MCF_crt_sysinfo);

#ifdef __MCF_DEBUG
    __MCF_CHECK(HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, __MCF_nullptr, 0));
#endif
    __MCF_crt_heap = GetProcessHeap();
    __MCF_CHECK(__MCF_crt_heap);

    LARGE_INTEGER pfreq;
    __MCF_CHECK(QueryPerformanceFrequency(&pfreq));
    __MCF_crt_pf_recip = 1000 / (double) pfreq.QuadPart;

    __MCF_CHECK(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, L"KERNELBASE.DLL", &__MCF_crt_kernelbase));
    __MCF_CHECK(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, L"NTDLL.DLL", &__MCF_crt_ntdll));

    __MCF_crt_TlsGetValue = TlsGetValue;
    __MCF_LAZY_LOAD(&__MCF_crt_TlsGetValue, __MCF_crt_kernelbase, TlsGetValue2);

    /* Generate the unique name for this process.  */
    static WCHAR gnbuffer[] = L"Local\\__MCF_crt_xglobals_*?pid???_#?cookie????????";
    UNICODE_STRING gname = __MCF_NT_STRING_INIT(gnbuffer);
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
    __MCF_G_SET_LAZY(__MCF_crt_kernelbase, GetSystemTimePreciseAsFileTime);  /* win8 */
    __MCF_G_SET_LAZY(__MCF_crt_kernelbase, QueryInterruptTime);  /* win10 */

    /* Attach the main thread and make it joinable. The structure should
     * be all zeroes so no initialization is necessary.  */
    __MCF_thread_attach_foreign(__MCF_g->__main_thread);
    _MCF_atomic_store_32_rel(__MCF_g->__main_thread->__nref, 2);
  }

__MCF_DLLEXPORT
void
__MCF_gthread_on_thread_exit(void)
  {
    __MCF_SEH_DEFINE_TERMINATE_FILTER;
    _MCF_thread* self = __MCF_crt_TlsGetValue(__MCF_g->__tls_index);
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
    while(__MCF_dtor_queue_pop(&elem, self->__atexit_queue, __MCF_nullptr) == 0)
      __MCF_invoke_cxa_dtor(elem.__dtor, elem.__this);

    while(self->__tls_table->__begin) {
      /* Call destructors of TLS keys. The TLS table may be modified by
       * destructors, so swap it out first.  */
      __MCF_tls_element* tls_begin = self->__tls_table->__begin;
      __MCF_tls_element* tls_end = self->__tls_table->__end;

      self->__tls_table->__begin = __MCF_nullptr;
      self->__tls_table->__end = __MCF_nullptr;
      self->__tls_table->__size_hint = 0;

      while(tls_begin != tls_end) {
        tls_end --;
        _MCF_tls_key* tkey = tls_end->__key_opt;
        if(!tkey)
          continue;

        /* POSIX requires that the destructor is called only when the key has
         * not been deleted and the value is not a null pointer.  */
        if(!_MCF_atomic_load_8_rlx(tkey->__deleted) && tkey->__dtor_opt && tls_end->__value_opt)
          __MCF_invoke_cxa_dtor(tkey->__dtor_opt, tls_end->__value_opt);

        _MCF_tls_key_drop_ref_nonnull(tkey);
      }

      /* Deallocate the table which should be empty now.  */
      __MCF_mfree_nonnull(tls_begin);
    }

    /* Poison this value.  */
    TlsSetValue(__MCF_g->__tls_index, __MCF_BAD_PTR);
    _MCF_thread_drop_ref_nonnull(self);
  }

/* These are constants that have to be initialized at load time. The
 * initializers prevent them from being placed into the`.bss` section.  */
GUID __MCF_crt_gthread_guid = __MCF_GUID(9FB2D15C,C5F2,4AE7,868D,2769591B8E92);
HANDLE __MCF_crt_heap = __MCF_BAD_PTR;
double __MCF_crt_pf_recip = 1;
SYSTEM_INFO __MCF_crt_sysinfo = { .dwPageSize = 1 };
HMODULE __MCF_crt_kernelbase = __MCF_BAD_PTR;
HMODULE __MCF_crt_ntdll = __MCF_BAD_PTR;
decltype_TlsGetValue2* __MCF_crt_TlsGetValue = __MCF_BAD_PTR;

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
    ULONG dummy1;
    HMODULE dummy2 = reserved;

    switch(reason)
      {
      case DLL_PROCESS_ATTACH:
        __MCF_gthread_initialize_globals();
        dummy1 = GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
        __MCF_CHECK(GetModuleHandleExW(dummy1, instance, &dummy2));
        __MCF_CHECK(VirtualProtect((void*) &__MCF_g, sizeof(__MCF_g), PAGE_READONLY, &dummy1));
        return 1;

      case DLL_THREAD_DETACH:
        __MCF_gthread_on_thread_exit();
        return 1;

      default:
        return 1;
      }
  }

__declspec(dllexport)
void*
memcpy(void* restrict dst, const void* restrict src, size_t size)
  {
    return __MCF_mcopy(dst, src, size);
  }

__declspec(dllexport)
void*
memmove(void* dst, const void* src, size_t size)
  {
    return ((uintptr_t) dst - (uintptr_t) src >= size)
           ? __MCF_mcopy(dst, src, size)
           : __MCF_mcopy_backward(dst, src, size);
  }

__declspec(dllexport)
void*
memset(void* dst, int val, size_t size)
  {
    return __MCF_mfill(dst, val, size);
  }

__declspec(dllexport)
int
memcmp(const void* src, const void* dst, size_t size)
  {
    return __MCF_mcompare(src, dst, size);
  }

#if defined _MSC_VER
/* Microsoft LINK requires this for a reason.  */
const int _fltused __attribute__((__used__)) = 0x9875;
#endif

#if defined __i386__
/* On x86-32, the load config directory contains the address and size of the
 * exception handler table. Exception handlers that are not in this table
 * will be rejected by the system. `__MCF_i386_se_handler_table` points to an
 * array of RVAs to valid handlers, and the value of (not the value it points
 * to) `__MCF_i386_se_handler_count` is the number of handlers.  */
extern const ULONG __MCF_i386_se_handler_table[];
extern char __MCF_i386_se_handler_count[];
__asm__ (
".section .rdata, \"dr\"  \n"
"  .p2align 2  \n"
"___MCF_i386_se_handler_table:  \n"
"  .rva @__MCF_safeseh__seh_top  \n"
"  .rva @__MCF_safeseh__gthr_once  \n"
".equiv ___MCF_i386_se_handler_count, (. - ___MCF_i386_se_handler_table) / 4  \n"
".text  \n"
);
#endif

#if defined __arm64ec__
/* This section has been heavily modified from 'chpe.S' from mingw-w64. Only
 * symbols that are documented by Microsoft are kept. Original code is declared
 * to be in the Public Domain.  */
extern const ULONG __MCF_arm64ec_chpe_metadata[];
__asm__ (
".section .rdata, \"dr\"  \n"
"  .p2align 2  \n"
"__MCF_arm64ec_chpe_metadata:  \n"
"  .long 1  \n" /* Version */
"  .rva __hybrid_code_map  \n"
"  .long __hybrid_code_map_count  \n"
"  .rva __x64_code_ranges_to_entry_points  \n"
"  .rva __arm64x_redirection_metadata  \n"
"  .rva __os_arm64x_dispatch_call_no_redirect  \n"
"  .rva __os_arm64x_dispatch_ret  \n"
"  .rva __os_arm64x_check_call  \n"
"  .rva __os_arm64x_check_icall  \n"
"  .rva __os_arm64x_check_icall_cfg  \n"
"  .rva __arm64x_native_entrypoint  \n"
"  .rva __hybrid_auxiliary_iat  \n"
"  .long __x64_code_ranges_to_entry_points_count  \n"
"  .long __arm64x_redirection_metadata_count  \n"
"  .rva __os_arm64x_get_x64_information  \n"
"  .rva __os_arm64x_set_x64_information  \n"
"  .rva __arm64x_extra_rfe_table  \n"
"  .long __arm64x_extra_rfe_table_size  \n"
"  .rva __os_arm64x_x64_jump  \n"
"  .rva __hybrid_auxiliary_iat_copy  \n"
"  .rva __hybrid_auxiliary_delayload_iat  \n"
"  .rva __hybrid_auxiliary_delayload_iat_copy  \n"
"  .long __hybrid_image_info_bitfield  \n"
"  .rva __os_arm64x_helper3  \n"
"  .rva __os_arm64x_helper4  \n"
"  .rva __os_arm64x_helper5  \n"
"  .rva __os_arm64x_helper6  \n"
"  .rva __os_arm64x_helper7  \n"
"  .rva __os_arm64x_helper8  \n"
/* These are pointers to helper routines, which will be filled after the image
 * is loaded by the operating system.  */
".section .00cfg, \"dr\"  \n"
"  .p2align 3  \n"
".globl __os_arm64x_dispatch_call_no_redirect  \n"
"__os_arm64x_dispatch_call_no_redirect:  \n"
"  .quad 0  \n"
".globl __os_arm64x_dispatch_ret  \n"
"__os_arm64x_dispatch_ret:  \n"
"  .quad 0  \n"
".globl __os_arm64x_check_call  \n"
"__os_arm64x_check_call:  \n"
"  .quad 0  \n"
".globl __os_arm64x_check_icall  \n"
"__os_arm64x_check_icall:  \n"
"  .quad 0  \n"
".globl __os_arm64x_check_icall_cfg  \n"
"__os_arm64x_check_icall_cfg:  \n"
"  .quad 0  \n"
".globl __os_arm64x_get_x64_information  \n"
"__os_arm64x_get_x64_information:  \n"
"  .quad 0  \n"
".globl __os_arm64x_set_x64_information  \n"
"__os_arm64x_set_x64_information:  \n"
"  .quad 0  \n"
".globl __os_arm64x_x64_jump  \n"
"__os_arm64x_x64_jump:  \n"
"  .quad 0  \n"
".globl __os_arm64x_helper3  \n"
"__os_arm64x_helper3:  \n"
"  .quad 0  \n"
".globl __os_arm64x_helper4  \n"
"__os_arm64x_helper4:  \n"
"  .quad 0  \n"
".globl __os_arm64x_helper5  \n"
"__os_arm64x_helper5:  \n"
"  .quad 0  \n"
".globl __os_arm64x_helper6  \n"
"__os_arm64x_helper6:  \n"
"  .quad 0  \n"
".globl __os_arm64x_helper7  \n"
"__os_arm64x_helper7:  \n"
"  .quad 0  \n"
".globl __os_arm64x_helper8  \n"
"__os_arm64x_helper8:  \n"
"  .quad 0  \n"
".text  \n"
"  .p2align 2  \n"
/* This is the ARM64EC Adjustor Thunk. Calls to this function are synthesized
 * by the compiler.  */
".globl __icall_helper_arm64ec  \n"
".def __icall_helper_arm64ec; .scl 2; .type 32; .endef  \n"
"__icall_helper_arm64ec:  \n"
".seh_proc __icall_helper_arm64ec  \n"
"  stp fp, lr, [sp, -16]!  \n"
".seh_save_fplr_x 16  \n"
"  mov fp, sp  \n"
".seh_set_fp  \n"
".seh_endprologue  \n"
"  adrp x16, __os_arm64x_check_icall  \n"
"  ldr x16, [x16, :lo12:__os_arm64x_check_icall]  \n"
"  blr x16  \n"
".seh_startepilogue  \n"
"  ldp fp, lr, [sp], 16  \n"
".seh_save_fplr_x 16  \n"
".seh_endepilogue  \n"
"  br x11  \n"
".seh_endproc  \n"
/* This is a common Adjustor Thunk for callback functions that return either
 * values in RAX, or void.  */
".globl __MCF_arm64ec_icall_helper_p  \n"
".def __MCF_arm64ec_icall_helper_p; .scl 2; .type 32; .endef  \n"
"__MCF_arm64ec_icall_helper_p:  \n"
"  adr x10, do_arm64ec_icall_exit_thunk_p  \n"
"  b __icall_helper_arm64ec  \n"
".def do_arm64ec_icall_exit_thunk_p; .scl 3; .type 32; .endef  \n"
"do_arm64ec_icall_exit_thunk_p:  \n"
".seh_proc do_arm64ec_icall_exit_thunk_p  \n"
"  stp fp, lr, [sp, -48]!  \n"
".seh_save_fplr_x 48  \n"
"  mov fp, sp  \n"
".seh_set_fp  \n"
".seh_endprologue  \n"
"  adrp x16, __os_arm64x_dispatch_call_no_redirect  \n"
"  ldr x16, [x16, :lo12:__os_arm64x_dispatch_call_no_redirect]  \n"
"  blr x16  \n"
"  mov x0, x8  \n"
".seh_startepilogue  \n"
"  ldp fp, lr, [sp], 48  \n"
".seh_save_fplr_x 48  \n"
".seh_endepilogue  \n"
"  ret  \n"
".seh_endproc  \n"
);
#endif

struct _IMAGE_LOAD_CONFIG_DIRECTORY_10_0_26100_0
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
  }
const _load_config_used __attribute__((__used__)) =
  {
    .Size = sizeof(_load_config_used),
    .DependentLoadFlags = LOAD_LIBRARY_SEARCH_SYSTEM32,
#if defined __i386__
    .SEHandlerTable = (ULONG_PTR) __MCF_i386_se_handler_table,
    .SEHandlerCount = (ULONG_PTR) __MCF_i386_se_handler_count,
#endif
#if defined __arm64ec__
    .CHPEMetadataPointer = (ULONG_PTR) __MCF_arm64ec_chpe_metadata,
#endif
  };

#else  /* __MCF_IN_DLL  */

/* When building the static library, invoke common routines from a TLS
 * callback.  */
static __MCF_REALIGN_SP
void
__stdcall
do_tls_callback(PVOID module, ULONG reason, LPVOID reserved)
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
__attribute__((__section__(".CRT$XLB"), __used__))
const PIMAGE_TLS_CALLBACK __MCF_crt_xl_b = do_tls_callback;

#endif  /* __MCF_IN_DLL  */
