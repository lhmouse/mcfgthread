/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2025 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_XGLOBALS_
#define __MCFGTHREAD_XGLOBALS_

#include "fwd.h"
#include "thread.h"
#include "mutex.h"
#include "cond.h"
#include "dtor_queue.h"
#include <minwindef.h>
#include <winnt.h>
#include <winternl.h>
#include <winerror.h>
#include <sysinfoapi.h>
#include <heapapi.h>

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#  error Windows platforms are assumed to be little-endian.
#endif

#ifdef __cplusplus
#  error This internal header is for C only.
#endif

#ifndef __MCF_XGLOBALS_IMPORT
#  define __MCF_XGLOBALS_IMPORT
#  define __MCF_XGLOBALS_INLINE  __MCF_GNU_INLINE
#  define __MCF_XGLOBALS_READONLY   const
#endif

/* Hard-code these.  */
#undef GetCurrentProcess
#undef GetCurrentThread

#define GetCurrentProcess()  ((HANDLE) -1)
#define GetCurrentThread()   ((HANDLE) -2)

/* Undefine macros that redirect to standard C functions, so the ones from
 * system DLLs will be called.  */
#undef RtlCopyMemory
#undef RtlMoveMemory
#undef RtlFillMemory
#undef RtlZeroMemory
#undef RtlEqualMemory

NTSYSAPI void NTAPI RtlMoveMemory(void* dst, const void* src, SIZE_T size);
NTSYSAPI void NTAPI RtlFillMemory(void* dst, SIZE_T size, int c);
NTSYSAPI void NTAPI RtlZeroMemory(void* dst, SIZE_T size);

NTSYSAPI ULONG NTAPI RtlNtStatusToDosError(NTSTATUS status);
NTSYSAPI ULONG NTAPI RtlNtStatusToDosErrorNoTeb(NTSTATUS status) __MCF_FN_CONST;

/* Define read-only data that must be placed in `.rdata` despite
 * `-fdata-sections`.  */
#define __MCF_CRT_RDATA  __attribute__((__used__, __section__(".rdata")))
#define __MCF_CRT_XL(x)  __attribute__((__used__, __section__(".CRT$XL" #x)))

