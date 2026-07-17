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
    OUT void* Destination,
    IN const void* Source,
    IN SIZE_T Length);

/** ntdll.dll  */
NTSYSAPI
void
NTAPI
RtlFillMemory(
    OUT void* Destination,
    IN SIZE_T Length,
    IN int Fill);

/** ntdll.dll  */
NTSYSAPI
void
NTAPI
RtlZeroMemory(
    OUT void* Destination,
    IN SIZE_T Length);

/** ntdll.dll  */
NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosError(
    IN NTSTATUS Status);

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
    OUT HANDLE* OutHandle);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateSection(
    OUT HANDLE* SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN OPTIONAL OBJECT_ATTRIBUTES* ObjectAttributes,
    IN LARGE_INTEGER* MaximumSize,
    IN ULONG SectionPageProtection,
    IN ULONG AllocationAttributes,
    IN OPTIONAL HANDLE FileHandle);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDuplicateObject(
    IN HANDLE SourceProcessHandle,
    IN HANDLE SourceHandle,
    IN OPTIONAL HANDLE TargetProcessHandle,
    OUT OPTIONAL HANDLE* TargetHandle,
    IN ACCESS_MASK DesiredAccess,
    IN ULONG HandleAttributes,
    IN ULONG Options);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtClose(
    IN HANDLE Handle);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtMapViewOfSection(
    IN HANDLE SectionHandle,
    IN HANDLE ProcessHandle,
    IN OUT PVOID* BaseAddress,
    IN ULONG_PTR ZeroBits,
    IN SIZE_T CommitSize,
    IN OUT OPTIONAL LARGE_INTEGER* SectionOffset,
    IN OUT SIZE_T* ViewSize,
    IN UINT InheritDisposition,
    IN ULONG AllocationType,
    IN ULONG Win32Protect);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN OPTIONAL PVOID BaseAddress);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForSingleObject(
    IN HANDLE Handle,
    IN BOOLEAN Alertable,
    IN OPTIONAL LARGE_INTEGER* Timeout);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDelayExecution(
    IN BOOLEAN Alertable,
    IN OPTIONAL LARGE_INTEGER* Timeout);

/** ntdll.dll since Windows XP  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForKeyedEvent(
    IN OPTIONAL HANDLE KeyedEventHandle,
    IN PVOID Key,
    IN BOOLEAN Alertable,
    IN OPTIONAL LARGE_INTEGER* Timeout);

/** ntdll.dll since Windows XP  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtReleaseKeyedEvent(
    IN OPTIONAL HANDLE KeyedEventHandle,
    IN PVOID Key,
    IN BOOLEAN Alertable,
    IN OPTIONAL LARGE_INTEGER* Timeout);

/** ntdll.dll  */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRaiseHardError(
    IN NTSTATUS Status,
    IN ULONG NumberOfParameters,
    IN ULONG UnicodeStringParameterMask,
    IN OPTIONAL ULONG_PTR* Parameters,
    IN ULONG ResponseOption,
    OUT ULONG* Response);
