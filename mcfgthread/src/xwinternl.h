/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#include <minwindef.h>
#include <winnt.h>

/** These are forward declarations.  */
typedef LONG NTSTATUS;
typedef struct _UNICODE_STRING UNICODE_STRING;
typedef struct _OBJECT_ATTRIBUTES OBJECT_ATTRIBUTES;
typedef UINT SECTION_INHERIT;
typedef UINT HARDERROR_RESPONSE_OPTION;
typedef UINT HARDERROR_RESPONSE;

/** `UNICODE_STRING`; ntdef.h  */
struct _UNICODE_STRING
  {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
  };

/** `OBJECT_ATTRIBUTES`; ntdef.h  */
struct _OBJECT_ATTRIBUTES
  {
    ULONG Length;
    HANDLE RootDirectory;
    UNICODE_STRING* ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
  };

/** `SECTION_INHERIT`; wdm.h  */
enum _SECTION_INHERIT
  {
    ViewShare = 1,  /* inheritable by child processes */
    ViewUnmap = 2,  /* not inheritable by child processes */
  };

/** `HARDERROR_RESPONSE_OPTION`; undocumented  */
enum _HARDERROR_RESPONSE_OPTION
  {
    OptionAbortRetryIgnore   = 0,  /* `MB_ABORTRETRYIGNORE` */
    OptionOk                 = 1,  /* `MB_OK` */
    OptionOkCancel           = 2,  /* `MB_OKCANCEL` */
    OptionRetryCancel        = 3,  /* `MB_RETRYCANCEL` */
    OptionYesNo              = 4,  /* `MB_YESNO` */
    OptionYesNoCancel        = 5,  /* `MB_YESNOCANCEL` */
    OptionShutdownSystem     = 6,  /* DANGER - effects BSoD */
    OptionOkNoWait           = 7,  /* shows tray balloon instead of message box */
    OptionCancelTryContinue  = 8,  /* `MB_CANCELTRYCONTINUE` */
};

/** `HARDERROR_RESPONSE`; undocumented  */
enum _HARDERROR_RESPONSE
  {
    ResponseReturnToCaller  =  0,  /* LPC communication error */
    ResponseNotHandled      =  1,  /* preset value in case of invalid argument */
    ResponseAbort           =  2,  /* `IDABORT` */
    ResponseCancel          =  3,  /* `IDCANCEL` */
    ResponseIgnore          =  4,  /* `IDIGNORE` */
    ResponseNo              =  5,  /* `IDNO` */
    ResponseOk              =  6,  /* `IDOK` */
    ResponseRetry           =  7,  /* `IDRETRY` */
    ResponseYes             =  8,  /* `IDYES` */
    ResponseTryAgain        =  9,  /* `IDTRYAGAIN` */
    ResponseContinue        = 10,  /* `IDCONTINUE` */
  };

/** `OBJECT_ATTRIBUTES.Attributes`; ntdef.h */
#define OBJ_INHERIT                        0x00000002
#define OBJ_PERMANENT                      0x00000010
#define OBJ_EXCLUSIVE                      0x00000020
#define OBJ_CASE_INSENSITIVE               0x00000040
#define OBJ_OPENIF                         0x00000080
#define OBJ_OPENLINK                       0x00000100
#define OBJ_KERNEL_HANDLE                  0x00000200
#define OBJ_FORCE_ACCESS_CHECK             0x00000400
#define OBJ_IGNORE_IMPERSONATED_DEVICEMAP  0x00000800
#define OBJ_DONT_REPARSE                   0x00001000

/** pseudo handles; wdm.h  */
#define NtCurrentProcess()  ((HANDLE) -1)
#define NtCurrentThread()   ((HANDLE) -2)
#define NtCurrentSession()  ((HANDLE) -3)

/** Undefine macros that redirect to standard C functions, so the ones from
 * system DLLs will be called.  */
#undef RtlCopyMemory
#undef RtlMoveMemory
#undef RtlFillMemory
#undef RtlZeroMemory
#undef RtlEqualMemory

/** Copies a block of memory. The source and destination blocks may overlap.
 *
 * This function resembles the standard C function `memmove()`.
 *
 * While in wdm.h this is defined as a macro, a real function is exported from
 * NTDLL.DLL.
 *
 * @param `Destination` points to the destination block.
 * @param `Source` points to the source block.
 * @param `Length` is the number of bytes to copy.
 * @returns nothing.
 * @since All  */
