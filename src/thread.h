// This file is part of MCF gthread.
// See LICENSE.TXT for licensing information.
// Copyleft 2022, LH_Mouse. All wrongs reserved.

#ifndef __MCF_THREAD_H_
#define __MCF_THREAD_H_

#include "fwd.h"

#ifdef __cplusplus
extern "C" {
#endif

// Define the thread information struct.
struct __MCF_thread_control
  {
    int __nref[1];  // atomic reference count
    uint32_t __tid;  // thread id
    void* __handle;  // win32 thread handle

    __MCF_thread_procedure* __proc;  // user-defined thread procedure
    intptr_t __params[4];  // user-defined data
  };

// This is the per-thread cleanup callback.
// It is declared here for the sake of completeness. Users are not meant to call it directly.
void __stdcall
__MCF_on_thread_detach(__MCF_thread_control* __control) __MCF_NOEXCEPT;

#ifdef __cplusplus
}
#endif

#endif  // __MCF_THREAD_H_
