/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2026 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.md as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_EXIT_
#define __MCFGTHREAD_EXIT_

#include "fwd.h"

__MCF_CXX(extern "C" {)
#ifndef __MCF_EXIT_IMPORT
#  define __MCF_EXIT_IMPORT
#  define __MCF_EXIT_INLINE  __MCF_GNU_INLINE
#endif

/* N1570 ISO/IEC 9899:201x
 * 7.22.4 Communication with the environment
 * 7.22.4.5 The _Exit function  */
__MCF_EXIT_IMPORT __MCF_NEVER_RETURN
void
__MCF__Exit(int __status)
  __MCF_noexcept;

/* 7.22.4.7 The quick_exit function  */
__MCF_EXIT_IMPORT __MCF_NEVER_RETURN
void
__MCF_quick_exit(int __status)
  __MCF_noexcept;

/* 7.22.4.4 The exit function  */
__MCF_EXIT_IMPORT __MCF_NEVER_RETURN
void
__MCF_exit(int __status)
  __MCF_noexcept;

__MCF_CXX(})  /* extern "C"  */
#endif  /* __MCFGTHREAD_EXIT_  */
