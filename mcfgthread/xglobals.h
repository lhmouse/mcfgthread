/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
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
#include <profileapi.h>
#include <realtimeapiset.h>
#include <heapapi.h>
#include <libloaderapi.h>
#include <processthreadsapi.h>
#include <shtypes.h>
#include <wincon.h>

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_XGLOBALS_IMPORT
#  define __MCF_XGLOBALS_IMPORT
#  define __MCF_XGLOBALS_INLINE  __MCF_GNU_INLINE
#  define __MCF_XGLOBALS_READONLY   const
#endif

#ifndef _WIN32_WINNT
#  error Only Windows platforms are supported.
#endif

#if _WIN32_WINNT < 0x0601
#  error Please define `_WIN32_WINNT` to at least Windows 7.
#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#  error Windows platforms are assumed to be little-endian.
#endif

#ifdef __arm__
#  error 32-bit ARM target is not supported.
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
#undef RtlCompareMemory
#undef RtlEqualMemory

void __stdcall RtlMoveMemory(void* dst, const void* src, SIZE_T size);
void __stdcall RtlFillMemory(void* dst, SIZE_T size, int c);
void __stdcall RtlZeroMemory(void* dst, SIZE_T size);
SIZE_T __stdcall RtlCompareMemory(const void* src, const void* cmp, SIZE_T size) __attribute__((__pure__));

/* Define a non-zero but invalid value. This can be used to mark a pointer
 * to freed memory, or to prevent a static pointer from being placed into
 * the `.bss` section.  */
#define __MCF_BAD_PTR  ((void*) -127)

/* Define macros and types for lazy binding.
 * If a symbol cannot be found during startup, it is set to a null pointer.  */
#define __MCF_G_LAZY_FIELD(name)   decltype_##name* __f_##name

typedef void __stdcall decltype_GetSystemTimePreciseAsFileTime(FILETIME*);
typedef void __stdcall decltype_QueryInterruptTime(ULONGLONG*);

/* Declare helper functions here.  */
__MCF_XGLOBALS_IMPORT
EXCEPTION_DISPOSITION
__cdecl
__MCF_seh_top(EXCEPTION_RECORD* rec, PVOID estab_frame, CONTEXT* ctx, PVOID disp_ctx) __MCF_NOEXCEPT;

#if defined __i386__
/* On x86, SEH is stack-based.  */
typedef struct __MCF_seh_i386_node __MCF_seh_i386_node;

struct __MCF_seh_i386_node
  {
    DWORD __next;
    DWORD __filter;
  };

__MCF_ALWAYS_INLINE
__MCF_seh_i386_node*
__MCF_seh_i386_install(__MCF_seh_i386_node* __seh_node) __MCF_NOEXCEPT
  {
    __asm__ volatile ("mov %0, fs:[0]" : "=r"(__seh_node->__next));
    __seh_node->__filter = (DWORD) __MCF_seh_top;
    __asm__ volatile ("mov fs:[0], %0" : : "r"(__seh_node));
    return __seh_node;
  }

__MCF_ALWAYS_INLINE
void
__MCF_seh_i386_cleanup(__MCF_seh_i386_node* __seh_node) __MCF_NOEXCEPT
  {
    __asm__ volatile ("mov fs:[0], %0" : : "r"(__seh_node->__next));
  }

#  define __MCF_SEH_I386_NODE_NY(n)  __MCF_seh_i386_node_##n
#  define __MCF_SEH_I386_NODE_NX(n)  __MCF_SEH_I386_NODE_NY(n)
#  define __MCF_SEH_I386_NODE_NAME_HERE   __MCF_SEH_I386_NODE_NX(__LINE__)

#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
    __MCF_seh_i386_node __MCF_SEH_I386_NODE_NAME_HERE  \
        __attribute__((__cleanup__(__MCF_seh_i386_cleanup)))  \
        = *__MCF_seh_i386_install(&__MCF_SEH_I386_NODE_NAME_HERE)  /* no semicolon  */

/* In the case of i386, the argument is passed both via the ECX register and
 * on the stack, to allow both `__cdecl` and `__thiscall` functions to work
 * properly.  */
__MCF_ALWAYS_INLINE
void
__MCF_invoke_cxa_dtor(__MCF_cxa_dtor_union dtor, void* arg)
  {
    /* Parameters are `EAX`, `EDX`, `ECX`, `ESP[4]`.  */
    typedef void dual_dtor_t(int, int, void*, void*) __attribute__((__regparm__(3)));
    int eax, edx;
    __asm__ ("" : "=a"(eax), "=d"(edx));  /* dummy */
    (*(dual_dtor_t*)(int) dtor.__cdecl_ptr) (eax, edx, arg, arg);
  }
