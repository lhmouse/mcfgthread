/* This file is part of MCF Gthread.
 * Copyright (C) 2022-2024 LH_Mouse. All wrongs reserved.
 *
 * MCF Gthread is free software. Licensing information is included in
 * LICENSE.TXT as a whole. The GCC Runtime Library Exception applies
 * to this file.  */

#ifndef __MCFGTHREAD_EXIT_
#define __MCFGTHREAD_EXIT_

#include "fwd.h"

__MCF_C_DECLARATIONS_BEGIN
#ifndef __MCF_EXIT_IMPORT
#  define __MCF_EXIT_IMPORT
#  define __MCF_EXIT_INLINE  __MCF_GNU_INLINE
#endif

/* Declare 'real' functions here.  */
__MCF_EXIT_IMPORT __MCF_NEVER_RETURN
void
__MCF__Exit(int __status) __MCF_NOEXCEPT;

__MCF_EXIT_IMPORT __MCF_NEVER_RETURN
void
__MCF_quick_exit(int __status) __MCF_NOEXCEPT;

__MCF_EXIT_IMPORT __MCF_NEVER_RETURN
void
__MCF_exit(int __status) __MCF_NOEXCEPT;

__MCF_C_DECLARATIONS_END
#endif  /* __MCFGTHREAD_EXIT_  */
