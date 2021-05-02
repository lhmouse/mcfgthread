// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2019, LH_Mouse. All wrongs reserved.

#ifndef __MCFCRT_ENV_SEH_TOP_H_
#define __MCFCRT_ENV_SEH_TOP_H_

#include "_crtdef.h"
#include "mcfwin.h"

_MCFCRT_EXTERN_C_BEGIN

__MCFCRT_C_CDECL extern EXCEPTION_DISPOSITION __MCFCRT_SehTopDispatcher(EXCEPTION_RECORD *__pRecord, void *__pEstablisherFrame, CONTEXT *__pContext, void *__pDispatcherContext) _MCFCRT_NOEXCEPT;

__attribute__((__unused__, __always_inline__, __artificial__)) static inline void __MCFCRT_SehTopInstaller_x86(void *__pSehNode) _MCFCRT_NOEXCEPT
  {
    void *__pUnused;
    __asm__ volatile (
      "mov{l} {%%fs:0, %0|%0, dword ptr fs:[0]} \n"
      "mov{l} {%0, (%1)|dword ptr[%1], %0} \n"
      "mov{l} {%2, 4(%1)|dword ptr[%1 + 4], %2} \n"
      "mov{l} {%1, %%fs:0|dword ptr fs:[0], %1} \n"
      : "=&r"(__pUnused) : "r"(__pSehNode), "p"(__MCFCRT_SehTopDispatcher)
    );
  }

__attribute__((__unused__, __always_inline__, __artificial__)) static inline void __MCFCRT_SehTopUninstaller_x86(void *__pSehNode) _MCFCRT_NOEXCEPT
  {
    void *__pUnused;
    __asm__ volatile (
      "mov{l} {(%1), %0|%0, dword ptr[%1]} \n"
      "mov{l} {%0, %%fs:0|dword ptr fs:[0], %0} \n"
      : "=&r"(__pUnused) : "r"(__pSehNode)
    );
  }

_MCFCRT_EXTERN_C_END

#ifdef _WIN64
#  define __MCFCRT_SEH_TOP_BEGIN        { __asm__ volatile (  \
                                            ".seh_handler __MCFCRT_SehTopDispatcher, @except \n");  \
                                          {
#  define __MCFCRT_SEH_TOP_END            }  \
                                        }
#else
#  define __MCFCRT_SEH_TOP_BEGIN        { void *__aSehNode[2]  \
                                            __attribute__((__cleanup__(__MCFCRT_SehTopUninstaller_x86)));  \
                                          __MCFCRT_SehTopInstaller_x86(&__aSehNode);  \
                                          {
#  define __MCFCRT_SEH_TOP_END            }  \
                                        }
#endif

#endif
