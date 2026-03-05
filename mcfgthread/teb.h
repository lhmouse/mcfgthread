/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_TEB_
#define __MCFGTHREAD_TEB_

#include "fwd.h"
#if defined _MSC_VER && !defined __clang__
#include <intrin.h>
#endif

__MCF_CXX(extern "C" {)
#ifndef __MCF_TEB_IMPORT
#  define __MCF_TEB_IMPORT
#  define __MCF_TEB_INLINE  __MCF_ALWAYS_INLINE
#endif

/* Get a pointer to the thread environment block of the current thread in
 * the flat address space.  */
__MCF_TEB_INLINE __MCF_FN_CONST
void*
__MCF_teb(void)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __FSGSBASE__
    return (void*) __builtin_ia32_rdgsbase64();
#  elif defined __clang__
    return *(void* __seg_gs*) 0x30;
#  else
    void* __teb;
    __asm__ ("mov { %%gs:0x30, %0 | %0, gs:[0x30] }" : "=r"(__teb));
    return __teb;
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    return *(void* __seg_fs*) 0x18;
#  else
    void* __teb;
    __asm__ ("mov { %%fs:0x18, %0 | %0, fs:[0x18] }" : "=r"(__teb));
    return __teb;
#  endif
#elif defined __MCF_M_ARM64_ASM
    register void* __teb __asm__("x18");
    __asm__ ("" : "=r"(__teb));
    return __teb;
#elif defined __MCF_M_X8664
    return (void*) __readgsqword(0x30);
#elif defined __MCF_M_X8632
    return (void*) __readfsdword(0x18);
#elif defined __MCF_M_ARM64
    return (void*) __readx18qword(0x30);
#else
#  error unimplemented
#endif
  }

/* Load a 32-bit integer at `__offset` of the environment block of the
 * current thread. This function may operate on TEB directly and may be more
 * efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE __MCF_FN_PURE
int32_t
__MCF_teb_load_32(uint32_t __offset)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM && defined __clang__
    return *(int32_t __seg_gs*)(uint64_t) __offset;
#elif defined __MCF_M_X8632_ASM && defined __clang__
    return *(int32_t __seg_fs*) __offset;
#elif defined __MCF_M_X8664 && defined _MSC_VER && !defined __clang__
    return (int32_t) __readgsdword(__offset);
#elif defined __MCF_M_X8632 && defined _MSC_VER && !defined __clang__
    return (int32_t) __readfsdword(__offset);
#elif defined __MCF_M_ARM64 && defined _MSC_VER && !defined __clang__
    return (int32_t) __readx18dword(__offset);
#else
    return *((int32_t*) __MCF_teb() + __offset / 4);
#endif
  }

/* Stores a 32-bit integer at `__offset` of the environment block of the
 * current thread. This function may operate on TEB directly and may be more
 * efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE
void
__MCF_teb_store_32(uint32_t __offset, int32_t __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM && defined __clang__
    *(int32_t __seg_gs*)(uint64_t) __offset = __value;
#elif defined __MCF_M_X8632_ASM && defined __clang__
    *(int32_t __seg_fs*) __offset = __value;
#elif defined __MCF_M_X8664 && defined _MSC_VER && !defined __clang__
    __writegsdword(__offset, (uint32_t) __value);
#elif defined __MCF_M_X8632 && defined _MSC_VER && !defined __clang__
    __writefsdword(__offset, (uint32_t) __value);
#elif defined __MCF_M_ARM64 && defined _MSC_VER && !defined __clang__
    __writex18dword(__offset, (uint32_t) __value);
#else
    *((int32_t*) __MCF_teb() + __offset / 4) = __value;
#endif
  }

/* Load a generic pointer at `__offset` of the environment block of the
 * current thread. This function may operate on TEB directly and may be more
 * efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE __MCF_FN_PURE
void*
__MCF_teb_load_ptr(uint32_t __offset)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM && defined __clang__
    return *(void* __seg_gs*)(uint64_t) __offset;
#elif defined __MCF_M_X8632_ASM && defined __clang__
    return *(void* __seg_fs*) __offset;
#elif defined __MCF_M_X8664 && defined _MSC_VER && !defined __clang__
    return (void*) __readgsqword(__offset);
#elif defined __MCF_M_X8632 && defined _MSC_VER && !defined __clang__
    return (void*) __readfsdword(__offset);
#elif defined __MCF_M_ARM64 && defined _MSC_VER && !defined __clang__
    return (void*) __readx18qword(__offset);
#else
    return *((void**) __MCF_teb() + __offset / __MCF_64_32(8, 4));
#endif
  }

/* Stores a generic pointer at `__offset` of the environment block of the
 * current thread. This function may operate on TEB directly and may be more
 * efficient than accessing through `__MCF_teb()`.  */
