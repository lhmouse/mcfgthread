/* This file is part of MCF Gthread.
 * See LICENSE.TXT for licensing information.
 * Copyleft 2022, LH_Mouse. All wrongs reserved.  */

#ifndef __MCFGTHREAD_PRECOMPILED_
#define __MCFGTHREAD_PRECOMPILED_

#include "version.h"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef DLL_EXPORT
#  define __MCF_DLLEXPORT  __declspec(dllexport)
#else
#  define __MCF_DLLEXPORT
#endif

#include <minwindef.h>
#include <minwinbase.h>
#include <winternl.h>
#include <winerror.h>
#include <ntstatus.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <limits.h>

#endif  /* __MCFGTHREAD_PRECOMPILED_  */
