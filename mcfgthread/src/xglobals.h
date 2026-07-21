/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include "../fwd.h"
#include "../thread.h"
#include "../mutex.h"
#include "../cond.h"
#include "../dtor_queue.h"
#include "xwinternl.h"
#include <heapapi.h>
#include <winerror.h>
#include <sysinfoapi.h>

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#  error Windows platforms are assumed to be little-endian.
#endif

#ifdef __cplusplus
#  error This internal header is for C only.
#endif

/** Guarantee that `nullptr` is available for C.  */
#if 0 __MCF_C23(+1) __MCF_CXX11(+1) == 0
#  define nullptr   ((void*) __MCF_INTPTR_0)
#endif

/** Annotate a `return` statement that the previous expression has set the last
 * error code. This doesn't do anything at the moment.  */
#define __MCF_FORWARD_WIN32_ERROR(...)  (__VA_ARGS__)

/** Define data that must be placed in `.rdata` despite `-fdata-sections`.  */
#define __MCF_CRT_RDATA  __attribute__((__used__, __section__(".rdata")))
#define __MCF_CRT_XL(x)  __attribute__((__used__, __section__(".CRT$XL" #x)))

/** Initialize a GUID in the canonical form.  */
#define __MCF_GUID(a8,b4,c4,d4,e12)  \
    ((GUID){ 0x##a8, 0x##b4, 0x##c4, { 0x##d4 >> 8, 0x##d4 & 0xFF, 0x##e12 >> 40,  \
             (0x##e12 >> 32) & 0xFF, (0x##e12 >> 24) & 0xFF, (0x##e12 >> 16) & 0xFF,  \
             (0x##e12 >> 8) & 0xFF, 0x##e12 & 0xFF } })

/** Define a non-zero but invalid value. This can be used to mark a pointer
 * to freed memory, or to prevent a static pointer from being placed into
 * the `.bss` section.  */
#define __MCF_BAD_PTR  ((void*) -127)

/** Define a macro that is identical to `RTL_CONSTANT_STRING()` as we are not
 * willing to include <ntdef.h> here.  */
#define __MCF_NT_STRING_INIT(s)  \
    { .Length = sizeof(s) - sizeof(*(s)),  \
      .MaximumLength = sizeof(s),  \
      .Buffer = (void*) ((s) + __MCF_STATIC_ASSERT_0(  \
         !__builtin_types_compatible_p(__typeof__(1+&*(s)), __typeof__(s))))  }

/** This function is used as the exception handler of a `noexcept` function.  */
__MCF_XGLOBALS_IMPORT
EXCEPTION_DISPOSITION
__MCF_seh_top(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx);

/** This is an alternative handler for `main()` or `WinMain()`, which terminates
 * the current process in case of an exit unwind.  */
__MCF_XGLOBALS_IMPORT
EXCEPTION_DISPOSITION
__MCF_seh_process_top(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx);

/** This is an alternative handler for threads, which terminates the current
 * thread in case of an exit unwind.  */
__MCF_XGLOBALS_IMPORT
EXCEPTION_DISPOSITION
__MCF_seh_thread_top(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx);

#if defined __MCF_M_X8632

/** Removes an SEH node from the linked list of handlers.  */
__MCF_ALWAYS_INLINE
void
__MCF_i386_seh_cleanup(const void* ref)
  {
    __MCF_teb_store_ptr(0, *(const intptr_t*) ref);
  }

/** On x86, SEH is stack-based. Each handler will be attached to the scope of
 * a local record object.  */
#  define __MCF_USING_SEH_HANDLER(fn)  \
    const intptr_t  \
      __MCF_i386_seh_record[] __attribute__((__cleanup__(__MCF_i386_seh_cleanup)))  \
        = { __MCF_teb_load_ptr(0), (intptr_t) (fn) },  \
      __MCF_i386_seh_record_dummy __attribute__((__unused__))  \
        = (__MCF_teb_store_ptr(0, (intptr_t) &__MCF_i386_seh_record), 0)  /* no semicolon  */

/** Some old code assumes that ESP is always aligned to a 16-byte boundary,
 * but that's not guaranteed for callbacks from system DLLs, so it has to be
 * enforced; otherwise SSE instructions may fault.  */
#  define __MCF_REALIGN_SP    __attribute__((__force_align_arg_pointer__, __noinline__))

/** Invokes `dtor`, passing `arg` both via ECX and on the stack, to allow both
 * `__cdecl` and `__thiscall` functions to work properly.  */
__MCF_ALWAYS_INLINE
void
__MCF_invoke_cxa_dtor(__MCF_cxa_dtor_any_ dtor, void* arg)
  {
    register int eax __asm__("eax"), edx __asm__("edx");
    __asm__ ("" : "=r"(eax), "=r"(edx));
    typedef __attribute__((__regparm__(3))) void Xfn(int, int, void*, void*);
    (* __MCF_CAST_PTR(Xfn, dtor.__cdecl_ptr)) (eax, edx, arg, arg);
  }

#else  /* !defined __MCF_M_X8632 */

/** SEH is table-based. Only x86-64 supports pure unwind handlers that are not
 * exception handlers; for simplicity, we use only dual handlers. This also
 * works on ARM64 and ARM64EC.  */
#  define __MCF_USING_SEH_HANDLER(fn)  \
    __asm__ (".seh_handler %c0, @except, @unwind" : : "i"(fn))  /* no semicolon  */

/** The stack is always aligned to 16-byte boundaries, as required by the ABI,
 * so this expands to nothing.  */
#  define __MCF_REALIGN_SP

/** There is only ever one calling convention.  */
__MCF_ALWAYS_INLINE
void
__MCF_invoke_cxa_dtor(__MCF_cxa_dtor_any_ dtor, void* arg)
  {
    (* dtor.__cdecl_ptr) (arg);
  }

#endif  /* !defined __MCF_M_X8632 */

typedef struct __MCF_winnt_timeout __MCF_winnt_timeout;
typedef struct __MCF_crt_xglobals __MCF_crt_xglobals;

/** This structure contains timeout values that will be passed to NT syscalls.  */
struct __MCF_winnt_timeout
  {
    LARGE_INTEGER li;
    ULONGLONG since;
  };

/** Converts a timeout in milliseconds to a native Windows NT timeout in 100
 * nanoseconds, and initializes `since` for a relative timeout.  */
__MCF_XGLOBALS_IMPORT
void
__MCF_initialize_winnt_timeout_v3(__MCF_winnt_timeout* to, const int64_t* ms_opt);

/** Adjusts `*to` for an interrupted wait operation, so it may be resumed.  */
__MCF_XGLOBALS_IMPORT
void
__MCF_adjust_winnt_timeout_v3(__MCF_winnt_timeout* to);

/** Checks whether it is safe to suspend the current thread. In `DllMain()` or
 * TLS callback upon `DLL_PROCESS_DETACH`, where all other threads have been
 * terminated by the system, it may not be safe to wait, and this function
 * terminates the current process.  */
__MCF_XGLOBALS_IMPORT
void
__MCF_check_wait_safety(const __MCF_winnt_timeout* to);

__MCF_XGLOBALS_IMPORT
void
__MCF_batch_release_common(const void* key, size_t count);

/** Copies a block of memory forward, like `memcpy()`.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_mcopy(void* dst, const void* src, size_t size);

/** Copies a block of memory backward.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_mcopy_backward(void* dst, const void* src, size_t size);

/** Fills a block of memory with the given byte, like `memset()`.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_mfill(void* dst, int val, size_t size);

/** Fills a block of memory with zeroes, like `bzero()`.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_mzero(void* dst, size_t size);

/** Compares two blocks of memory, like `memcmp()`.  */
__MCF_XGLOBALS_INLINE __MCF_FN_PURE
int
__MCF_mcompare(const void* src, const void* cmp, size_t size);

/** Checks whether two blocks of memory compare equal.  */
__MCF_XGLOBALS_INLINE __MCF_FN_PURE
bool
__MCF_mequal(const void* src, const void* cmp, size_t size);

/** Allocates a block of zeroed memory, like `calloc()`.  */
__MCF_XGLOBALS_INLINE __attribute__((__malloc__, __alloc_size__(1)))
void*
__MCF_malloc_0(size_t size);

/** Sets the size a block of memory in place, like `truncate()` on files. If the
 * existent block should be extended, vacuum bytes are filled with zeroes.  */
__MCF_XGLOBALS_INLINE __attribute__((__alloc_size__(2)))
void*
__MCF_mresize_0(void* ptr, size_t size);

/** Re-allocates a block of memory, like `realloc()`. If the existent block should
 * be extended, vacuum bytes are filled with zeroes.  */
__MCF_XGLOBALS_INLINE __attribute__((__alloc_size__(2)))
void*
__MCF_mrealloc_0(void** pptr, size_t size);

/** Allocates a copy of a block of memory, like `malloc()` followed by `memcpy()`.  */
__MCF_XGLOBALS_INLINE __attribute__((__alloc_size__(2)))
void*
__MCF_malloc_copy(const void* data, size_t size);

/** Gets the size of an allocated block, like `malloc_usable_size()`.  */
__MCF_XGLOBALS_INLINE __MCF_FN_PURE
size_t
__MCF_msize(const void* ptr);

/** Frees a block of memory, like `free()`, except that the argument shall not be
 * a null pointer.  */
__MCF_XGLOBALS_INLINE
void
__MCF_mfree_nonnull(void* ptr);

/** Sets the last error code to `code` and returns `val`. This function should be
 * the target of a tail call.  */
__MCF_XGLOBALS_IMPORT __MCF_FN_COLD
int
__MCF_win32_error_i(ULONG code, int val);

/** Sets the last error code to `code` and returns `ptr`. This function should be
 * the target of a tail call.  */
__MCF_XGLOBALS_IMPORT __MCF_FN_COLD
void*
__MCF_win32_error_p(ULONG code, void* ptr);

/** Sets the last error code to `RtlNtStatusToDosError(status)` and returns `ptr`.
 * This function should be the target of a tail call.  */
__MCF_XGLOBALS_IMPORT __MCF_FN_COLD
void*
__MCF_win32_ntstatus_p(NTSTATUS status, void* ptr);

/** Undocumented  */
__MCF_XGLOBALS_IMPORT
void
__MCF_run_static_dtors(_MCF_mutex* mtx, __MCF_dtor_queue* queue, void* dso);

/** Undocumented  */
__MCF_XGLOBALS_IMPORT
void
__MCF_gthread_initialize_globals(void);

/** Undocumented  */
__MCF_XGLOBALS_IMPORT
void
__MCF_gthread_on_thread_exit(void);

/** These are global variables that are initialized either statically, or when
 * `DLL_PROCESS_ATTACH` is received. These are really read-only; after
 * successful initialization, the DLL changes the protection of its `.data`
 * section to `PAGE_READONLY`, so an attempt to modify these variables will
 * result in an access violation.  */
extern __MCF_ALIGNED(8) __MCF_BR(GUID) const __MCF_crt_gthread_guid;
extern __MCF_BR(__MCF_winnt_timeout) const __MCF_crt_timeout_0;
extern __MCF_BR(__MCF_winnt_timeout) const __MCF_crt_timeout_1s;

extern SYSTEM_INFO __MCF_XGLOBALS_READONLY __MCF_crt_sysinfo;
extern double __MCF_XGLOBALS_READONLY __MCF_crt_perf_freq_reciprocal;
extern HANDLE __MCF_XGLOBALS_READONLY __MCF_crt_heap;
extern HMODULE __MCF_XGLOBALS_READONLY __MCF_crt_ntdll;
extern HMODULE __MCF_XGLOBALS_READONLY __MCF_crt_kernel32;
extern HMODULE __MCF_XGLOBALS_READONLY __MCF_crt_kernelbase;

typedef void __stdcall typeof_GetSystemTimePreciseAsFileTime(FILETIME*);
extern typeof_GetSystemTimePreciseAsFileTime*
    __MCF_XGLOBALS_READONLY __MCF_crt_GetSystemTimePreciseAsFileTime;

typedef void __stdcall typeof_QueryUnbiasedInterruptTimePrecise(ULONGLONG*);
extern typeof_QueryUnbiasedInterruptTimePrecise*
    __MCF_XGLOBALS_READONLY __MCF_crt_QueryUnbiasedInterruptTimePrecise;

typedef void __stdcall typeof_QueryInterruptTimePrecise(ULONGLONG*);
extern typeof_QueryInterruptTimePrecise*
    __MCF_XGLOBALS_READONLY __MCF_crt_QueryInterruptTimePrecise;

typedef BOOL __stdcall typeof_GetSystemCpuSetInformation(
    SYSTEM_CPU_SET_INFORMATION*, ULONG, ULONG*, HANDLE, ULONG);
extern typeof_GetSystemCpuSetInformation*
    __MCF_XGLOBALS_READONLY __MCF_crt_GetSystemCpuSetInformation_opt;

typedef BOOL __stdcall typeof_GetThreadSelectedCpuSets(HANDLE, ULONG*, ULONG, ULONG*);
extern typeof_GetThreadSelectedCpuSets*
    __MCF_XGLOBALS_READONLY __MCF_crt_GetThreadSelectedCpuSets_opt;

typedef BOOL __stdcall typeof_SetThreadSelectedCpuSets(HANDLE, const ULONG*, ULONG);
extern typeof_SetThreadSelectedCpuSets*
    __MCF_XGLOBALS_READONLY __MCF_crt_SetThreadSelectedCpuSets_opt;

typedef LPVOID __stdcall typeof_TlsGetValue2(ULONG);
extern typeof_TlsGetValue2* __MCF_XGLOBALS_READONLY __MCF_crt_TlsGetValue2;

/** Declare the structure of global data in named shared memory.
 *
 * As mcfgthread may be linked statically by user DLLs, we must ensure that, in
 * the same process, all instances of `__MCF_g` (see below) point to the same
 * object. This is achieved by having them point to a shared memory object whose
 * name is made from the current PID, with additional obfuscation. For security
 * reasons, the section is created with exclusive access so no other process can
 * open it.  */
struct __MCF_crt_xglobals
  {
    __MCF_crt_xglobals* self_ptr;
    uint32_t self_size;
    uint32_t tls_index;

    __MCF_BR(__MCF_thread_base) main_thread;
    __MCF_BR(_MCF_mutex) exit_mtx;
    __MCF_BR(__MCF_dtor_queue) exit_queue;
    __MCF_BR(_MCF_mutex) quick_exit_mtx;
    __MCF_BR(__MCF_dtor_queue) quick_exit_queue;
    __MCF_ALIGNED(64) uint8_t mutex_spin_field[2048];
    __MCF_BR(_MCF_cond) interrupt_cond;

    /* Fields after this line were added after the first release of mcfgthread.
     * In a process which loads a DLL that is linked against an old static copy of
     * mcfgthread, these might be missing from the named shared memory object, and
     * must be accessed with `__MCF_G_OPT()`.  */
    void* reserved_1_must_be_null;  /* v1.8 */
    void* reserved_2_must_be_null;  /* v1.8 */
    __MCF_BR(_MCF_mutex) thread_oom_mtx;  /* v1.9 */
    __MCF_thread_base opt_thread_oom_self_st;  /* v1.9 */
    void* reserved_3_must_be_null;  /* v2.4 */
    void* reserved_4_must_be_null;  /* v2.4 */
  };

/** This is a pointer to the process-specific named shared memory in the
 * current module.  */
extern __MCF_crt_xglobals* __MCF_XGLOBALS_READONLY restrict __MCF_g;

/** These are utility macros for accessing fields in the named shared memory
 * with version checking.  */
#define __MCF_G_SIZE_TOTAL    (sizeof(__MCF_crt_xglobals))
#define __MCF_G_SIZE(field)   (RTL_SIZEOF_THROUGH_FIELD(__MCF_crt_xglobals, field))
#define __MCF_HAS_G(field)    (RTL_CONTAINS_FIELD(__MCF_g, __MCF_g->self_size, field))
#define __MCF_G(field)        (*(__MCF_ASSERT(__MCF_HAS_G(field)), &(__MCF_g->field)))
#define __MCF_G_OPT(field)    (__MCF_HAS_G(opt_##field) ? &(__MCF_g->opt_##field) : nullptr)

/** Ensure we don't mess things up.  */
__MCF_STATIC_ASSERT(__MCF_G_SIZE(self_ptr) == __MCF_64_32(8, 4));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(self_size) == __MCF_64_32(12, 8));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(tls_index) == __MCF_64_32(16, 12));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(main_thread) == __MCF_64_32(1616, 816));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(exit_mtx) == __MCF_64_32(1624, 820));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(exit_queue) == __MCF_64_32(3152, 1584));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(quick_exit_mtx) == __MCF_64_32(3160, 1588));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(quick_exit_queue) == __MCF_64_32(4688, 2352));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(mutex_spin_field) == __MCF_64_32(6784, 4416));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(interrupt_cond) == __MCF_64_32(6792, 4420));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(reserved_1_must_be_null) == __MCF_64_32(6800, 4424));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(reserved_2_must_be_null) == __MCF_64_32(6808, 4428));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(thread_oom_mtx) == __MCF_64_32(6816, 4432));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(opt_thread_oom_self_st) == __MCF_64_32(8416, 5232));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(reserved_3_must_be_null) == __MCF_64_32(8424, 5236));
__MCF_STATIC_ASSERT(__MCF_G_SIZE(reserved_4_must_be_null) == __MCF_64_32(8432, 5240));