/* Initialize a GUID in the canonical form.  */
#define __MCF_GUID(a8,b4,c4,d4,e12)  \
    ((GUID) { 0x##a8, 0x##b4, 0x##c4, { (0x##d4 >> 8) & 0xFF, 0x##d4 & 0xFF,  \
              (0x##e12 >> 40) & 0xFF, (0x##e12 >> 32) & 0xFF, (0x##e12 >> 24) & 0xFF,  \
              (0x##e12 >> 16) & 0xFF, (0x##e12 >> 8) & 0xFF, 0x##e12 & 0xFF } })

/* Define a non-zero but invalid value. This can be used to mark a pointer
 * to freed memory, or to prevent a static pointer from being placed into
 * the `.bss` section.  */
#define __MCF_BAD_PTR  ((void*) -127)

/* Define a macro that is identical to `RTL_CONSTANT_STRING()` as we are not
 * willing to include <ntdef.h> here.  */
#define __MCF_NT_STRING_INIT(s)  \
    { .Length = sizeof(s) - sizeof(*(s)),  \
      .MaximumLength = sizeof(s),  \
      .Buffer = (void*) ((s) + __MCF_STATIC_ASSERT_0(  \
         !__builtin_types_compatible_p(__typeof__(1+&*(s)), __typeof__(s))))  }

/* Define macros and types for lazy binding. If a symbol cannot be found
 * during process startup, it shall be initialized to a null pointer.  */
typedef void __stdcall decltype_GetSystemTimePreciseAsFileTime(FILETIME*);
typedef void __stdcall decltype_QueryInterruptTime(ULONGLONG*);
typedef LPVOID __stdcall decltype_TlsGetValue2(ULONG);

#define __MCF_LAZY_D_(name)   decltype_##name* __f_##name
#define __MCF_LAZY_P_(name)   __f_##name

#define __MCF_LAZY_LOAD(out, dll, name)  \
    ({ decltype_##name* __temp1 = __MCF_nullptr;  \
       if(dll) __temp1 = __MCF_CAST_PTR(decltype_##name, GetProcAddress(dll, #name));  \
       if(__temp1) *(out) = __temp1;  \
       __temp1;  })

/* Declare helper functions here.  */
typedef struct __MCF_winnt_timeout __MCF_winnt_timeout;
typedef struct __MCF_crt_xglobals __MCF_crt_xglobals;

__MCF_XGLOBALS_IMPORT
EXCEPTION_DISPOSITION
__cdecl
__MCF_seh_top(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx);

#if defined __i386__

/* On x86, SEH is stack-based.  */
__MCF_ALWAYS_INLINE
EXCEPTION_REGISTRATION_RECORD*
__MCF_i386_seh_install(EXCEPTION_REGISTRATION_RECORD* record)
  {
    __MCF_TEB_LOAD_32_ABS(&(record->Next), 0);
    __MCF_TEB_STORE_32_ABS(0, record);
    return record;
  }

__MCF_ALWAYS_INLINE
void
__MCF_i386_seh_cleanup(EXCEPTION_REGISTRATION_RECORD* const* ref)
  {
    __MCF_TEB_STORE_32_ABS(0, (*ref)->Next);
  }

#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
    EXCEPTION_REGISTRATION_RECORD* const __MCF_i386_seh_node__  \
            __attribute__((__cleanup__(__MCF_i386_seh_cleanup)))  \
      = __MCF_i386_seh_install(&(EXCEPTION_REGISTRATION_RECORD) {  \
           .Handler = __MCF_CAST_PTR(EXCEPTION_ROUTINE, __MCF_seh_top) })

__MCF_ALWAYS_INLINE
void
__MCF_invoke_cxa_dtor(__MCF_cxa_dtor_any_ dtor, void* arg)
  {
    /* The argument is passed both via the ECX register and on the stack, to
     * allow both `__cdecl` and `__thiscall` functions to work properly.  */
    typedef void omni_type(int, int, void*, void*) __attribute__((__regparm__(3)));
    int eax, edx;
    __asm__ ("" : "=a"(eax), "=d"(edx));  /* dummy */
    (* __MCF_CAST_PTR(omni_type, dtor.__cdecl_ptr)) (eax, edx, arg, arg);
  }

/* GCC assumes that ESP is always aligned to a 16-byte boundary, but for MSVC
 * it might not be aligned, so it has to be enforced, otherwise SSE instructions
 * may fault.  */
#  define __MCF_REALIGN_SP    __attribute__((__force_align_arg_pointer__))

#else

/* Otherwise, SEH is table-based. This code must work on both x86_64 and ARM64,
 * as well as ARM64EC.  */
#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
    __asm__ (".seh_handler __MCF_seh_top, @except, @unwind")  /* no semicolon  */

__MCF_ALWAYS_INLINE
void
__MCF_invoke_cxa_dtor(__MCF_cxa_dtor_any_ dtor, void* arg)
  {
    (* dtor.__cdecl_ptr) (arg);
  }

#  define __MCF_REALIGN_SP    /* nothing */

#endif  /* defined __i386__ */

/* This structure contains timeout values that will be passed to NT syscalls.  */
struct __MCF_winnt_timeout
  {
    LARGE_INTEGER __li;
    ULONGLONG __since;
  };

__MCF_XGLOBALS_IMPORT
void
__MCF_initialize_winnt_timeout_v3(__MCF_winnt_timeout* to, const int64_t* ms_opt);

__MCF_XGLOBALS_IMPORT
void
__MCF_adjust_winnt_timeout_v3(__MCF_winnt_timeout* to);

__MCF_XGLOBALS_IMPORT
void
__MCF_batch_release_common(const void* key, size_t count);

/* Copy a block of memory forward, like `memcpy()`.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy(void* dst, const void* src, size_t size);

/* Copy a block of memory backward.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy_backward(void* dst, const void* src, size_t size);

/* Fill a block of memory with the given byte, like `memset()`.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mfill(void* dst, int val, size_t size);

/* Fill a block of memory with zeroes, like `bzero()`.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mzero(void* dst, size_t size);

/* Compare two blocks of memory, like `memcmp()`.  */
__MCF_XGLOBALS_INLINE __MCF_FN_PURE
int
__cdecl
__MCF_mcompare(const void* src, const void* cmp, size_t size);

/* Check whether two blocks of memory compare equal.  */
__MCF_XGLOBALS_INLINE __MCF_FN_PURE
bool
__cdecl
__MCF_mequal(const void* src, const void* cmp, size_t size);

/* Allocate a block of zeroed memory, like `calloc()`.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_0(size_t size) __attribute__((__malloc__, __alloc_size__(1)));

/* Set the size a block of memory in place, like `truncate()` on files.
 * If the existent block should be extended, vacuum bytes are filled with
 * zeroes.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_mresize_0(void* ptr, size_t size) __attribute__((__alloc_size__(2)));

/* Re-allocate a block of memory, like `realloc()`. If the existent
 * block should be extended, vacuum bytes are filled with zeroes.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_mrealloc_0(void** pptr, size_t size) __attribute__((__alloc_size__(2)));

/* Allocate a copy of a block of memory, like `malloc()` followed by
 * `memcpy()`.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_copy(const void* data, size_t size) __attribute__((__alloc_size__(2)));

/* Get the size of an allocated block, like `malloc_usable_size()`.  */
__MCF_XGLOBALS_INLINE
size_t
__MCF_msize(const void* ptr) __MCF_FN_PURE;

/* Free a block of memory, like `free()`, except that the argument shall not
 * be a null pointer.  */
__MCF_XGLOBALS_INLINE
void
__MCF_mfree_nonnull(void* ptr);

/* These functions set the last error code and return the second argument.
 * They should be subject to tail-call optimization.  */
__MCF_XGLOBALS_IMPORT __MCF_FN_COLD
int
__MCF_win32_error_i(ULONG code, int val);

__MCF_XGLOBALS_IMPORT __MCF_FN_COLD
void*
__MCF_win32_error_p(ULONG code, void* ptr);

__MCF_XGLOBALS_IMPORT __MCF_FN_COLD
void*
__MCF_win32_ntstatus_p(NTSTATUS status, void* ptr);

/* These functions are declared here for the sake of completeness, and are not
 * meant to be called directly.  */
__MCF_XGLOBALS_IMPORT
void
__MCF_run_static_dtors(_MCF_mutex* mtx, __MCF_dtor_queue* queue, void* dso);

__MCF_XGLOBALS_IMPORT
void
__MCF_gthread_initialize_globals(void);

__MCF_XGLOBALS_IMPORT
void
__MCF_gthread_on_thread_exit(void);

/* Declare global data.  */
struct __MCF_crt_xglobals
  {
    __MCF_crt_xglobals* __self_ptr;
    uint32_t __self_size;
    uint32_t __tls_index;

    /* the static thread object  */
    _MCF_thread __main_thread[1];

    /* `atexit()` support  */
    _MCF_mutex __exit_mtx[1];
    __MCF_dtor_queue __exit_queue[1];

    /* `at_quick_exit()` support  */
    _MCF_mutex __quick_exit_mtx[1];
    __MCF_dtor_queue __quick_exit_queue[1];

    /* mutex support  */
    __MCF_ALIGNED(64) bool __mutex_spin_field[2048];

    /* thread suspension support  */
    _MCF_cond __interrupt_cond[1];

    /* WARNING: Fields hereinafter must be accessed via `__MCF_G_FIELD_OPT`!  */
    __MCF_LAZY_D_(GetSystemTimePreciseAsFileTime);
    __MCF_LAZY_D_(QueryInterruptTime);
    _MCF_mutex __thread_oom_mtx[1];
    _MCF_thread __thread_oom_self_st;
  };

/* Ensure we don't mess things up.  */
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __self_ptr) == 0);
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __self_size) == __MCF_64_32(8, 4));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __tls_index) == __MCF_64_32(12, 8));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __main_thread) == __MCF_64_32(16, 16));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __exit_mtx) == __MCF_64_32(1616, 816));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __exit_queue) == __MCF_64_32(1624, 820));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __quick_exit_mtx) == __MCF_64_32(3152, 1584));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __quick_exit_queue) == __MCF_64_32(3160, 1588));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __mutex_spin_field) == __MCF_64_32(4736, 2368));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __interrupt_cond) == __MCF_64_32(6784, 4416));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __f_GetSystemTimePreciseAsFileTime) == __MCF_64_32(6792, 4420));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __f_QueryInterruptTime) == __MCF_64_32(6800, 4424));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __thread_oom_mtx) == __MCF_64_32(6808, 4428));
__MCF_STATIC_ASSERT(offsetof(__MCF_crt_xglobals, __thread_oom_self_st) == __MCF_64_32(6816, 4432));

