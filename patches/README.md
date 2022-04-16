These are patches that I use to build GCC 11 with mcfgthread support.

Normally, mingw-w64 CRT performs per-thread cleanup upon receipt of `DLL_PROCESS_DETACH` in a TLS callback (of an EXE) or the `DllMainCRTStartup()` function (of a DLL). There are two major issues in this approach:

    1. These callbacks are invoked after Windows has terminated all the other threads. If another thread is terminated while it has locked a mutex, the mutex will never get unlocked. If a destructor of a static object or a callback that has been registered with `atexit()` attempts to acquire the exact mutex, deadlocks can occur.
    2. These callbacks are still invoked if the user calls `_Exit()` or `quick_exit()`, such as [in LLVM](https://reviews.llvm.org/D102944). As specified by the C++ standard, they shall not be called.
    3. Per-thread cleanup may be performed after destructors of static objects. The C++ standard does not allow this behavior.

GCC uses `atexit()` to register destructors for static objects. Therefore, the CRT has to be modified to forward such calls to `__MCF_cxa_atexit()`, passing the address of the module-specific `__dso_handle` as its third argument to it. The modified CRT also forwards calls to `exit()`, `_Exit()`, `_exit()` and `quick_exit()` to standard-conforming ones in mcfgthread, which eventually calls `TerminateProcess()` instead of `ExitProcess()`, to address such issues. Per-thread and process cleanup is performed by `__cxa_finalize(NULL)`,in accordance with the Itanium ABI.