/** Define inline functions after all declarations.
 *
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_mcopy(void* dst, const void* src, size_t size)
  {
    if(__builtin_constant_p(size) && (size == 0))
      return dst;

    __MCF_ASSERT((uintptr_t) dst - (uintptr_t) src >= size);
#if defined __MCF_M_X86_ASM
    PVOID edi, esi, ecx;
    __asm__ volatile (
      "rep movsb"
      : "=D"(edi), "=S"(esi), "=c"(ecx)
      : "0"(dst), "1"(src), "2"(size)
      : "memory"
    );
#elif defined __MCF_M_ARM64_ASM && defined __ARM_FEATURE_MOPS
    PVOID x0, x1, x2;
    __asm__ volatile (
      "cpyfp [%0]!, [%1]!, %2!; "
      "cpyfm [%0]!, [%1]!, %2!; "
      "cpyfe [%0]!, [%1]!, %2!; "
      : "=&r"(x0), "=&r"(x1), "=&r"(x2)
      : "0"(dst), "1"(src), "2"(size)
      : "memory"
    );
#else
    RtlMoveMemory(dst, src, size);
#endif
    return dst;
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_mcopy_backward(void* dst, const void* src, size_t size)
  {
    if(__builtin_constant_p(size) && (size == 0))
      return dst;

    __MCF_ASSERT((uintptr_t) src - (uintptr_t) dst >= size);
#if defined __MCF_M_X86_ASM
    PVOID edi, esi, ecx;
    __asm__ volatile (
      "std; "
      "rep movsb; "
      "cld; "
      : "=D"(edi), "=S"(esi), "=c"(ecx)
      : "0"((char*) dst + size - 1), "1"((char*) src + size - 1), "2"(size)
      : "memory"
    );
#elif defined __MCF_M_ARM64_ASM && defined __ARM_FEATURE_MOPS
    PVOID x0, x1, x2;
    __asm__ volatile (
      "cpyp [%0]!, [%1]!, %2!; "
      "cpym [%0]!, [%1]!, %2!; "
      "cpye [%0]!, [%1]!, %2!; "
      : "=&r"(x0), "=&r"(x1), "=&r"(x2)
      : "0"(dst), "1"(src), "2"(size)
      : "memory"
    );
#else
    RtlMoveMemory(dst, src, size);
#endif
    return dst;
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_mfill(void* dst, int val, size_t size)
  {
    if(__builtin_constant_p(size) && (size == 0))
      return dst;

#if defined __MCF_M_X86_ASM
    PVOID edi, ecx;
    __asm__ volatile (
      "rep stosb"
      : "=D"(edi), "=c"(ecx)
      : "0"(dst), "1"(size), "a"(val)
      : "memory"
    );
#elif defined __MCF_M_ARM64_ASM && defined __ARM_FEATURE_MOPS
    PVOID x0, x2;
    __asm__ volatile (
      "setp [%0]!, %1!, %x4; "
      "setm [%0]!, %1!, %x4; "
      "sete [%0]!, %1!, %x4; "
      : "=&r"(x0), "=&r"(x2)
      : "0"(dst), "1"(size), "r"(val)
      : "memory"
    );
#else
    RtlFillMemory(dst, size, val);
#endif
    return dst;
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_mzero(void* dst, size_t size)
  {
    if(__builtin_constant_p(size) && (size == 0))
      return dst;

#if defined __MCF_M_X86_ASM
    PVOID edi, ecx;
    __asm__ volatile (
      "rep stosb"
      : "=D"(edi), "=c"(ecx)
      : "0"(dst), "1"(size), "a"(0)
      : "memory"
    );
#elif defined __MCF_M_ARM64_ASM && defined __ARM_FEATURE_MOPS
    PVOID x0, x1;
    __asm__ volatile (
      "setp [%0]!, %1!, xzr; "
      "setm [%0]!, %1!, xzr; "
      "sete [%0]!, %1!, xzr; "
      : "=&r"(x0), "=&r"(x1)
      : "0"(dst), "1"(size)
      : "memory"
    );
#else
    RtlZeroMemory(dst, size);
#endif
    return dst;
  }

__MCF_ALWAYS_INLINE
int
__MCF_do_std_compare(const void* src, const void* src_end, const void* cmp)
  {
    typedef const volatile unsigned char* PCVBYTE;
    PCVBYTE sptr = src, cptr = cmp;
    int diff = 0;
    while((diff == 0) && (sptr != src_end))
      diff = *(sptr ++) - *(cptr ++);
    return diff;
  }

__MCF_XGLOBALS_INLINE __MCF_FN_PURE
int
__MCF_mcompare(const void* src, const void* cmp, size_t size)
  {
    if(__builtin_constant_p(size) && (size == 0))
      return 0;

    int diff;
#if defined __MCF_M_X86_ASM
    PVOID esi, edi, ecx;
    __asm__ (
      "xor %k0, %k0; "  /* set ZF and clear CF  */
      "repz cmpsb; "    /* compare DS:[ESI] with ES:[EDI]  */
      "setnz %b0; "     /* EAX = 0 if equal, 1 if less or greater  */
      "sbb ecx, ecx; "  /* ECX = 0 if equal or greater, -1 if less  */
      "or %k0, ecx; "
      : "=q"(diff), "=S"(esi), "=D"(edi), "=c"(ecx)
      : "1"(src), "2"(cmp), "3"(size)
      : "memory", "cc"
    );
