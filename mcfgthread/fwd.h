/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_FWD_
#define __MCFGTHREAD_FWD_

#include "version.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#if !defined _WIN32_WINNT
#  define _WIN32_WINNT  0x0601
#elif _WIN32_WINNT < 0x0601
#  warning Please define `_WIN32_WINNT` to at least Windows 7.
#endif

#if defined __arm__ || defined _M_ARM
#  error 32-bit ARM target is not supported.
#endif

/* Define language-support macros. We start from C89 (which doesn't have an
 * identification macro) and then redefine these macros according to
 * `__STDC_VERSION__` and `__cplusplus`.  */
#define __MCF_C(...)     __VA_ARGS__
#define __MCF_CXX(...)
#define __MCF_noexcept
#define __MCF_MAY_THROW
#define __MCF_nullptr    __MCF_IPTR_0

#ifdef __cplusplus
#  undef __MCF_CXX
#  define __MCF_CXX(...)   __VA_ARGS__
#  undef __MCF_C
#  define __MCF_C(...)
#  undef __MCF_noexcept
#  define __MCF_noexcept   throw()
#  ifdef _MSC_VER
#    undef __MCF_MAY_THROW
#    define __MCF_MAY_THROW   throw(...)
#  endif
#endif

#define __MCF_C99(...)
#define __MCF_C11(...)
#define __MCF_C17(...)
#define __MCF_C23(...)

#define __MCF_CXX11(...)
#define __MCF_CXX14(...)
#define __MCF_CXX17(...)
#define __MCF_CXX20(...)
#define __MCF_CXX23(...)

#if defined __STDC_VERSION__ && (__STDC_VERSION__ >= 199901L)
#  undef __MCF_C99
#  define __MCF_C99(...)   __VA_ARGS__
#endif

#if defined __STDC_VERSION__ && (__STDC_VERSION__ >= 201112L)
#  undef __MCF_C11
#  define __MCF_C11(...)   __VA_ARGS__
#endif

#if defined __STDC_VERSION__ && (__STDC_VERSION__ >= 201710L)
#  undef __MCF_C17
#  define __MCF_C17(...)   __VA_ARGS__
#endif

#if defined __STDC_VERSION__ && (__STDC_VERSION__ >= 202311L)
#  undef __MCF_C23
#  define __MCF_C23(...)   __VA_ARGS__
#  undef __MCF_nullptr
#  define __MCF_nullptr   nullptr
#endif

#if defined __cplusplus && ((__cplusplus >= 201103L) || (defined _MSC_VER && !defined __clang__))
#  undef __MCF_CXX11
#  define __MCF_CXX11(...)   __VA_ARGS__
#  undef __MCF_noexcept
#  define __MCF_noexcept   noexcept
#  undef __MCF_MAY_THROW
#  define __MCF_MAY_THROW  noexcept(false)
#  undef __MCF_nullptr
#  define __MCF_nullptr    nullptr
#endif

#if defined __cplusplus && ((__cplusplus >= 201402L) || (defined _MSC_VER && !defined __clang__))
#  undef __MCF_CXX14
#  define __MCF_CXX14(...)   __VA_ARGS__
#endif

#if defined __cplusplus && (__cplusplus >= 201703L)
#  undef __MCF_CXX17
#  define __MCF_CXX17(...)   __VA_ARGS__
#endif

#if defined __cplusplus && (__cplusplus >= 202002L)
#  undef __MCF_CXX20
#  define __MCF_CXX20(...)   __VA_ARGS__
#endif

#if defined __cplusplus && (__cplusplus >= 202302L)
#  undef __MCF_CXX23
#  define __MCF_CXX23(...)   __VA_ARGS__
#endif

