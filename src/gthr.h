// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_GTHR_H_
#define __MCFGTHREAD_GTHR_H_

#include "fwd.h"
#include "tls.h"
#include "once.h"
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include <time.h>  // struct timespec
#include <errno.h>  // E* macros

#ifdef __cplusplus
extern "C" {
#endif

// Define wrappers as required by 'gthr-default.h'.
#ifndef __MCFGTHREAD_GTHR_C_
#  define __MCFGTHREAD_GTHR_INLINE  __MCF_GNU_INLINE
#else
#  define __MCFGTHREAD_GTHR_INLINE
#endif



#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_GTHR_H_