#else
    diff = __MCF_do_std_compare(src, (const char*) src + size, cmp);
#endif
    return diff;
  }

__MCF_XGLOBALS_INLINE __MCF_FN_PURE
bool
__MCF_mequal(const void* src, const void* cmp, size_t size)
  {
    if(__builtin_constant_p(size) && (size == 0))
      return true;

    bool eq;
#if defined __MCF_M_X86_ASM
    PVOID esi, edi, ecx;
    __asm__ (
      ".ifeq %c7; "     /* length is NOT known non-zero? (`.ifeq` reads 'if not')  */
      "cmp ecx, ecx; "  /* ensure ZF is set in case of length of zero  */
      ".endif; "
      "repz cmpsb; "    /* compare DS:[ESI] with ES:[EDI]  */
      : "=@ccz"(eq), "=S"(esi), "=D"(edi), "=c"(ecx)
      : "1"(src), "2"(cmp), "3"(size),
        "i"(__builtin_constant_p(size != 0) ? (size != 0) : 0)
      : "memory", "cc"
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
    return HeapReAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY | HEAP_REALLOC_IN_PLACE_ONLY,
                       ptr, size);
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

__MCF_XGLOBALS_INLINE __MCF_FN_PURE
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
    __MCF_ASSERT(ptr != nullptr);
#ifdef __MCF_DEBUG
    size_t size = HeapSize(__MCF_crt_heap, 0, ptr);
    __MCF_ASSERT(size != (size_t) -1);
    __MCF_mfill(ptr, 0xFE, size);
#endif
    int succ = HeapFree(__MCF_crt_heap, 0, ptr);
    __MCF_ASSERT(succ);
  }