/* These are constants that have to be initialized at load time.  */
extern GUID __MCF_XGLOBALS_READONLY __MCF_crt_gthread_guid;
extern SYSTEM_INFO __MCF_XGLOBALS_READONLY __MCF_crt_sysinfo;
extern HANDLE __MCF_XGLOBALS_READONLY __MCF_crt_heap;
extern double __MCF_XGLOBALS_READONLY __MCF_crt_pf_recip;
extern HMODULE __MCF_XGLOBALS_READONLY __MCF_crt_kernelbase;
extern HMODULE __MCF_XGLOBALS_READONLY __MCF_crt_ntdll;
extern decltype_TlsGetValue2* __MCF_XGLOBALS_READONLY __MCF_crt_TlsGetValue;

/* This is a pointer to the process-specific data.
 * As mcfgthread may be linked statically by user DLLs, we must ensure that, in
 * the same process, all instances of this pointer point to the same object. This
 * is achieved by having them point to a named file mapping, which is created
 * by the current process with exclusive access, and whose name is generated from
 * its process ID. Additional randomness is introduced to prevent the name from
 * being predicted.  */
extern __MCF_crt_xglobals* __MCF_XGLOBALS_READONLY restrict __MCF_g;

/* As `__MCF_crt_xglobals` is shared between all static and shared instances of
 * this library within a single process, we have to involve sort of versioning.  */