__MCF_TEB_INLINE
void
__MCF_teb_store_ptr(uint32_t __offset, const void* __value)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM && defined __clang__
    *(const void* __seg_gs*)(uint64_t) __offset = __value;
#elif defined __MCF_M_X8632_ASM && defined __clang__
    *(const void* __seg_fs*) __offset = __value;
#elif defined __MCF_M_X8664 && defined _MSC_VER && !defined __clang__
    __writegsqword(__offset, (uint64_t) __value);
#elif defined __MCF_M_X8632 && defined _MSC_VER && !defined __clang__
    __writefsdword(__offset, (uint32_t) __value);
#elif defined __MCF_M_ARM64 && defined _MSC_VER && !defined __clang__
    __writex18qword(__offset, (uint64_t) __value);
#else
    *((const void**) __MCF_teb() + __offset / __MCF_64_32(8, 4)) = __value;
#endif
  }

/* Get a pointer to the process environment block of the current process in
 * the flat address space.  */
__MCF_TEB_INLINE __MCF_FN_CONST
void*
__MCF_peb(void)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    return *(void* __seg_gs*) 0x60;
#  else
    void* __peb;
    __asm__ ("mov { %%gs:0x60, %0 | %0, gs:[0x60] }" : "=r"(__peb));
    return __peb;
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    return *(void* __seg_fs*) 0x30;
#  else
    void* __peb;
    __asm__ ("mov { %%fs:0x30, %0 | %0, fs:[0x30] }" : "=r"(__peb));
    return __peb;
#  endif
#elif defined __MCF_M_ARM64_ASM
    void* __peb;
    __asm__ ("ldr %0, [x18, 0x60]" : "=r"(__peb));
    return __peb;
#elif defined __MCF_M_X8664
    return (void*) __readgsqword(0x60);
#elif defined __MCF_M_X8632
    return (void*) __readfsdword(0x30);
#elif defined __MCF_M_ARM64
    return (void*) __readx18qword(0x60);
#else
    return *((char*) __MCF_teb() + __MCF_64_32(0x60, 0x30));
#endif
  }

/* Get the ID of the current thread. This is the same value as
 * `GetCurrentThreadId()`, but as a signed integer.  */
__MCF_TEB_INLINE __MCF_FN_CONST
int32_t
__MCF_tid(void)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    return *(int32_t __seg_gs*) 0x48;
#  else
    int32_t __tid;
    __asm__ ("mov { %%gs:0x48, %k0 | %k0, gs:[0x48] }" : "=r"(__tid));
    return __tid;
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    return *(int32_t __seg_fs*) 0x24;
#  else
    int32_t __tid;
    __asm__ ("mov { %%fs:0x24, %k0 | %k0, fs:[0x24] }" : "=r"(__tid));
    return __tid;
#  endif
#elif defined __MCF_M_ARM64_ASM
    int32_t __tid;
    __asm__ ("ldr %w0, [x18, 0x48]" : "=r"(__tid));
    return __tid;
#elif defined __MCF_M_X8664
    return (int32_t) __readgsdword(0x48);
#elif defined __MCF_M_X8632
    return (int32_t) __readfsdword(0x24);
#elif defined __MCF_M_ARM64
    return (int32_t) __readx18dword(0x48);
#else
    return *(int32_t*) ((char*) __MCF_teb() + __MCF_64_32(0x48, 0x24));
#endif
  }

/* Get the ID of the current process. This is the same value as
 * `GetCurrentProcessId()`, but as a signed integer.  */
__MCF_TEB_INLINE __MCF_FN_CONST
int32_t
__MCF_pid(void)
  __MCF_noexcept
  {
#if defined __MCF_M_X8664_ASM
#  if defined __clang__
    return *(int32_t __seg_gs*) 0x40;
#  else
    int32_t __pid;
    __asm__ ("mov { %%gs:0x40, %k0 | %k0, gs:[0x40] }" : "=r"(__pid));
    return __pid;
#  endif
#elif defined __MCF_M_X8632_ASM
#  if defined __clang__
    return *(int32_t __seg_fs*) 0x20;
#  else
    int32_t __pid;
    __asm__ ("mov { %%fs:0x20, %k0 | %k0, fs:[0x20] }" : "=r"(__pid));
    return __pid;
#  endif
#elif defined __MCF_M_ARM64_ASM
    int32_t __pid;
    __asm__ ("ldr %w0, [x18, 0x40]" : "=r"(__pid));
    return __pid;
#elif defined __MCF_M_X8664
    return (int32_t) __readgsdword(0x40);
#elif defined __MCF_M_X8632
    return (int32_t) __readfsdword(0x20);
#elif defined __MCF_M_ARM64
    return (int32_t) __readx18dword(0x40);
#else
    return *(int32_t*) ((char*) __MCF_teb() + __MCF_64_32(0x40, 0x20));
#endif
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_TEB_  */
