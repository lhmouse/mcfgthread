# NEWS

## v2.4

This minor release contains the following changes since v2.3-ga.1:

1. A new header, `mcfgthread/teb.h`, has been introduced for accessing the
   Thread Environment Block (TEB).
2. Building with Link Time Optimization (LTO) is now supported, which can be
   enabled by passing `-Db_lto=true` to `meson setup`.
3. `_MCF_mutex_lock()` and `_MCF_mutex_unlock()` now recognize more patterns
   in fast paths, so they no longer take slow paths due to intermittent
   contention.
4. The safety timeout value in `__MCF_batch_release_common()` was set to 100
   microseconds by mistake. It is now set to 1 second.
5. When `_MCF_event_set()` doesn't change the value of an event, it no longer
   effects thundering herd issues.
6. `_MCF_event_await_change()` now only compares the lowest byte of its second
   parameter.
7. An issue which caused the result of `_MCF_utc_now()` to be off by one
   millisecond has been fixed.
8. Wait operations on synchronization objects now handle zero timeouts more
   efficiently.
9. Many functions on slow paths of a program have been optimized for size.
10. Documentation (including updated benchmark results) and many testsuite
    issues have been fixed.

Special thanks to GitHub Copilot for finding out 5-7 and the testsuite fixes!

**Full Changelog**: https://github.com/lhmouse/mcfgthread/compare/v2.3-ga.1...master
