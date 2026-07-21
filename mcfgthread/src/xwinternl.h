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

/** ntdll.dll  */
NTSYSAPI
void
NTAPI
RtlMoveMemory(
    _Out_writes_bytes_(Length) void* Destination,
    _In_reads_bytes_(Length) const void* Source,
    _In_ SIZE_T Length);

/** ntdll.dll  */
NTSYSAPI
void
NTAPI
RtlFillMemory(
    _Out_writes_bytes_(Length) void* Destination,
    _In_ SIZE_T Length,
    _In_range_(0, 255) int Fill);

/** ntdll.dll  */
NTSYSAPI
void
NTAPI
RtlZeroMemory(
    _Out_writes_bytes_(Length) void* Destination,
    _In_ SIZE_T Length);

/** ntdll.dll  */
NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosError(
    _In_ NTSTATUS Status);

/** ntdll.dll since Windows XP  */
NTSYSAPI __attribute__((__pure__))
BOOLEAN
NTAPI
RtlDllShutdownInProgress(
    void);

/** kernel32.dll since Windows 7  */
NTSYSAPI
NTSTATUS
NTAPI
BaseGetNamedObjectDirectory(
    _Out_ HANDLE* OutHandle);

/** ntdll.dll  */
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

/** ntdll.dll  */
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

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtClose(
    _In_ HANDLE Handle);

/** ntdll.dll  */
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
    _In_ UINT InheritDisposition,
    _In_ ULONG AllocationType,
    _In_ ULONG Win32Protect);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnmapViewOfSection(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForSingleObject(
    _In_ HANDLE Handle,
    _In_ BOOLEAN Alertable,
    _In_opt_ LARGE_INTEGER* Timeout);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDelayExecution(
    _In_ BOOLEAN Alertable,
    _In_opt_ LARGE_INTEGER* Timeout);

/** ntdll.dll since Windows XP  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForKeyedEvent(
    _In_opt_ HANDLE KeyedEventHandle,
    _In_ PVOID Key,
    _In_ BOOLEAN Alertable,
    _In_opt_ LARGE_INTEGER* Timeout);

/** ntdll.dll since Windows XP  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseKeyedEvent(
    _In_opt_ HANDLE KeyedEventHandle,
    _In_ PVOID Key,
    _In_ BOOLEAN Alertable,
    _In_opt_ LARGE_INTEGER* Timeout);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRaiseHardError(
    _In_ NTSTATUS Status,
    _In_ ULONG NumberOfParameters,
    _In_ ULONG UnicodeStringParameterMask,
    _In_opt_ ULONG_PTR* Parameters,
    _In_ ULONG ResponseOption,
    _Out_ ULONG* Response);