__MCF_ALWAYS_INLINE
ULONGLONG
__MCF_get_interrupt_time(void)
  {
    /* At address `0x7FFE0000` (`MM_SHARED_USER_DATA_VA` in Windows SDK for
     * assembly, same on all architectures) there's a read-only structure of
     * type `KUSER_SHARED_DATA`. The `InterruptTime` field is at offset `8` on
     * all architectures; see
     * <https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-kuser_shared_data>.  */
    volatile char* shared_user_data = (char*) 0x7FFE0000;
#if !defined __MCF_M_X8664_ASM
    __asm__ ("" : "+r"(shared_user_data));  /* workaround for optimizer bug  */
#endif
#if __MCF_64_32(1, 0)
    return *(volatile ULONGLONG*) (shared_user_data + 8);
#else
    ULONG high, low;
    do {
      /* A 32-bit kernel does not write 64-bit integers atomically, which
       * requires special treatment. The kernel writes `High2Time` then
       * `LowPart` then `High1Time` so we read them in the reverse order. If
       * `High1Time` does not equal `High2Time`, then the value will have
       * been split and we must try again.  */
      high = *(volatile ULONG*) (shared_user_data + 12);
      low = *(volatile ULONG*) (shared_user_data + 8);
    } while(high != *(volatile ULONG*) (shared_user_data + 16));
    return (ULONGLONG) high << 32 | low;
#endif
  }

