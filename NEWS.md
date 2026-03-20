# NEWS

## v2.4

This minor release contains the following changes since v2.3-ga.1:

1. A new header, `mcfgthread/teb.h`, has been added, which provides inline
   functions for accessing fields of the Thread Environment Block (TEB) and
   the Process Environment Block (PEB). TEB-based `__MCF_tid()` and
   `__MCF_pid()` replace the previous inline assembly macros. The thread ID
   field in `_MCF_thread` is now `int32_t` (was `uint32_t`).
2. Building with Link Time Optimization (LTO) is now supported. Section
   garbage collection can be disabled with `-Denable-gc-sections=false` to
   work around linker bugs. `-funwind-tables` has been moved from per-target
   to global compiler options.
3. `_MCF_mutex_lock()` and `_MCF_mutex_unlock()` fast paths now operate on
   raw integers to work around missed compiler optimizations (see #320).
   They recognize more bit patterns so fewer calls fall through to slow
   paths. The same raw-integer approach applies to `_MCF_shared_mutex_*`
   fast paths.
4. The safety timeout in `__MCF_batch_release_common()` was 100 microseconds
   (`-1000` in NT ticks) by mistake; it is now 1 second (`-10000000`).
5. `_MCF_event_set()` now uses a CAS loop instead of an unconditional
   exchange: if the new value equals the current value, it returns
   immediately without waking any threads, avoiding thundering herd issues.
6. `_MCF_event_await_change()` now casts its `undesired` parameter to
   `uint8_t` before comparison, so only the lowest byte is compared.
7. The 128-bit reciprocal constant used by `do_divide_by_10000()` in
   `clock.c` has been corrected (from `0x68DB8BAC710CB` to
   `0x68DB8BAC710CC`), fixing an off-by-one-millisecond error in
   `_MCF_utc_now()`.
8. All slow-path wait functions (`_MCF_mutex_lock_slow`,
   `_MCF_shared_mutex_lock_shared_slow`, `_MCF_shared_mutex_lock_exclusive_slow`,
   `_MCF_once_wait_slow`, `_MCF_sem_wait`, `_MCF_event_await_change_slow`)
   now check for zero timeouts early (via `nt_timeout.__li.QuadPart == 0`)
   and return immediately without entering the wait loop.
9. `_MCF_sem_wait()` and `_MCF_cond_wait()` now issue an acquire fence after
   being notified. `_MCF_once_wait_slow()` now uses acquire-ordered loads.
10. `__MCF_seh_top()` now terminates the process (via `RaiseFailFastException`)
    on unhandled exceptions instead of continuing the search. The SEH
    registration macros have been unified into `__MCF_USING_SEH_HANDLER()`.
11. `__MCF_gthr_objc_thread_get_priority()` is no longer marked `pure`, as it
    calls the Win32 API `GetThreadPriority()`. `mtx_unlock()` in `c11.h`
    now returns `thrd_success` instead of a literal `0`.
12. Many functions on slow paths have been optimized for code size, including
    the use of explicit bit masks (`& 0x0FU`, `& (__MCF_UPTR_MAX >> 9)`)
    for field truncation, replacing `#pragma GCC diagnostic` suppressions.
13. Documentation (including updated benchmark results on multiple platforms)
    and many testsuite issues have been fixed; new tests have been added
    (e.g., `c11_call_once_unwind_seh`, `atexit_terminate`,
    `at_quick_exit_terminate`, `lazy_load`, `xglobals_name`).

Special thanks to GitHub Copilot for finding out 5-7 and the testsuite fixes!

**Full Changelog**: https://github.com/lhmouse/mcfgthread/compare/v2.3-ga.1...master