#else
/* Otherwise, SEH is table-based.  */
#  define __MCF_SEH_DEFINE_TERMINATE_FILTER  \
    __asm__ (".seh_handler __MCF_seh_top, @except")  /* no semicolon  */

/* This works on x86_64 and ARM64.  */
__MCF_ALWAYS_INLINE
void
__MCF_invoke_cxa_dtor(__MCF_cxa_dtor_cdecl* dtor, void* arg)
  {
    (*dtor) (arg);
  }
#endif

/* This structure contains timeout values that will be passed to NT syscalls.  */
typedef struct __MCF_winnt_timeout __MCF_winnt_timeout;

struct __MCF_winnt_timeout
  {
    LARGE_INTEGER __li;
    ULONGLONG __since;
  };

__MCF_XGLOBALS_IMPORT
void
__MCF_initialize_winnt_timeout_v3(__MCF_winnt_timeout* to, const int64_t* ms_opt) __MCF_NOEXCEPT;

__MCF_XGLOBALS_IMPORT
void
__MCF_adjust_winnt_timeout_v3(__MCF_winnt_timeout* to) __MCF_NOEXCEPT;

/* Note this function is subject to tail-call optimization.  */
__MCF_XGLOBALS_IMPORT
size_t
__MCF_batch_release_common(const void* key, size_t count) __MCF_NOEXCEPT;

/* Copy a block of memory forward, like `memcpy()`.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy(void* dst, const void* src, size_t size) __MCF_NOEXCEPT;

/* Copy a block of memory backward.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy_backward(void* dst, const void* src, size_t size) __MCF_NOEXCEPT;

/* Fill a block of memory with the given byte, like `memset()`.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mfill(void* dst, int val, size_t size) __MCF_NOEXCEPT;

/* Fill a block of memory with zeroes, like `bzero()`.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mzero(void* dst, size_t size) __MCF_NOEXCEPT;

/* Allocate a block of zeroed memory, like `calloc()`.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_0(size_t size) __MCF_NOEXCEPT __attribute__((__warn_unused_result__, __malloc__, __alloc_size__(1)));

/* Re-allocate a block of memory, like `realloc()`. If the existent
 * block should be extended, vacuum bytes are filled with zeroes.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_mrealloc_0(void** pptr, size_t size) __MCF_NOEXCEPT __attribute__((__warn_unused_result__, __alloc_size__(2)));

/* Allocate a copy of a block of memory, like `malloc()` followed by
 * `memcpy()`.  */
__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_copy(const void* data, size_t size) __MCF_NOEXCEPT __attribute__((__warn_unused_result__, __alloc_size__(2)));

/* Get the size of an allocated block, like `malloc_usable_size()`.  */
__MCF_XGLOBALS_INLINE
size_t
__MCF_msize(const void* ptr) __MCF_NOEXCEPT __attribute__((__pure__));

/* Free a block of memory, like `free()`.  */
__MCF_XGLOBALS_INLINE
void
__MCF_mfree(void* ptr_opt) __MCF_NOEXCEPT;

/* These functions set the last error code and return the second argument.
 * They should be subject to tail-call optimization.  */
__MCF_XGLOBALS_IMPORT
int
__MCF_win32_error_i(DWORD code, int val) __MCF_NOEXCEPT;

__MCF_XGLOBALS_IMPORT
void*
__MCF_win32_error_p(DWORD code, void* ptr) __MCF_NOEXCEPT;

__MCF_XGLOBALS_IMPORT
void*
__MCF_win32_ntstatus_p(NTSTATUS status, void* ptr) __MCF_NOEXCEPT;

/* These functions are declared here for the sake of completeness, and are not
 * meant to be called directly.  */
__MCF_XGLOBALS_IMPORT
void
__MCF_run_dtors_at_quick_exit(void* dso) __MCF_NOEXCEPT;

__MCF_XGLOBALS_IMPORT
void
__MCF_run_dtors_atexit(void* dso) __MCF_NOEXCEPT;

/* Declare global data.  */
typedef struct __MCF_crt_xglobals __MCF_crt_xglobals;