__MCF_ALWAYS_INLINE
void
__MCF_set_directory_to_BaseNamedObject(OBJECT_ATTRIBUTES* attrs)
  {
    NTSTATUS status = BaseGetNamedObjectDirectory(&(attrs->RootDirectory));
    __MCF_ASSERT(status >= 0);
  }

__MCF_ALWAYS_INLINE
HANDLE
__MCF_create_named_section(const OBJECT_ATTRIBUTES* Attributes, LONGLONG MaximumSize)
  {
    HANDLE handle;
    LARGE_INTEGER size = { .QuadPart = MaximumSize };
    NTSTATUS status = NtCreateSection(&handle, SECTION_ALL_ACCESS,
                                      (OBJECT_ATTRIBUTES*) Attributes, &size,
                                      PAGE_READWRITE, SEC_COMMIT, NULL);
    if(status < 0)
      return __MCF_win32_ntstatus_p(status, NULL);
    return handle;
  }

__MCF_ALWAYS_INLINE
HANDLE
__MCF_duplicate_handle(HANDLE SourceHandle)
  {
    HANDLE handle;
    NTSTATUS status = NtDuplicateObject(NtCurrentProcess(), SourceHandle,
                                        NtCurrentProcess(), &handle, 0, 0,
                                        DUPLICATE_SAME_ACCESS);
    if(status < 0)
      return __MCF_win32_ntstatus_p(status, NULL);
    return handle;
  }

