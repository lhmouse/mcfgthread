// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCFGTHREAD_THREAD_H_
#define __MCFGTHREAD_THREAD_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

// Define the thread information struct.
struct _MCF_thread_control
  {
    int __nref[1];  // atomic reference count
    uint32_t __tid;  // thread id
    void* __handle;  // win32 thread handle

    _MCF_thread_procedure* __proc;  // user-defined thread procedure
    intptr_t __params[4];  // user-defined data
  };

// This is the per-thread cleanup callback. It is declared here for
// the sake of completeness and is not meant to be call directly.
void
__MCF_thread_exit_callback(void) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCFGTHREAD_THREAD_H_