#define __MCF_G_FIELD_OPT(field)  \
    ((__MCF_g->__self_size >= offsetof(__MCF_crt_xglobals, field) + sizeof(__MCF_g->field))  \
     ? &(__MCF_g->field)  \
     : __MCF_nullptr)

#define __MCF_G_HAS_LAZY(name)   (__MCF_G_FIELD_OPT(__MCF_LAZY_P_(name)) && __MCF_g->__MCF_LAZY_P_(name))
#define __MCF_G_LAZY(name)           (*(__MCF_g->__MCF_LAZY_P_(name)))
#define __MCF_G_SET_LAZY(dll, name)    __MCF_LAZY_LOAD(&(__MCF_g->__MCF_LAZY_P_(name)), dll, name)

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy(void* dst, const void* src, size_t size)
  {
    __MCF_ASSERT((uintptr_t) dst - (uintptr_t) src >= size);
#if defined __i386__ || (defined __amd64__ && !defined __arm64ec__)
    PVOID edi, ecx, esi;
    __asm__ volatile (
      "rep movsb"
      : "=D"(edi), "=c"(ecx), "=S"(esi)
      : "0"(dst), "1"(size), "2"(src)
      : "memory"
    );
#else
    RtlMoveMemory(dst, src, size);
#endif
    return dst;
  }

__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy_backward(void* dst, const void* src, size_t size)
  {
    __MCF_ASSERT((uintptr_t) src - (uintptr_t) dst >= size);
#if defined __i386__ || (defined __amd64__ && !defined __arm64ec__)
    PVOID edi, ecx, esi;
    __asm__ volatile (
      "std; "
      "rep movsb; "
      "cld; "
      : "=D"(edi), "=c"(ecx), "=S"(esi)
      : "0"((char*) dst + size - 1), "1"(size), "2"((char*) src + size - 1)
      : "memory"
    );
#else
    RtlMoveMemory(dst, src, size);
#endif
    return dst;
  }

__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mfill(void* dst, int val, size_t size)
  {
#if defined __i386__ || (defined __amd64__ && !defined __arm64ec__)
    PVOID edi, ecx;
    __asm__ volatile (
      "rep stosb"
      : "=D"(edi), "=c"(ecx)
      : "0"(dst), "1"(size), "a"(val)
      : "memory"
    );
#else
    RtlFillMemory(dst, size, val);
#endif
    return dst;
  }

