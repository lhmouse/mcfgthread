# The MCF Gthread Library

|CI            |Category                   |Host OS         |Build Status     |Remarks          |
|:-------------|:--------------------------|:---------------|:----------------|:----------------|
|[**AppVeyor**](https://ci.appveyor.com/project/lhmouse/mcfgthread) |:1st_place_medal:Primary |Windows (MSYS2) |[![Build Status](https://ci.appveyor.com/api/projects/status/github/lhmouse/mcfgthread?branch=master&svg=true)](https://ci.appveyor.com/project/lhmouse/mcfgthread) ||

**MCF Gthread** is a threading support library for **Windows 7** and above that implements the _gthread interface set_, which is used internally both by **GCC** to provide synchronization of initialization of local static objects, and by **libstdc++** to provide C++11 threading facilities.

I decide to recreate everything from scratch. Apologies for the trouble.

# How to build

You need to run these commands in a native MSYS2 shell (**MINGW32** or **MINGW64** is recommended):

```sh
autoreconf -i  # requires autoconf, automake and libtool
./configure
make -j$(nproc)
make -j$(nproc) check
```

Cross-compiling from Linux is also supported:

```sh
autoreconf -i  # requires autoconf, automake and libtool
# Install cross-compilers first.
# On Debian this can be done with `sudo aptitude install gcc-mingw-w64-{i686,x86-64}`.
./configure --host=i686-w64-mingw32  # or `x86_64-w64-mingw32` for 64-bit builds
make -j$(nproc)
```

# Notes

In order for `__cxa_atexit()` (and the non-standard `__cxa_at_quick_exit()`) to conform to the Itanium C++ ABI, it is required 1) for a process to call `__cxa_finalize(NULL)` when exiting, and 2) for a DLL to call `__cxa_finalize(&__dso_handle)` when it is unloaded dynamically. This requires [hacking the CRT](https://github.com/lhmouse/MINGW-packages/blob/master/mingw-w64-crt-git/9000-crt-Remove-stuff-that-has-been-provided-by-mcfgthrea.patch). If you don't have the modified CRT, you may still get standard compliance by 1) calling `__MCF_exit()` instead of `exit()` from your program, and 2) calling `__cxa_finalize(&__dso_handle)` followed by `fflush(NULL)` upon receipt of `DLL_PROCESS_DETACH` in your `DllMain()`.

This project is developed and tested on x86 and x64 and hasn't been tested on other CPU architectures.

This project uses some undocumented NT system calls and might be broken in future Windows versions. The author gives no warranty for this project. Use it at your own risk.

# Benchmarking

[The test program](mutex_performance.c) was compiled and run on a **Windows 10** machine with a 10-core **Intel i9 10900K** processor.

* **#THREADS**: number of threads
* **#ITERATIONS**: number of iterations per thread
* **SRWLOCK**: Windows `SRWLOCK`
* **CRITICAL_SECTION**: Windows `CRITICAL_SECTION`
* **WINPTHREAD**: winpthread `pthread_mutex_t`
* **MCFGTHREAD**: mcfgthread `__gthread_t` with `-fno-inline`

| #THREADS | #ITERATIONS |       SRWLOCK | CRITICAL_SECTION |    WINPTHREAD |    MCFGTHREAD |
|---------:|------------:|--------------:|-----------------:|--------------:|--------------:|
|        1 |  20,000,000 |  1541.035 ms  |     1684.556 ms  |**1537.788 ms**|  1539.504 ms  |
|        2 |  10,000,000 |  1410.687 ms  |     1916.520 ms  |  2135.853 ms  |**1377.103 ms**|
|        4 |   5,000,000 |  2070.238 ms  |     4613.832 ms  |  2979.166 ms  |**1553.278 ms**|
|        6 |   3,000,000 |  2500.003 ms  |     5016.650 ms  |  3159.182 ms  |**1409.130 ms**|
|       10 |   1,500,000 |  2416.953 ms  |     6239.123 ms  |  3004.653 ms  |**1177.269 ms**|
|       20 |     600,000 |  2266.024 ms  |     8687.350 ms  |  2559.691 ms  |**1001.314 ms**|
|       60 |     200,000 |**2831.348 ms**|    10164.012 ms  |  3814.880 ms  |  3299.509 ms  |
|      200 |      60,000 |**2849.850 ms**|    10544.007 ms  |  3825.518 ms  |  3579.925 ms  |

# Implementation details

### The condition variable

A condition variable is implemented as an atomic counter of threads that are currently waiting on it. Initially the counter is zero, which means no thread is waiting.

When a thread is about to start waiting on a condition variable, it increments the counter and suspends itself using the global keyed event, passing the address of the condition variable as the key. Another thread may read the counter to tell how many threads that it will have to wake up (note this has to be atomic), and release them from the global keyed event, also passing the address of the condition variable as the key.

### The primitive mutex

A primitive mutex is just a condition variable with a boolean bit, which designates whether the mutex is LOCKED. A mutex is initialized to all-bit zeroes which means it is unlocked and no thread is waiting.

When a thread wishes to lock a mutex, it checks whether the LOCKED bit is clear. If so, it sets the LOCKED bit and returns, having taken ownership of the mutex. If the LOCKED bit has been set by another thread, it goes to wait on the condition variable. If the thread wishes to unlock this mutex, it clears the LOCKED bit and wakes up at most one waiting thread on the condition variable, if any.

### The 'real' mutex

In reality, critical sections are fairly small. If a thread fails to lock a mutex, it might be able to do so soon, and we don't want it to give up its time slice as a syscall is an overkill. Therefore, it is reasonable for a thread to perform some spinning (busy waiting), before it actually decides to sleep.

This could however lead to severe problems in case of heavy contention. When there are hundreds of thread attempting to lock the same mutex, the system scheduler has no idea whether they are spinning or not. As it is likely that a lot of threads will eventually give up spinning and make a syscall to sleep, we are wasting a lot of CPU time and aggravating the situation.

This issue is ultimately solved by mcfgthread by encoding a spin failure counter in each mutex. If a thread gives up spinning because it couldn't lock the mutex within a given number of iterations, the spin failure counter is incremented. If a thread locks a mutex successfully while it is spinning, the spin failure counter is decremented. This counter provides a heuristic way to determine how heavily a mutex is seized. If there have been many spin failures, newcomers will not attempt to spin, but will make a syscall to sleep on the mutex directly.

### The once-initialization flag

A once-initialization flag contains a READY byte (this is the first one according to Itanium ABI) which indicates whether initialization has completed. The other bytes are used as a primitive mutex.

A thread that sees the READY byte set to non-zero knows initialization has been done, so it will return immediately. A thread that sees the READY byte set to zero will lock the bundled primitive mutex, and shall perform initialization thereafter. If initialization fails, it unlocks the primitive mutex without setting the READY byte, so the next thread that locks the primitive mutex will perform initialization. If initialization is successful, it sets the READY byte and unlocks the primitive mutex, releasing all threads that are waiting on it. (Do you remember that a primitive mutex actually contains a condition variable?)