__MCF_CXX(extern "C" {)
#ifndef __MCF_FWD_IMPORT
#  define __MCF_FWD_IMPORT
#  define __MCF_FWD_INLINE  __MCF_GNU_INLINE
#endif

#define __MCF_S_(...)     #__VA_ARGS__
#define __MCF_S(...)       __MCF_S_(__VA_ARGS__)
#define __MCF_C2_(x, y)     x##y
#define __MCF_C2(x, y)       __MCF_C2_(x, y)
#define __MCF_C3_(x, y, z)   x##y##z
#define __MCF_C3(x, y, z)    __MCF_C3_(x, y, z)
#define __MCF_0_INIT           { __MCF_C(0) }
#define __MCF_SET_IF(x, ...)    ((void) ((x) && (*(x) = (__VA_ARGS__))))

/* Define compiler-specific macros. In the case of Clang-CL, prefer GNU
 * extensions to Microsoft ones.  */
#if defined __GNUC__ || defined __clang__
#  define __MCF_EX             __extension__
#  define __MCF_GNU_INLINE      extern __inline__ __attribute__((__gnu_inline__))
#  define __MCF_ALWAYS_INLINE   extern __inline__ __attribute__((__gnu_inline__, __always_inline__))
#  define __MCF_NEVER_INLINE   __attribute__((__noinline__))
#  define __MCF_NEVER_RETURN   __attribute__((__noreturn__))
#  define __MCF_FN_CONST       __attribute__((__const__))
#  define __MCF_FN_PURE       __attribute__((__pure__))
#  define __MCF_FN_COLD       __attribute__((__cold__))
#  define __MCF_ALIGNED(x)    __attribute__((__aligned__(x)))
#  define __MCF_UNREACHABLE     __builtin_unreachable()
#  define __MCF_ALT_SYM(x, fn)   extern __typeof__(x) fn __asm__(__MCF_USYM #x)
#  if defined __amd64__ && !defined __arm64ec__
#    define __MCF_64_32(x, y)  x
#    define __MCF_USYM  ""
#    define __MCF_M_X8664_ASM  1
#    define __MCF_M_X8664  1
#  elif defined __i386__
#    define __MCF_64_32(x, y)  y
#    define __MCF_USYM  "_"
#    define __MCF_M_X8632_ASM  1
#    define __MCF_M_X8632  1
#  elif defined __aarch64__ || defined __arm64ec__
#    define __MCF_64_32(x, y)  x
#    define __MCF_USYM  ""
#    define __MCF_M_ARM64_ASM  1
#    if defined __aarch64__
#      define __MCF_M_ARM64  1
#    else
#      define __MCF_M_X8664  1
#      define __MCF_M_ARM64EC  1
#    endif
#  endif
#else
#  define __MCF_EX             /* unsupported */
#  define __MCF_GNU_INLINE      __inline
#  define __MCF_ALWAYS_INLINE   __forceinline
#  define __MCF_NEVER_INLINE   __declspec(noinline)
#  define __MCF_NEVER_RETURN   __declspec(noreturn)
#  define __MCF_FN_CONST       __declspec(noalias)
#  define __MCF_FN_PURE       __declspec(noalias)
#  define __MCF_FN_COLD       /* unsupported */
#  define __MCF_ALIGNED(x)    __declspec(align(x))
#  define __MCF_UNREACHABLE     __assume(0)
#  define __MCF_ALT_SYM(x, fn)   __pragma(comment(linker, "/alternatename:" __MCF_USYM #fn "=" __MCF_USYM #x))
#  if defined _M_X64 && !defined _M_ARM64EC
#    define __MCF_64_32(x, y)  x
#    define __MCF_USYM  ""
#    define __MCF_M_X8664  1
#  elif defined _M_IX86
#    define __MCF_64_32(x, y)  y
#    define __MCF_USYM  "_"
#    define __MCF_M_X8632  1
#  elif defined _M_ARM64 || defined _M_ARM64EC
#    define __MCF_64_32(x, y)  x
#    define __MCF_USYM  ""
#    if defined __aarch64__
#      define __MCF_M_ARM64  1
#    else
#      define __MCF_M_X8664  1
#      define __MCF_M_ARM64EC  1
#    endif
#  endif
#endif

/* These are necessary when the header is compiled as C89 or C++98. The check
 * for `_LP64` is for Cygwin and MSYS2.  */
#ifdef _LP64
#  define __MCF_INT64_    long
#  define __MCF_INTPTR_   long
#else
#  define __MCF_INT64_    long long
#  define __MCF_INTPTR_   __MCF_64_32(long long, int)
#endif

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __MCF_INT64_ int64_t;
typedef unsigned __MCF_INT64_ uint64_t;
typedef __MCF_INTPTR_ intptr_t;
typedef unsigned __MCF_INTPTR_ uintptr_t;

#define __MCF_PTR_BITS    __MCF_64_32(64, 32)
#define __MCF_IPTR_MIN    __MCF_64_32((-0x7FFFFFFFFFFFFFFFLL-1), (-0x7FFFFFFF-1))
#define __MCF_IPTR_0      __MCF_64_32(0LL, 0)
#define __MCF_IPTR_MAX    __MCF_64_32(0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFF)
#define __MCF_UPTR_0      __MCF_64_32(0ULL, 0U)
#define __MCF_UPTR_MAX    __MCF_64_32(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFU)

/* I can't find a better name for this macro. It controls whether the complete
 * definitions of inline functions are compiled. If headers are included by user
 * code and they are optimizing for size, most fast-path parts are opted out.
 * Complete definitions are compiled when a user is optimizing for speed, or when
 * it's inside mcfgthread itself regardless of optimization.  */
#ifndef __MCF_EXPAND_INLINE_DEFINITIONS
#  define __MCF_EXPAND_INLINE_DEFINITIONS   0
#endif

#if defined __OPTIMIZE__ && !defined __OPTIMIZE_SIZE__
#  undef __MCF_EXPAND_INLINE_DEFINITIONS
#  define __MCF_EXPAND_INLINE_DEFINITIONS   1
#endif

/* This defines a by-reference variable, in other words, an array of exactly one
 * object, which is always passed around as a pointer.  */
#define __MCF_BR(...)    __typeof__(__VA_ARGS__ [1])

/* Some compilers warn about casts between pointers, so launder the pointer via
 * an in-between integral type.  */
#ifdef __cplusplus
#  define __MCF_CAST_PTR(T, ...)   (reinterpret_cast<T*>(reinterpret_cast<__MCF_INTPTR_>(__VA_ARGS__)))
#else
#  define __MCF_CAST_PTR(T, ...)   (__MCF_EX (T*)(__MCF_INTPTR_) (__VA_ARGS__))
#endif

/* The `__MCF_STATIC_ASSERT_0()` macro is an expression that yields zero if it
 * compiles anyway. Its argument must be a constant expression.  */
#if defined __cplusplus
extern "C++" {
template<bool> struct __MCF_static_assert_helper;
template<> struct __MCF_static_assert_helper<true> { static const int __one = 1; };
}  /* extern "C++"  */
#  define __MCF_STATIC_ASSERT_1(...)   (+::__MCF_static_assert_helper<(__VA_ARGS__)>::__one)
#elif defined __GNUC__ || defined __clang__
#  define __MCF_STATIC_ASSERT_1(...)   (__builtin_choose_expr((__VA_ARGS__), 1, __builtin_unreachable()))
#else
#  define __MCF_STATIC_ASSERT_1(...)   ((int) sizeof(struct { char __one : (__VA_ARGS__) ? 1 : -1; }))
#endif

#define __MCF_STATIC_ASSERT_0(...)   (__MCF_STATIC_ASSERT_1(__VA_ARGS__) - 1)
#define __MCF_STATIC_ASSERT(...)    extern int __MCF_static_assert_true[__MCF_STATIC_ASSERT_1(__VA_ARGS__)]

/* The `__MCF_ASSERT()` and `__MCF_CHECK()` macros perform run-time checks. If
 * an argument yields false, `__MCF_ASSERT()` results in undefined behavior,
 * and `__MCF_CHECK()` effects abnormal termination of the current program.  */
__MCF_FWD_IMPORT __MCF_NEVER_RETURN __MCF_FN_COLD
void
__MCF_runtime_failure(const char* __where)
  __MCF_noexcept;

#ifdef __MCF_DEBUG
#  undef __MCF_UNREACHABLE
#  define __MCF_UNREACHABLE   (__MCF_runtime_failure(__MCF_EX __func__))
#endif

#define __MCF_ASSERT(...)    ((__VA_ARGS__) ? (void) 0 : __MCF_UNREACHABLE)
#define __MCF_CHECK(...)    ((__VA_ARGS__) ? (void) 0 : __MCF_runtime_failure(__MCF_EX __func__))

/* Below are public declarations for users.  */
typedef void* __MCF_HANDLE;
typedef struct timespec __MCF_timespec;

typedef struct __MCF_dtor_element __MCF_dtor_element;
typedef struct __MCF_dtor_queue __MCF_dtor_queue;
typedef struct __MCF_tls_table __MCF_tls_table;
typedef struct __MCF_tls_element __MCF_tls_element;

typedef struct __MCF_cond _MCF_cond;
typedef struct __MCF_mutex _MCF_mutex;
typedef struct __MCF_shared_mutex _MCF_shared_mutex;
typedef struct __MCF_once _MCF_once;
typedef struct __MCF_sem _MCF_sem;
typedef struct __MCF_event _MCF_event;
typedef struct __MCF_thread _MCF_thread;
typedef int _MCF_thread_priority;
typedef struct __MCF_tls_key _MCF_tls_key;

/* See `_MCF_cond_wait()` for details about these callbacks.  */
typedef intptr_t _MCF_cond_unlock_callback(intptr_t __lock_arg);
typedef void _MCF_cond_relock_callback(intptr_t __lock_arg, intptr_t __unlocked);

/* Define the prototype for thread procedures.  */
typedef void _MCF_thread_procedure(_MCF_thread* __thrd);

/* Define the prototype for destructors for `_MCF_tls_key_new()`.  */
typedef void _MCF_tls_dtor(void* __ptr);

/* Define the prototype for destructors for `atexit()` and `at_quick_exit()`.  */
typedef void __MCF_atexit_callback(void);

/* Define the prototype for `call_once()`.  */
typedef void __MCF_once_callback(void);

/* Define the prototype for destructors for `__cxa_atexit()`, `__cxa_at_quick_exit()`
 * and `__cxa_thread_atexit()`.  */
typedef void __MCF_cxa_dtor_cdecl(void* __arg);
typedef void __fastcall __MCF_cxa_dtor_fastcall(void* __arg);

/* In the case of i386, the argument is passed both via the ECX register and on
 * the stack, to allow both `__cdecl` and `__thiscall` functions to work
 * properly. GCC and Clang accept `__thiscall` on non-member functions as an
 * extension, but MSVC doesn't so we use `__fastcall` there.  */
#if defined __GNUC__ || defined __clang__
#  define __MCF_CXA_DTOR_DUAL_ABI  1
__MCF_EX typedef void __thiscall __MCF_cxa_dtor_thiscall(void* __arg);
#else
typedef void __fastcall __MCF_cxa_dtor_thiscall(void* __arg);
#endif

#if defined __MCF_CXA_DTOR_DUAL_ABI || defined __cplusplus
#  define __MCF_TRANSPARENT_UNION   union __MCF_C(__attribute__((__transparent_union__)))
#  define __MCF_TRANSPARENT_UNION_FIELD(tag, type, x)  \
    __MCF_CXX(__MCF_CXX11(constexpr) tag(type x##_) __MCF_noexcept : x(x##_) { })  \
    /* ^= constructor / field => */ type x  /* no semicolon  */
typedef union __MCF_cxa_dtor_any __MCF_cxa_dtor_any_;
__MCF_TRANSPARENT_UNION __MCF_cxa_dtor_any
  {
    __MCF_TRANSPARENT_UNION_FIELD(__MCF_cxa_dtor_any, __MCF_cxa_dtor_cdecl*, __cdecl_ptr);
    __MCF_TRANSPARENT_UNION_FIELD(__MCF_cxa_dtor_any, __MCF_atexit_callback*, __nullary_ptr);
#  if defined __MCF_M_X8632
    __MCF_TRANSPARENT_UNION_FIELD(__MCF_cxa_dtor_any, __MCF_cxa_dtor_fastcall*, __fastcall_ptr);
    __MCF_TRANSPARENT_UNION_FIELD(__MCF_cxa_dtor_any, __MCF_cxa_dtor_thiscall*, __thiscall_ptr);
#  endif
  };
#else
typedef __MCF_cxa_dtor_thiscall* __MCF_cxa_dtor_any_;
#endif

/* Gets the last error number, like `GetLastError()`.  */
__MCF_FWD_IMPORT __MCF_FN_PURE
uint32_t
_MCF_get_win32_error(void)
  __MCF_noexcept;

/* Gets the system page size, which is usually 4KiB or 8KiB.  */
__MCF_FWD_IMPORT __MCF_FN_CONST
size_t
_MCF_get_page_size(void)
  __MCF_noexcept;

/* Gets the number of logical processors in the current group.  */
__MCF_FWD_IMPORT __MCF_FN_CONST
size_t
_MCF_get_processor_count(void)
  __MCF_noexcept;

/* Gets the mask of active processors. Each bit 1 denotes a processor that
 * has been configured into the system.  */
__MCF_FWD_IMPORT __MCF_FN_CONST
uintptr_t
_MCF_get_active_processor_mask(void)
  __MCF_noexcept;

/* Declare some helper functions.  */
__MCF_ALWAYS_INLINE __MCF_CXX11(constexpr)
size_t
_MCF_minz(size_t __x, size_t __y)
  __MCF_noexcept
  {
    return (__y < __x) ? __y : __x;
  }

__MCF_ALWAYS_INLINE __MCF_CXX11(constexpr)
size_t
_MCF_maxz(size_t __x, size_t __y)
  __MCF_noexcept
  {
    return (__x < __y) ? __y : __x;
  }

__MCF_ALWAYS_INLINE __MCF_CXX11(constexpr)
intptr_t
_MCF_dim(intptr_t __x, intptr_t __y)
  __MCF_noexcept
  {
    return (__x > __y) ? (__x - __y) : 0;
  }

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_FWD_  */