__MCF_ALWAYS_INLINE
void
__MCF_close_handle(HANDLE Handle)
  {
    NTSTATUS status = NtClose(Handle);
    __MCF_ASSERT(status >= 0);
  }

__MCF_ALWAYS_INLINE
void*
__MCF_map_view_of_section(HANDLE Section, size_t* Size, bool Inheritable)
  {
    void* address = nullptr;
    UINT inherit = Inheritable ? 1U : 2U;  /* ViewShare : ViewUnmap */
    NTSTATUS status = NtMapViewOfSection(Section, NtCurrentProcess(), &address, 0, 0,
                                         nullptr, (SIZE_T*) Size, inherit, 0, PAGE_READWRITE);
    if(status < 0)
      return __MCF_win32_ntstatus_p(status, nullptr);
    return address;
  }

__MCF_ALWAYS_INLINE
void
__MCF_unmap_view_of_section(void* Address)
  {
    NTSTATUS status = NtUnmapViewOfSection(NtCurrentProcess(), Address);
    __MCF_ASSERT(status >= 0);
  }

__MCF_ALWAYS_INLINE
int
__MCF_wait_for_single_object(HANDLE Handle, const __MCF_winnt_timeout* Timeout)
  {
    NTSTATUS status = NtWaitForSingleObject(Handle, false, (LARGE_INTEGER*) &(Timeout->li));
    __MCF_ASSERT(status >= 0);
    return (status == STATUS_WAIT_0) ? 0 : -1;
  }

__MCF_ALWAYS_INLINE
void
__MCF_sleep(const __MCF_winnt_timeout* Timeout)
  {
    NTSTATUS status = NtDelayExecution(false, (LARGE_INTEGER*) &(Timeout->li));
    __MCF_ASSERT(status >= 0);
  }

__MCF_ALWAYS_INLINE
int
__MCF_keyed_event_wait(const void* Key, const __MCF_winnt_timeout* Timeout)
  {
    NTSTATUS status = NtWaitForKeyedEvent(NULL, (PVOID) Key, false,
                                          (LARGE_INTEGER*) &(Timeout->li));
    __MCF_ASSERT(status >= 0);
    return (status == STATUS_WAIT_0) ? 0 : -1;
  }

__MCF_ALWAYS_INLINE
int
__MCF_keyed_event_signal(const void* Key, const __MCF_winnt_timeout* Timeout)
  {
    NTSTATUS status = NtReleaseKeyedEvent(NULL, (PVOID) Key, false,
                                          (LARGE_INTEGER*) &(Timeout->li));
    __MCF_ASSERT(status >= 0);
    return (status == STATUS_WAIT_0) ? 0 : -1;
  }