__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mzero(void* dst, size_t size)
  {
#if defined __i386__ || (defined __amd64__ && !defined __arm64ec__)
    PVOID edi, ecx;
    __asm__ volatile (
      "rep stosb"
      : "=D"(edi), "=c"(ecx)
      : "0"(dst), "1"(size), "a"(0)
      : "memory"
    );
#else
    RtlZeroMemory(dst, size);
#endif
    return dst;
  }

__MCF_ALWAYS_INLINE
int
__cdecl
__MCF_do_std_compare(const void* src, const void* src_end, const void* cmp)
  {
    typedef const volatile unsigned char* PCVBYTE;
    PCVBYTE sptr = src, cptr = cmp;
    int diff = 0;
    while((diff == 0) && (sptr != src_end))
      diff = *(sptr ++) - *(cptr ++);
    return diff;
  }

__MCF_XGLOBALS_INLINE
int
__cdecl
__MCF_mcompare(const void* src, const void* cmp, size_t size)
  {
    int diff;
#if defined __i386__ || (defined __amd64__ && !defined __arm64ec__)
    /* Perform string comparison with hardware.  */
    PVOID esi, edi, ecx;
    __asm__ (
      "xor eax, eax; "  /* clear ZF and CF  */
      "repz cmpsb; "    /* compare DS:[ESI] with ES:[EDI]  */
      "setnz al; "      /* EAX = 0 if equal, 1 if less or greater  */
      "sbb ecx, ecx; "  /* ECX = 0 if equal or greater, -1 if less  */
      "or eax, ecx; "
      : "=a"(diff), "=S"(esi), "=c"(ecx), "=D"(edi)
      : "1"(src), "2"(size), "3"(cmp)
      : "memory"
    );
#else
    diff = __MCF_do_std_compare(src, (const char*) src + size, cmp);
#endif
    return diff;
  }

