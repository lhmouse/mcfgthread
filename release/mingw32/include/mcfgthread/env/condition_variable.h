// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2016, LH_Mouse. All wrongs reserved.

#ifndef __MCFCRT_ENV_CONDITION_VARIABLE_H_
#define __MCFCRT_ENV_CONDITION_VARIABLE_H_

#include "_crtdef.h"

#ifndef __MCFCRT_CONDITION_VARIABLE_INLINE_OR_EXTERN
#	define __MCFCRT_CONDITION_VARIABLE_INLINE_OR_EXTERN     __attribute__((__gnu_inline__)) extern inline
#endif

_MCFCRT_EXTERN_C_BEGIN

// In the case of static initialization, please initialize it with { 0 }.
typedef struct __MCFCRT_tagConditionVariable {
	volatile _MCFCRT_STD uintptr_t __u;
} _MCFCRT_ConditionVariable;

#define _MCFCRT_CONDITION_VARIABLE_SUGGESTED_SPIN_COUNT   1000u

typedef _MCFCRT_STD intptr_t (*_MCFCRT_ConditionVariableUnlockCallback)(_MCFCRT_STD intptr_t __nContext);
typedef void (*_MCFCRT_ConditionVariableRelockCallback)(_MCFCRT_STD intptr_t __nContext, _MCFCRT_STD intptr_t __nUnlocked);

__MCFCRT_CONDITION_VARIABLE_INLINE_OR_EXTERN void _MCFCRT_InitializeConditionVariable(_MCFCRT_ConditionVariable *__pConditionVariable) _MCFCRT_NOEXCEPT {
	__atomic_store_n(&(__pConditionVariable->__u), 0, __ATOMIC_RELEASE);
}

extern bool _MCFCRT_WaitForConditionVariable(_MCFCRT_ConditionVariable *__pConditionVariable,
	_MCFCRT_ConditionVariableUnlockCallback __pfnUnlockCallback, _MCFCRT_ConditionVariableRelockCallback __pfnRelockCallback, _MCFCRT_STD intptr_t __nContext,
	_MCFCRT_STD size_t __uMaxSpinCount, _MCFCRT_STD uint64_t __u64UntilFastMonoClock) _MCFCRT_NOEXCEPT;
extern bool _MCFCRT_WaitForConditionVariableOrAbandon(_MCFCRT_ConditionVariable *__pConditionVariable,
	_MCFCRT_ConditionVariableUnlockCallback __pfnUnlockCallback, _MCFCRT_ConditionVariableRelockCallback __pfnRelockCallback, _MCFCRT_STD intptr_t __nContext,
	_MCFCRT_STD size_t __uMaxSpinCount, _MCFCRT_STD uint64_t __u64UntilFastMonoClock) _MCFCRT_NOEXCEPT;
extern void _MCFCRT_WaitForConditionVariableForever(_MCFCRT_ConditionVariable *__pConditionVariable,
	_MCFCRT_ConditionVariableUnlockCallback __pfnUnlockCallback, _MCFCRT_ConditionVariableRelockCallback __pfnRelockCallback, _MCFCRT_STD intptr_t __nContext,
	_MCFCRT_STD size_t __uMaxSpinCount) _MCFCRT_NOEXCEPT;
extern _MCFCRT_STD size_t _MCFCRT_SignalConditionVariable(_MCFCRT_ConditionVariable *__pConditionVariable, _MCFCRT_STD size_t __uMaxCountToSignal) _MCFCRT_NOEXCEPT;
extern _MCFCRT_STD size_t _MCFCRT_BroadcastConditionVariable(_MCFCRT_ConditionVariable *__pConditionVariable) _MCFCRT_NOEXCEPT;

_MCFCRT_EXTERN_C_END

#endif
