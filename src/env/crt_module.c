// This file is part of MCFCRT.
// See MCFLicense.txt for licensing information.
// Copyleft 2013 - 2017, LH_Mouse. All wrongs reserved.

#include "crt_module.h"
#include "_atexit_queue.h"
#include "mcfwin.h"
#include "bail.h"

#undef GetCurrentProcess
#define GetCurrentProcess()  ((HANDLE)-1)

#undef GetCurrentThread
#define GetCurrentThread()   ((HANDLE)-2)

static __MCFCRT_AtExitQueue g_vAtQuickExitQueue = __MCFCRT_ATEXIT_QUEUE_INIT;

static void Dispose_at_quick_exit_queue(void){
	__MCFCRT_AtExitElement vElement;
	while(__MCFCRT_AtExitQueuePop(&vElement, &g_vAtQuickExitQueue)){
		__MCFCRT_AtExitQueueInvoke(&vElement);
	}
}

bool _MCFCRT_AtCrtModuleQuickExit(_MCFCRT_AtCrtModuleExitCallback pfnProc, intptr_t nContext){
	const __MCFCRT_AtExitElement vElement = { pfnProc, nContext };
	return __MCFCRT_AtExitQueuePush(&g_vAtQuickExitQueue, &vElement);
}

void __MCFCRT_DiscardCrtModuleQuickExitCallbacks(void){
	__MCFCRT_AtExitQueueClear(&g_vAtQuickExitQueue);
}

static volatile DWORD s_dwExitingThreadId = 0;

static void CheckExitingThread(unsigned uExitCode){
	const DWORD dwCurrentThreadId = GetCurrentThreadId();
	DWORD dwOldExitingThreadId = 0;
	__atomic_compare_exchange_n(&s_dwExitingThreadId, &dwOldExitingThreadId, dwCurrentThreadId, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
	if(dwOldExitingThreadId != 0){
		if(dwOldExitingThreadId == dwCurrentThreadId){
			_MCFCRT_Bail(L"_MCFCRT_QuickExit() or _MCFCRT_Exit() is called recursively.\n"
				"This is probably caused by calling exit() or quick_exit() inside the destructor of a static or thread_local object.");
		}
		ExitThread(uExitCode);
		__builtin_trap();
	}
}

_Noreturn void _MCFCRT_ImmediateExit(unsigned uExitCode){
	TerminateProcess(GetCurrentProcess(), uExitCode);
	__builtin_trap();
}
_Noreturn void _MCFCRT_QuickExit(unsigned uExitCode){
	CheckExitingThread(uExitCode);
	Dispose_at_quick_exit_queue();
	_MCFCRT_ImmediateExit(uExitCode);
	__builtin_trap();
}
_Noreturn void _MCFCRT_Exit(unsigned uExitCode){
	CheckExitingThread(uExitCode);
	ExitProcess(uExitCode);
	__builtin_trap();
}