__MCF_XGLOBALS_INLINE
bool
__cdecl
__MCF_mequal(const void* src, const void* cmp, size_t size)
  {
    bool eq;
#if defined __i386__ || (defined __amd64__ && !defined __arm64ec__)
    /* Perform string comparison with hardware.  */
    PVOID esi, edi, ecx;
    __asm__ (
      "test ecx, ecx; " /* clear ZF if there is no input  */
      "repz cmpsb; "    /* compare DS:[ESI] with ES:[EDI]  */
      : "=@ccz"(eq), "=S"(esi), "=c"(ecx), "=D"(edi)
      : "1"(src), "2"(size), "3"(cmp)
      : "memory"
    );
#else
    eq = __MCF_do_std_compare(src, (const char*) src + size, cmp) == 0;
#endif
    return eq;
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_0(size_t size)
  {
    return HeapAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, size);
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_mresize_0(void* ptr, size_t size)
  {
    return HeapReAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY | HEAP_REALLOC_IN_PLACE_ONLY, ptr, size);
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_mrealloc_0(void** pptr, size_t size)
  {
    void* ptr = HeapReAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, *pptr, size);
    if(ptr)
      *pptr = ptr;
    return ptr;
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_copy(const void* data, size_t size)
  {
    void* ptr = HeapAlloc(__MCF_crt_heap, 0, size);
    if(ptr)
      __MCF_mcopy(ptr, data, size);
    return ptr;
  }

__MCF_XGLOBALS_INLINE
size_t
__MCF_msize(const void* ptr)
  {
    size_t size = HeapSize(__MCF_crt_heap, 0, ptr);
    __MCF_ASSERT(size != (size_t) -1);
    return size;
  }

__MCF_XGLOBALS_INLINE
void
__MCF_mfree_nonnull(void* ptr)
  {
    __MCF_ASSERT(ptr != __MCF_nullptr);
#ifdef __MCF_DEBUG
    size_t size = HeapSize(__MCF_crt_heap, 0, ptr);
    __MCF_ASSERT(size != (size_t) -1);
    __MCF_mfill(ptr, 0xFE, size);
#endif
    int succ = HeapFree(__MCF_crt_heap, 0, ptr);
    __MCF_ASSERT(succ);
  }

/* Gets a handle to the directory for all named objects (mutexes, semaphores,
 * events, etc.) for the current session. The handle is cached in KERNEL32.DLL
 * and must not be closed.  */
NTSYSAPI NTSTATUS NTAPI BaseGetNamedObjectDirectory(HANDLE* OutHandle);

__MCF_ALWAYS_INLINE
HANDLE
__MCF_get_directory_for_named_objects(void)
  {
    HANDLE handle;
    NTSTATUS status = BaseGetNamedObjectDirectory(&handle);
    __MCF_ASSERT(NT_SUCCESS(status));
    return handle;
  }

/* Indicates whether the current process is being shut down. This function has
 * existed since at least Windows 7, but is only documented since Windows 10.  */
NTSYSAPI BOOLEAN NTAPI RtlDllShutdownInProgress(void);

__MCF_ALWAYS_INLINE
bool
__MCF_is_process_shutting_down(void)
  {
    return RtlDllShutdownInProgress();
  }

/* Create a named section of memory.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatesection  */
NTSYSAPI NTSTATUS NTAPI NtCreateSection(HANDLE* OutHandle, ACCESS_MASK DesiredAccess,
                                        OBJECT_ATTRIBUTES* Attributes, LARGE_INTEGER* MaximumSize,
                                        ULONG Protection, ULONG Allocation, HANDLE File);

__MCF_ALWAYS_INLINE
HANDLE
__MCF_create_named_section(OBJECT_ATTRIBUTES* Attributes, LONGLONG MaximumSize)
  {
    HANDLE handle;
    LARGE_INTEGER size = { .QuadPart = MaximumSize };
    NTSTATUS status = NtCreateSection(&handle, SECTION_ALL_ACCESS, Attributes, &size, PAGE_READWRITE, SEC_COMMIT, NULL);
    return NT_SUCCESS(status) ? handle : __MCF_win32_ntstatus_p(status, NULL);
  }

/* Creates another handle to an object.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwduplicateobject  */
NTSYSAPI NTSTATUS NTAPI NtDuplicateObject(HANDLE SourceProcess, HANDLE SourceHandle, HANDLE TargetProcess,
                                          HANDLE* TargetHandle, ACCESS_MASK DesiredAccess,
                                          ULONG HandleAttributes, ULONG Options);

__MCF_ALWAYS_INLINE
HANDLE
__MCF_duplicate_handle(HANDLE SourceHandle)
  {
    HANDLE handle;
    HANDLE process = GetCurrentProcess();
    NTSTATUS status = NtDuplicateObject(process, SourceHandle, process, &handle, 0, 0, DUPLICATE_SAME_ACCESS);
    return NT_SUCCESS(status) ? handle : __MCF_win32_ntstatus_p(status, NULL);
  }

/* Closes a handle to an object.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose  */
NTSYSAPI NTSTATUS NTAPI NtClose(HANDLE Handle);

__MCF_ALWAYS_INLINE
void
__MCF_close_handle(HANDLE Handle)
  {
    NTSTATUS status = NtClose(Handle);
    __MCF_ASSERT(NT_SUCCESS(status));
  }

/* Maps a named section of memory into the virtual address space.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwmapviewofsection  */
NTSYSAPI NTSTATUS NTAPI NtMapViewOfSection(HANDLE Section, HANDLE Process, PVOID* BaseAddress, ULONG_PTR ZeroBits,
                                           SIZE_T CommitSize, LARGE_INTEGER* Offset, SIZE_T* ViewSize,
                                           UINT SectionInherit, ULONG Allocation, ULONG Protection);

__MCF_ALWAYS_INLINE
void
__MCF_map_view_of_section(HANDLE Section, void** BaseAddress, size_t* ViewSize, bool Inheritable)
  {
    HANDLE process = GetCurrentProcess();
    UINT inherit = Inheritable ? 1U : 2U;  /* ViewShare : ViewUnmap */
    NTSTATUS status = NtMapViewOfSection(Section, process, BaseAddress, 0, 0, __MCF_nullptr,
                                         (SIZE_T*) ViewSize, inherit, 0, PAGE_READWRITE);
    __MCF_ASSERT(NT_SUCCESS(status));
  }

/* Unmaps a named section of memory from the virtual address space.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwunmapviewofsection  */
NTSYSAPI NTSTATUS NTAPI NtUnmapViewOfSection(HANDLE Process, PVOID BaseAddress);

__MCF_ALWAYS_INLINE
void
__MCF_unmap_view_of_section(void* BaseAddress)
  {
    HANDLE process = GetCurrentProcess();
    NTSTATUS status = NtUnmapViewOfSection(process, BaseAddress);
    __MCF_ASSERT(NT_SUCCESS(status));
  }

/* Waits until an object is signaled.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwwaitforsingleobject  */
NTSYSAPI NTSTATUS NTAPI NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, LARGE_INTEGER* Timeout);