struct __MCF_crt_xglobals
  {
    __MCF_crt_xglobals* __self_ptr;
    uint32_t __self_size;
    uint32_t __tls_index;

    /* the static thread object  */
    _MCF_thread __main_thread[1];

    /* `atexit()` support  */
    _MCF_mutex __cxa_atexit_mtx[1];
    __MCF_dtor_queue __cxa_atexit_queue[1];

    /* `at_quick_exit()` support  */
    _MCF_mutex __cxa_at_quick_exit_mtx[1];
    __MCF_dtor_queue __cxa_at_quick_exit_queue[1];

    /* mutex support  */
    __MCF_ALIGNED(64) BYTE __mutex_spin_field[2048];

    /* thread suspension support  */
    uintptr_t __sleeping_threads[1];

    /* WARNING: Fields hereinafter must be accessed via `__MCF_G_FIELD_OPT`!  */
    __MCF_G_LAZY_FIELD(GetSystemTimePreciseAsFileTime);
    __MCF_G_LAZY_FIELD(QueryInterruptTime);
    _MCF_mutex __thread_oom_mtx[1];
    _MCF_thread __thread_oom_self_st;
  };

/* These are constants that have to be initialized at load time.  */
extern SYSTEM_INFO __MCF_XGLOBALS_READONLY __MCF_crt_sysinfo;
extern HANDLE __MCF_XGLOBALS_READONLY __MCF_crt_heap;
extern double __MCF_XGLOBALS_READONLY __MCF_crt_pf_recip;

/* This is a pointer to the process-specific data.
 * As mcfgthread may be linked statically by user DLLs, we must ensure that, in
 * the same process, all instances of this pointer point to the same object. This
 * is achieved by having them point to a named file mapping, which is created
 * by the current process with exclusive access, and whose name is generated from
 * its process ID. Additional randomness is introduced to prevent the name from
 * being predicted.  */
extern __MCF_crt_xglobals* __MCF_XGLOBALS_READONLY __MCF_g;

/* As `__MCF_crt_xglobals` is shared between all static and shared instances of
 * this library within a single process, we have to involve sort of versioning.  */
#define __MCF_G_SIZE  \
    ((unsigned long) __builtin_expect((long) __MCF_g->__self_size, sizeof(__MCF_crt_xglobals)))

#define __MCF_G_FIELD_OPT(field)  \
    ((__MCF_G_SIZE >= offsetof(__MCF_crt_xglobals, field) + sizeof(__MCF_g->field))  \
     ? &(__MCF_g->field)  \
     : __MCF_nullptr)