NTSYSAPI
void
NTAPI
RtlMoveMemory(
    _Out_writes_bytes_(Length) void* Destination,
    _In_reads_bytes_(Length) const void* Source,
    _In_ SIZE_T Length);

/** Fills a block of memory.
 *
 * This function resembles the standard C function `memset()`.
 *
 * While in wdm.h this is defined as a macro, a real function is exported from
 * NTDLL.DLL.
 *
 * @param `Destination` points to the destination block.
 * @param `Length` is the number of bytes to fill.
 * @param `Fill` is the value to set. Only the lowest bytes is significant.
 * @returns nothing.
 * @since All  */
NTSYSAPI
void
NTAPI
RtlFillMemory(
    _Out_writes_bytes_(Length) void* Destination,
    _In_ SIZE_T Length,
    _In_range_(0, 255) int Fill);

/** Fills a block of memory with zeroes.
 *
 * This function resembles the BSD function `bzero()`.
 *
 * While in wdm.h this is defined as a macro, a real function is exported from
 * NTDLL.DLL.
 *
 * @param `Destination` points to the destination block.
 * @param `Length` is the number of bytes to fill.
 * @returns nothing.
 * @since All  */
NTSYSAPI
void
NTAPI
RtlZeroMemory(
    _Out_writes_bytes_(Length) void* Destination,
    _In_ SIZE_T Length);

/** Converts an `NTSTATUS` value to a Windows error code.
 *
 * This function converts an `NTSTATUS` value to a Windows error code, suitable
 * for `SetLastError()`. The original `NTSTATUS` value is written to
 * `LastStatusValue` (offset 0xBF4/0x1250).
 *
 * This function is exported from NTDLL.DLL.
 *
 * @param `Status` is the `NTSTATUS` value to convert.
 * @returns the corresponding Windows error code. If the `NTSTATUS` value has no
 *    corresponding Windows error code, `ERROR_MR_MID_NOT_FOUND` is returned.
 * @since All  */
NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosError(
    _In_ NTSTATUS Status);

/** Indicates whether the DLL loader of the current process is shutting down.
 *
 * This status is set by `ExitProcess()`. This function returns true when
 * `DLL_PROCESS_DETACH` is received by a `DllMain()` or TLS callback, where all
 * the other threads have been terminated in indeterminate states by the system,
 * and attempting to wait for other threads is likely to result in a deadlock.
 *
 * This function is exported from NTDLL.DLL.
 *
 * @returns whether the DLL loader of the current process is shutting down.
 * @since Windows XP  */
NTSYSAPI __attribute__((__pure__))
BOOLEAN
NTAPI
RtlDllShutdownInProgress(
    void);

/** Gets a handle to the directory of all named objects in the current session.
 *
 * This is the directory for named kernel objects, such as semaphores, events,
 * mutexes, file mappings, and so on. For non-packaged applications, the path
 * looks like `\Sessions\{SessionID}\BaseNamedObjects`.
 *
 * This function is exported from KERNEL32.DLL and KERNELBASE.DLL.
 *
 * @param `OutHandle` points to a variable which receives the handle. The handle
 *    is cached in KERNEL32.DLL and shall not be closed by the caller.
 * @returns an `NTSTATUS` code.
 * @since Windows 7  */
NTSYSAPI
NTSTATUS
NTAPI
BaseGetNamedObjectDirectory(
    _Out_ HANDLE* OutHandle);