__MCF_ALWAYS_INLINE
int
__MCF_wait_for_single_object(HANDLE Handle, const __MCF_winnt_timeout* Timeout)
  {
    NTSTATUS status = NtWaitForSingleObject(Handle, false, (LARGE_INTEGER*) &(Timeout->__li));
    __MCF_ASSERT(NT_SUCCESS(status));
    return (status != STATUS_WAIT_0) ? -1 : 0;
  }

/* Suspends the calling thread for the given duration. This function is called by
 * `SleepEx()`.  */
NTSYSAPI NTSTATUS NTAPI NtDelayExecution(BOOLEAN Alertable, LARGE_INTEGER* Timeout);

__MCF_ALWAYS_INLINE
void
__MCF_sleep(const __MCF_winnt_timeout* Timeout)
  {
    NTSTATUS status = NtDelayExecution(false, (LARGE_INTEGER*) &(Timeout->__li));
    __MCF_ASSERT(NT_SUCCESS(status));
  }

/* Suspends the calling thread on the given keyed event object, until another
 * thread calls `NtReleaseKeyedEvent()` on the same object with the same key.
 * The lowest bit of the key must be zero.  */
NTSYSAPI NTSTATUS NTAPI NtWaitForKeyedEvent(HANDLE KeyedEvent, PVOID Key, BOOLEAN Alertable, LARGE_INTEGER* Timeout);

__MCF_ALWAYS_INLINE
int
__MCF_keyed_event_wait(const void* Key, const __MCF_winnt_timeout* Timeout)
  {
    NTSTATUS status = NtWaitForKeyedEvent(NULL, (PVOID) Key, false, (LARGE_INTEGER*) &(Timeout->__li));
    __MCF_ASSERT(NT_SUCCESS(status));
    return (status != STATUS_WAIT_0) ? -1 : 0;
  }

/* Wakes another thread which shall be waiting on the given keyed event object
 * for the given key. If no such thread exists, the calling thread is suspended
 * unless a matching thread calls `NtWaitForKeyedEvent()`. The lowest bit of
 * the key must be zero.  */
NTSYSAPI NTSTATUS NTAPI NtReleaseKeyedEvent(HANDLE KeyedEvent, PVOID Key, BOOLEAN Alertable, LARGE_INTEGER* Timeout);

__MCF_ALWAYS_INLINE
int
__MCF_keyed_event_signal(const void* Key, const __MCF_winnt_timeout* Timeout)
  {
    NTSTATUS status = NtReleaseKeyedEvent(NULL, (PVOID) Key, false, (LARGE_INTEGER*) &(Timeout->__li));
    __MCF_ASSERT(NT_SUCCESS(status));
    return (status != STATUS_WAIT_0) ? -1 : 0;
  }

/* Sends a hard-error LPC message to CSRSS.EXE. This function is useful in a DLL
 * entry-point function or a TLS callback, where it is not safe to make a call
 * to `MessageBox()`.  */
NTSYSAPI NTSTATUS NTAPI NtRaiseHardError(NTSTATUS Status, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
                                         ULONG_PTR* Parameters, ULONG ResponseOption, ULONG* Response);

__MCF_ALWAYS_INLINE
int
__MCF_show_service_notification(const UNICODE_STRING* caption, ULONG options, const UNICODE_STRING* text)
  {
    ULONG_PTR params[4] = { (ULONG_PTR) text, (ULONG_PTR) caption, options, 0 };
    ULONG response = 0;
    NTSTATUS status = NtRaiseHardError(0x50000018, 4, 0b0011, params, 1, &response);
    return !NT_SUCCESS(status) ? -1 : (int) response;
  }

#endif  /* __MCFGTHREAD_XGLOBALS_  */