#define __MCF_G_INITIALIZE_LAZY(dll, name)  \
    (__MCF_g->__f_##name =  \
        (dll) ? (decltype_##name*)(INT_PTR) GetProcAddress(dll, #name)  \
              : __MCF_nullptr)

#define __MCF_LAZY_P(name)    (__MCF_G_FIELD_OPT(__f_##name) && __MCF_g->__f_##name)
#define __MCF_LAZY_REF(name)   (*(__MCF_g->__f_##name))

/* Define inline functions after all declarations.
 * We would like to keep them away from declarations for conciseness, which also
 * matches the disposition of non-inline functions. Note that however, unlike C++
 * inline functions, they have to have consistent inline specifiers throughout
 * this file.  */
__MCF_XGLOBALS_INLINE
void*
__cdecl
__MCF_mcopy(void* dst, const void* src, size_t size) __MCF_NOEXCEPT
  {
    __MCF_ASSERT((uintptr_t) dst - (uintptr_t) src >= size);
#if defined __i386__ || defined __amd64__
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
__MCF_mcopy_backward(void* dst, const void* src, size_t size) __MCF_NOEXCEPT
  {
    __MCF_ASSERT((uintptr_t) src - (uintptr_t) dst >= size);
#if defined __i386__ || defined __amd64__
    PVOID edi, ecx, esi;
    __asm__ volatile (
      "std; "
      "lea %0, [%0 + %1 - 1]; "
      "lea %2, [%2 + %1 - 1]; "
      "rep movsb; "
      "cld; "
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
__MCF_mfill(void* dst, int val, size_t size) __MCF_NOEXCEPT
  {
#if defined __i386__ || defined __amd64__
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
__MCF_mzero(void* dst, size_t size) __MCF_NOEXCEPT
  {
#if defined __i386__ || defined __amd64__
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

__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_0(size_t size) __MCF_NOEXCEPT
  {
    void* ptr = HeapAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, size);
    return ptr;
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_mrealloc_0(void** pptr, size_t size) __MCF_NOEXCEPT
  {
    void* ptr = HeapReAlloc(__MCF_crt_heap, HEAP_ZERO_MEMORY, *pptr, size);
    if(ptr)
      *pptr = ptr;
    return ptr;
  }

__MCF_XGLOBALS_INLINE
void*
__MCF_malloc_copy(const void* data, size_t size) __MCF_NOEXCEPT
  {
    void* ptr = HeapAlloc(__MCF_crt_heap, 0, size);
    if(ptr)
      __MCF_mcopy(ptr, data, size);
    return ptr;
  }

__MCF_XGLOBALS_INLINE
size_t
__MCF_msize(const void* ptr) __MCF_NOEXCEPT
  {
    size_t size = HeapSize(__MCF_crt_heap, 0, ptr);
    __MCF_ASSERT(size != (size_t) -1);
    return size;
  }

__MCF_XGLOBALS_INLINE
void
__MCF_mfree(void* ptr_opt) __MCF_NOEXCEPT
  {
    if(!ptr_opt)
      return;

#ifdef __MCF_DEBUG
    __MCF_mfill(ptr_opt, 0xFE, HeapSize(__MCF_crt_heap, 0, ptr_opt));
#endif
    int succ = HeapFree(__MCF_crt_heap, 0, ptr_opt);
    __MCF_ASSERT(succ);
  }

/* Gets a handle to the directory for all named objects (mutexes, semaphores,
 * events, etc.) for the current session. The handle is cached in KERNEL32.DLL
 * and must not be closed.  */
NTSTATUS NTAPI BaseGetNamedObjectDirectory(HANDLE* OutHandle) __attribute__((__dllimport__));

__MCF_ALWAYS_INLINE
HANDLE
__MCF_get_directory_for_named_objects(void) __MCF_NOEXCEPT
  {
    HANDLE handle;
    NTSTATUS status = BaseGetNamedObjectDirectory(&handle);
    __MCF_ASSERT(NT_SUCCESS(status));
    return handle;
  }

/* Indicates whether the current process is being shut down. This function has
 * existed since at least Windows 7, but is only documented since Windows 10.  */
BOOLEAN NTAPI RtlDllShutdownInProgress(void) __attribute__((__dllimport__));

__MCF_ALWAYS_INLINE
bool
__MCF_is_process_shutting_down(void) __MCF_NOEXCEPT
  {
    return RtlDllShutdownInProgress();
  }

/* Create a named section of memory.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatesection  */
NTSTATUS NTAPI NtCreateSection(HANDLE* OutHandle, ACCESS_MASK DesiredAccess, OBJECT_ATTRIBUTES* Attributes, LARGE_INTEGER* MaximumSize, ULONG Protection, ULONG Allocation, HANDLE File);

__MCF_ALWAYS_INLINE
HANDLE
__MCF_create_named_section(OBJECT_ATTRIBUTES* Attributes, LONGLONG MaximumSize) __MCF_NOEXCEPT
  {
    HANDLE handle;
    LARGE_INTEGER size = { .QuadPart = MaximumSize };
    NTSTATUS status = NtCreateSection(&handle, SECTION_ALL_ACCESS, Attributes, &size, PAGE_READWRITE, SEC_COMMIT, NULL);
    return NT_SUCCESS(status) ? handle : __MCF_win32_ntstatus_p(status, NULL);
  }

/* Creates another handle to an object.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwduplicateobject  */
NTSTATUS NTAPI NtDuplicateObject(HANDLE SourceProcess, HANDLE SourceHandle, HANDLE TargetProcess, HANDLE* TargetHandle, ACCESS_MASK DesiredAccess, ULONG HandleAttributes, ULONG Options);

__MCF_ALWAYS_INLINE
HANDLE
__MCF_duplicate_handle(HANDLE SourceHandle) __MCF_NOEXCEPT
  {
    HANDLE handle;
    HANDLE process = GetCurrentProcess();
    NTSTATUS status = NtDuplicateObject(process, SourceHandle, process, &handle, 0, 0, DUPLICATE_SAME_ACCESS);
    return NT_SUCCESS(status) ? handle : __MCF_win32_ntstatus_p(status, NULL);
  }

/* Closes a handle to an object.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose  */
NTSTATUS NTAPI NtClose(HANDLE Handle);

__MCF_ALWAYS_INLINE
void
__MCF_close_handle(HANDLE Handle) __MCF_NOEXCEPT
  {
    NTSTATUS status = NtClose(Handle);
    __MCF_ASSERT(NT_SUCCESS(status));
  }

/* Maps a named section of memory into the virtual address space.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwmapviewofsection  */
NTSTATUS NTAPI NtMapViewOfSection(HANDLE Section, HANDLE Process, PVOID* BaseAddress, ULONG_PTR ZeroBits, SIZE_T CommitSize, LARGE_INTEGER* Offset, SIZE_T* ViewSize, UINT SectionInherit, ULONG Allocation, ULONG Protection);

__MCF_ALWAYS_INLINE
void
__MCF_map_view_of_section(HANDLE Section, void** BaseAddress, size_t* ViewSize, bool Inheritable) __MCF_NOEXCEPT
  {
    HANDLE process = GetCurrentProcess();
    UINT inherit = Inheritable ? 1U : 2U;  /* ViewShare : ViewUnmap */
    NTSTATUS status = NtMapViewOfSection(Section, process, BaseAddress, 0, 0, __MCF_nullptr, (SIZE_T*) ViewSize, inherit, 0, PAGE_READWRITE);
    __MCF_ASSERT(NT_SUCCESS(status));
  }

/* Unmaps a named section of memory from the virtual address space.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwunmapviewofsection  */
NTSTATUS NTAPI NtUnmapViewOfSection(HANDLE Process, PVOID BaseAddress);

__MCF_ALWAYS_INLINE
void
__MCF_unmap_view_of_section(void* BaseAddress) __MCF_NOEXCEPT
  {
    HANDLE process = GetCurrentProcess();
    NTSTATUS status = NtUnmapViewOfSection(process, BaseAddress);
    __MCF_ASSERT(NT_SUCCESS(status));
  }

/* Waits until an object is signaled.
 * https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwwaitforsingleobject  */
NTSTATUS NTAPI NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, LARGE_INTEGER* Timeout);

__MCF_ALWAYS_INLINE
int
__MCF_wait_for_single_object(HANDLE Handle, const __MCF_winnt_timeout* Timeout) __MCF_NOEXCEPT
  {
    NTSTATUS status = NtWaitForSingleObject(Handle, false, (LARGE_INTEGER*) &(Timeout->__li));
    __MCF_ASSERT(NT_SUCCESS(status));
    return (status != STATUS_WAIT_0) ? -1 : 0;
  }

/* Suspends the calling thread for the given duration. This function is called by
 * `SleepEx()`.  */
NTSTATUS NTAPI NtDelayExecution(BOOLEAN Alertable, LARGE_INTEGER* Timeout);

__MCF_ALWAYS_INLINE
void
__MCF_sleep(const __MCF_winnt_timeout* Timeout) __MCF_NOEXCEPT
  {
    NTSTATUS status = NtDelayExecution(false, (LARGE_INTEGER*) &(Timeout->__li));
    __MCF_ASSERT(NT_SUCCESS(status));
  }

/* Suspends the calling thread on the given keyed event object, until another
 * thread calls `NtReleaseKeyedEvent()` on the same object with the same key.
 * The lowest bit of the key must be zero.  */
NTSTATUS NTAPI NtWaitForKeyedEvent(HANDLE KeyedEvent, PVOID Key, BOOLEAN Alertable, LARGE_INTEGER* Timeout);

__MCF_ALWAYS_INLINE
int
__MCF_keyed_event_wait(const void* __key, const __MCF_winnt_timeout* Timeout) __MCF_NOEXCEPT
  {
    NTSTATUS status = NtWaitForKeyedEvent(NULL, (PVOID) __key, false, (LARGE_INTEGER*) &(Timeout->__li));
    __MCF_ASSERT(NT_SUCCESS(status));
    return (status != STATUS_WAIT_0) ? -1 : 0;
  }

/* Wakes another thread which shall be waiting on the given keyed event object
 * for the given key. If no such thread exists, the calling thread is suspended
 * unless a matching thread calls `NtWaitForKeyedEvent()`. The lowest bit of
 * the key must be zero.  */
NTSTATUS NTAPI NtReleaseKeyedEvent(HANDLE KeyedEvent, PVOID Key, BOOLEAN Alertable, LARGE_INTEGER* Timeout);

__MCF_ALWAYS_INLINE
int
__MCF_keyed_event_signal(const void* __key, const __MCF_winnt_timeout* Timeout) __MCF_NOEXCEPT
  {
    NTSTATUS status = NtReleaseKeyedEvent(NULL, (PVOID) __key, false, (LARGE_INTEGER*) &(Timeout->__li));
    __MCF_ASSERT(NT_SUCCESS(status));
    return (status != STATUS_WAIT_0) ? -1 : 0;
  }

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_XGLOBALS_  */