/** Opens a new handle to an existing kernel object.
 *
 * This function implements `DuplicateHandle()`, but with more options.
 *
 * This function is a public driver API, exported from NTDLL.DLL.
 *
 * @param `SourceProcessHandle` specifies the handle to the owner process of the
 *    source handle, or `NtCurrentProcess()`.
 * @param `SourceHandle` specifies the handle to the existing kernel object.
 * @param `TargetProcessHandle` specifies the handle to the target process. This
 *    can be null if `DUPLICATE_CLOSE_SOURCE` is specified in `Options`.
 * @param `TargetHandle` receives the new handle. The caller shall close the
 *    handle when it is no longer needed.
 * @param `DesiredAccess` is the access mask for the new handle.
 * @param `Attributes` specifies a combination of `OBJ_*` constants.
 * @param `Options` specifies a combination of `DUPLICATE_*` constants.
 * @returns an `NTSTATUS` code.
 * @since All  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDuplicateObject(
    _In_ HANDLE SourceProcessHandle,
    _In_ HANDLE SourceHandle,
    _In_opt_ HANDLE TargetProcessHandle,
    _Out_opt_ HANDLE* TargetHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG HandleAttributes,
    _In_ ULONG Options);

/** Closes a handle to a kernel object.
 *
 * This function implements `CloseHandle()`.
 *
 * This function is a public driver API, exported from NTDLL.DLL.
 *
 * @param `Handle` specifies the handle to close.
 * @returns an `NTSTATUS` code.
 * @since All  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtClose(
    _In_ HANDLE Handle);

/** Creates a named file mapping object.
 *
 * This function implements `CreateFileMappingW()`, but with more options.
 *
 * This function is a public driver API, exported from NTDLL.DLL.
 *
 * @param `SectionHandle` points to a variable which receives the new handle.
 *    The caller shall close the handle when it is no longer needed.
 * @param `DesiredAccess` is the access mask for the new handle.
 * @param `ObjectAttributes` specifies the attributes for the object.
 * @param `MaximumSize` is the maximum size of the file mapping object, which
 *    is rounded up to a multiple of the system page size.
 * @param `SectionPageProtection` specifies the protection of pages in the
 *    section. This shall be one of the `PAGE_*` constants.
 * @param `AllocationAttributes` specifies the allocation attributes of the
 *    section. This shall be a combination of `SEC_*` constants.
 * @param `FileHandle` specifies the backing file for the section. If this
 *    handle is null, the section is backed by the system paging file.
 * @returns an `NTSTATUS` code.
 * @since All  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateSection(
    _Out_ HANDLE* SectionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ OBJECT_ATTRIBUTES* ObjectAttributes,
    _In_ LARGE_INTEGER* MaximumSize,
    _In_ ULONG SectionPageProtection,
    _In_ ULONG AllocationAttributes,
    _In_opt_ HANDLE FileHandle);

/** Maps a named file mapping object into virtual address space of a process.
 *
 * This function implements `MapViewOfFile()`.
 *
 * This function is a public driver API, exported from NTDLL.DLL.
 *
 * @param `SectionHandle` specifies a handle to the file mapping object.
 * @param `ProcessHandle` specifies a handle to the process in which the file
 *    mapping will be mapped, or `NtCurrentProcess()`.
 * @param `BaseAddress` points to a variable which specifies the base address of
 *    the view, and receives the base address after the function returns. If the
 *    variable contains a null pointer before the call, the base address will be
 *    chosen by the system.
 * @param `ZeroBits` specifies the number of high-order address bits that must be
 *    zero in the base address.
 * @param `CommitSize` specifies the size of the region that will be committed
 *    initially.
 * @param `SectionOffset` specifies the offset from the beginning of the file
 *    mapping to map. If this is null, the mapping starts at the beginning.
 * @param `ViewSize` points to a variable which specifies the size of the view,
 *    and receives the size after the function returns. If this variable
 *    contains zero before the call, the file mapping is mapped until the end.
 * @param `InheritDisposition` specifies one of the `View*` constants.
 * @param `AllocationType` specifies a combination of `MEM_*` constants.
 * @param `Win32Protect` specifies the protection of the view. This shall be one
 *    of the `PAGE_*` constants.
 * @returns an `NTSTATUS` code.
 * @since All  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtMapViewOfSection(
    _In_ HANDLE SectionHandle,
    _In_ HANDLE ProcessHandle,
    _Inout_ PVOID* BaseAddress,
    _In_ ULONG_PTR ZeroBits,
    _In_ SIZE_T CommitSize,
    _Inout_opt_ LARGE_INTEGER* SectionOffset,
    _Inout_ SIZE_T* ViewSize,
    _In_ SECTION_INHERIT InheritDisposition,
    _In_ ULONG AllocationType,
    _In_ ULONG Win32Protect);

/** Unmaps a mapped view from virtual address space of a process.
 *
 * This function implements `UnmapViewOfFile()`.
 *
 * This function is a public driver API, exported from NTDLL.DLL.
 *
 * @param `ProcessHandle` specifies a handle to the process in which the file
 *    mapping will be unmapped, or `NtCurrentProcess()`.
 * @param `BaseAddress` points to the base address that was returned by a
 *    previous call to `NtMapViewOfSection()`.
 * @returns an `NTSTATUS` code.
 * @since All  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnmapViewOfSection(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress);

/** Waits for an object to become signaled.
 *
 * This function implements `WaitForSingleObjectEx()`.
 *
 * This function is a public driver API, exported from NTDLL.DLL.
 *
 * @param `Handle` specifies a handle to the object to wait for.
 * @param `Alertable` specifies whether the operation is alertable.
 * @param `Timeout` specifies an optional timeout. If the parameter is null, the
 *    function waits indefinitely.
 * @returns an `NTSTATUS` code.
 * @since All  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForSingleObject(
    _In_ HANDLE Handle,
    _In_ BOOLEAN Alertable,
    _In_opt_ LARGE_INTEGER* Timeout);

/** Suspends execution of the current thread.
 *
 * This function implements `SleepEx()`.
 *
 * This function is exported from NTDLL.DLL.
 *
 * @param `Alertable` specifies whether the operation is alertable.
 * @param `Timeout` specifies an optional timeout. If the parameter is null, the
 *    function waits indefinitely.
 * @returns an `NTSTATUS` code.
 * @since All  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDelayExecution(
    _In_ BOOLEAN Alertable,
    _In_opt_ LARGE_INTEGER* Timeout);

/** Awaits a keyed event.
 *
 * This function suspends the current thread until another thread signals the
 * same keyed event with the same key, then both threads are released.
 *
 * This function is exported from NTDLL.DLL.
 *
 * @param `KeyedEventHandle` specifies a handle to the keyed event. Since Windows
 *    Vista, this handle can be null, in which case the function will use
 *    `\KernelObjects\CritSecOutOfMemoryEvent`.
 * @param `Key` specifies the key. The lowest bit must be clear. The key is not
 *    shared between processes.
 * @param `Alertable` specifies whether the operation is alertable.
 * @param `Timeout` specifies an optional timeout. If the parameter is null, the
 *    function waits indefinitely.
 * @returns an `NTSTATUS` code.
 * @since Windows XP  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForKeyedEvent(
    _In_opt_ HANDLE KeyedEventHandle,
    _In_ PVOID Key,
    _In_ BOOLEAN Alertable,
    _In_opt_ LARGE_INTEGER* Timeout);

/** Signals a keyed event.
 *
 * This function suspends the current thread until another thread awaits the same
 * keyed event with the same key, then both threads are released.
 *
 * This function is exported from NTDLL.DLL.
 *
 * @param `KeyedEventHandle` specifies a handle to the keyed event. Since Windows
 *    Vista, this handle can be null, in which case the function will use
 *    `\KernelObjects\CritSecOutOfMemoryEvent`.
 * @param `Key` specifies the key. The lowest bit must be clear. The key is not
 *    shared between processes.
 * @param `Alertable` specifies whether the operation is alertable.
 * @param `Timeout` specifies an optional timeout. If the parameter is null, the
 *    function waits indefinitely.
 * @returns an `NTSTATUS` code.
 * @since Windows XP  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseKeyedEvent(
    _In_opt_ HANDLE KeyedEventHandle,
    _In_ PVOID Key,
    _In_ BOOLEAN Alertable,
    _In_opt_ LARGE_INTEGER* Timeout);

/** Raises a Hard Error.
 *
 * When `Status` is `STATUS_SERVICE_NOTIFICATION`, this function sends an LPC
 * message to CSRSS.EXE which displays a modal message box to the user:
 *
 * - Parameter 0 is a pointer to a `UNICODE_STRING` which specifies the text for
 *   the message box.
 * - Parameter 1 is a pointer to a `UNICODE_STRING` which specifies the caption
 *   for the message box.
 * - Parameter 2 is a combination of `MB_*` constants.
 * - Parameter 3 is the timeout of the message box in milliseconds.
 *
 * @param `Status` specifies the status code to raise.
 * @param `NumberOfParameters` specifies the number of parameters that
 *    `Parameters` points to.
 * @param `UnicodeStringParameterMask` specifies a bit mask where each bit
 *    indicates whether the corresponding parameter is actually a pointer to a
 *    `UNICODE_STRING`.
 * @param `Parameters` points to an array of parameters.
 * @param `ResponseOption` specifies an enumerator of `HARDERROR_RESPONSE_OPTION`.
 * @param `Response` points to a variable which receives the user response as
 *    an enumerator of `HARDERROR_RESPONSE`.
 * @returns an `NTSTATUS` code.
 * @since All  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRaiseHardError(
    _In_ NTSTATUS Status,
    _In_ ULONG NumberOfParameters,
    _In_ ULONG UnicodeStringParameterMask,
    _In_opt_ ULONG_PTR* Parameters,
    _In_ HARDERROR_RESPONSE_OPTION ResponseOption,
    _Out_ HARDERROR_RESPONSE* Response);
