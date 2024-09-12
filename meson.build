## Copyright (C) 2024 by LH_Mouse <lh_mouse@126.com>
##
## Permission to use, copy, modify, and/or distribute this
## software for any purpose with or without fee is hereby granted.
##
## THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
## WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
## WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
## THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
## CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
## LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
## NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
## CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

project('mcfgthread',
        'c', 'cpp',
        version: '1.9.alpha',
        default_options: [ 'c_std=c99', 'cpp_std=c++11', 'warning_level=2' ])

#===========================================================
# List of files
#===========================================================
mcfgthread_include = [
  'mcfgthread/atomic.h',
  'mcfgthread/fwd.h',
  'mcfgthread/clock.h',
  'mcfgthread/mutex.h',
  'mcfgthread/cond.h',
  'mcfgthread/once.h',
  'mcfgthread/sem.h',
  'mcfgthread/event.h',
  'mcfgthread/thread.h',
  'mcfgthread/cxa.h',
  'mcfgthread/dtor_queue.h',
  'mcfgthread/exit.h',
  'mcfgthread/tls.h',
  'mcfgthread/gthr_aux.h',
  'mcfgthread/gthr.h',
  'mcfgthread/gthr_libobjc.h',
  'mcfgthread/c11.h',
  'mcfgthread/libcxx.h',
  'mcfgthread/cxx11.hpp',
]

mcfgthread_src_min = [
  'mcfgthread/xglobals.c',
  'mcfgthread/memcpy.c',
  'mcfgthread/memmove.c',
  'mcfgthread/memcmp.c',
  'mcfgthread/memset.c',
  'mcfgthread/fwd.c',
  'mcfgthread/clock.c',
  'mcfgthread/mutex.c',
  'mcfgthread/cond.c',
  'mcfgthread/once.c',
  'mcfgthread/sem.c',
  'mcfgthread/event.c',
  'mcfgthread/thread.c',
  'mcfgthread/cxa.c',
  'mcfgthread/dtor_queue.c',
  'mcfgthread/exit.c',
  'mcfgthread/tls.c',
  'mcfgthread/gthr_aux.c',
]

mcfgthread_src_ex = [
  'mcfgthread/gthr.c',
  'mcfgthread/gthr_libobjc.c',
  'mcfgthread/c11.c',
  'mcfgthread/libcxx.c',
]

test_src = [
  'test/win8.c',
  'test/win10.c',
  'test/self_oom.c',
  'test/memory.c',
  'test/clock.c',
  'test/winnt_timeout.c',
  'test/once_zero_timeout.c',
  'test/once_release.c',
  'test/once_abort.c',
  'test/once_timeout.c',
  'test/mutex_spin_fail.c',
  'test/mutex_spin_mask.c',
  'test/mutex_zero_timeout.c',
  'test/mutex_timeout.c',
  'test/mutex.c',
  'test/cond_timeout.c',
  'test/cond_multi_wait.c',
  'test/sem_init.c',
  'test/sem_timeout.c',
  'test/sem_value.c',
  'test/sem_overflow.c',
  'test/event_init.c',
  'test/event_timeout.c',
  'test/event_value.c',
  'test/dtor_queue_dso.c',
  'test/dtor_queue_all.c',
  'test/dtor_queue_remove.c',
  'test/cxa_atexit_dso.c',
  'test/cxa_atexit_all.c',
  'test/cxa_thread_atexit.c',
  'test/cxa_thread_atexit_exit.c',
  'test/cxa_atexit_order.c',
  'test/tls_dtor.c',
  'test/tls_xset.c',
  'test/tls_deleted_key.c',
  'test/tls_deleted_key_get_set.c',
  'test/tls_many.c',
  'test/tls_dtor_thread_exit.c',
  'test/tls_dtor_ignored_on_exit.c',
  'test/tls_dtor_ignored_on_quick_exit.c',
  'test/tls_dtor_ignored_on__Exit.c',
  'test/tls_foreign_dtor.c',
  'test/tls_foreign_xset.c',
  'test/tls_foreign_dtor_thread_exit.c',
  'test/thread_self_id.c',
  'test/thread_new_alignment.c',
  'test/thread_new_aligned.c',
  'test/thread_new_aligned_bad_alignment.c',
  'test/thread_overlarge.c',
  'test/thread_x87_precision.c',
  'test/thread_priority.c',
  'test/thread_sleep.c',
  'test/gthr_c89_pedantic.c',
  'test/gthr_inline_alias.c',
  'test/gthr_once.c',
  'test/gthr_mutex_trylock.c',
  'test/gthr_mutex_timeout.c',
  'test/gthr_mutex.c',
  'test/gthr_mutex_nonrecursive.c',
  'test/gthr_rc_mutex_trylock.c',
  'test/gthr_rc_mutex_timeout.c',
  'test/gthr_rc_mutex.c',
  'test/gthr_cond_timeout.c',
  'test/gthr_cond_consumers.c',
  'test/gthr_cond_consumers_recursive.c',
  'test/gthr_tls_dtor.c',
  'test/gthr_tls_set.c',
  'test/gthr_tls_deleted_key.c',
  'test/gthr_tls_many.c',
  'test/gthr_thread_return.c',
  'test/gthr_equal.c',
  'test/libcxx_inline_alias.c',
  'test/libcxx_once.c',
  'test/libcxx_mutex_trylock.c',
  'test/libcxx_mutex.c',
  'test/libcxx_mutex_nonrecursive.c',
  'test/libcxx_rc_mutex_trylock.c',
  'test/libcxx_rc_mutex.c',
  'test/libcxx_condvar_timeout.c',
  'test/libcxx_condvar_consumers.c',
  'test/libcxx_tls_dtor.c',
  'test/libcxx_tls_set.c',
  'test/libcxx_tls_deleted_key.c',
  'test/libcxx_tls_many.c',
  'test/libcxx_thread_return.c',
  'test/libcxx_thread_id_equal.c',
  'test/libcxx_sleep.c',
  'test/c11_c99_pedantic.c',
  'test/c11_inline_alias.c',
  'test/c11_call_once.c',
  'test/c11_mtx_trylock.c',
  'test/c11_mtx_timeout.c',
  'test/c11_mtx_timeout_unsupported.c',
  'test/c11_mtx.c',
  'test/c11_mtx_nonrecursive.c',
  'test/c11_mtx_recursive.c',
  'test/c11_mtx_recursive_trylock.c',
  'test/c11_mtx_recursive_timeout.c',
  'test/c11_cnd_timeout.c',
  'test/c11_cnd_consumers.c',
  'test/c11_cnd_consumers_recursive.c',
  'test/c11_tss_dtor.c',
  'test/c11_tss_set.c',
  'test/c11_tss_deleted_key.c',
  'test/c11_tss_many.c',
  'test/c11_thrd_return.c',
  'test/c11_thrd_equal.c',
  'test/c11_thrd_sleep.c',
  'test/c11__thrd_sleep_until.c',
  'test/atexit_thread_self.c',
  'test/atexit_order.c',
  'test/atexit_ignored_on_quick_exit.c',
  'test/atexit_ignored_on__Exit.c',
  'test/at_quick_exit_order.c',
  'test/at_quick_exit_ignored_on_exit.c',
  'test/at_quick_exit_ignored_on__Exit.c',
  'test/cxx11_no_exceptions.cpp',
  'test/cxx11_pedantic.cpp',
  'test/mutex.cpp',
  'test/mutex_ctor_constexpr.cpp',
  'test/mutex_nonrecursive.cpp',
  'test/mutex_try_lock.cpp',
  'test/timed_mutex_try_lock_until.cpp',
  'test/timed_mutex_try_lock_for.cpp',
  'test/condition_variable_consumers.cpp',
  'test/condition_variable_wait_until.cpp',
  'test/condition_variable_wait_for.cpp',
  'test/call_once_returning.cpp',
  'test/call_once_exceptional.cpp',
  'test/call_once_seh_returning.cpp',
  'test/call_once_seh_exceptional.cpp',
  'test/recursive_mutex.cpp',
  'test/recursive_mutex_try_lock.cpp',
  'test/recursive_timed_mutex_try_lock_until.cpp',
  'test/recursive_timed_mutex_try_lock_for.cpp',
  'test/thread_dtor_terminate.cpp',
  'test/thread_move_assign_terminate.cpp',
  'test/thread_move_assign_joined.cpp',
  'test/thread_join_deadlock.cpp',
  'test/thread_id.cpp',
  'test/thread_decay_copy.cpp',
  'test/this_thread_sleep_until.cpp',
  'test/this_thread_sleep_for.cpp',
  'test/tls_dtor.cpp',
  'test/tls_many.cpp',
  'test/tls_set.cpp',
]

#===========================================================
# Global configuration
#===========================================================
ver = configuration_data()
ver.set('abi_major', meson.project_version().split('.')[0])
ver.set('abi_minor', meson.project_version().split('.')[1])
ver.set_quoted('abi_string', meson.project_version())

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
cc_is_i386 = cc.has_define('__i386__')
cc_is_amd64 = cc.has_define('__amd64__')
cc_asm_intel = cc.has_argument('-masm=intel')

if cc_asm_intel
  add_project_arguments('-masm=intel', language: [ 'c', 'cpp' ])
endif

if cc_is_i386
  add_project_arguments('-msse2', '-mfpmath=sse', language: [ 'c', 'cpp' ])
endif

if get_option('enable-debug-checks')
  add_project_arguments('-D__MCF_DEBUG', language: [ 'c', 'cpp' ])
endif

add_project_arguments(
    '-D_WIN32_WINNT=0x0601', '-D__USE_MINGW_ANSI_STDIO', '-U_FORTIFY_SOURCE',
    cc.get_supported_arguments(
        '-fno-stack-protector', '-fstrict-aliasing', '-ffast-math', '-fno-ident',
        '-ffunction-sections', '-Werror=conversion', '-Werror=sign-compare',
        '-Werror=sign-conversion', '-Werror=write-strings', '-Werror=return-type',
        '-Werror=double-promotion', '-Wmissing-declarations', '-Wswitch-enum',
        '-Wmissing-field-initializers', '-Wsuggest-attribute=noreturn', '-Wshadow',
        '-Wunused-function', '-Wunused-label', '-Wunused-local-typedefs',
        '-Wunused-but-set-variable', '-Wunused-but-set-parameter',
        '-Wunused-const-variable'
    ),
    language: [ 'c', 'cpp' ])

add_project_arguments(
   cc.get_supported_arguments(
       '-Werror=discarded-qualifiers', '-Werror=strict-prototypes',
       '-Werror=incompatible-pointer-types', '-Werror=int-conversion',
       '-Werror=implicit-function-declaration', '-Werror=implicit-int'),
   language: 'c')

add_project_arguments(
   cxx.get_supported_arguments(
       '-Wno-redundant-move', '-Wzero-as-null-pointer-constant'),
   language: 'cpp')

#===========================================================
# Dependencies
#===========================================================
dep_ntdll = cc.find_library('ntdll')
dep_kernel32 = cc.find_library('kernel32')
dep_advapi32 = cc.find_library('advapi32')

#===========================================================
# Rules for headers
#===========================================================
configure_file(input: 'mcfgthread/version.h.in',
               configuration: ver,
               output: 'version.h',
               install: false)

install_headers(mcfgthread_include, preserve_path: true)
install_headers(meson.current_build_dir() / 'version.h', subdir: 'mcfgthread')

#===========================================================
# Rules for 'lib/libmcfgthread'
#===========================================================
mcfgthread_version_o = import('windows').compile_resources(
      'mcfgthread/version.rc',
      args: [ '-I.', '-c65001' ],
      depend_files: [ 'mcfgthread/version.h.in', 'mcfgthread/version.manifest' ])

lib_mcfgthread_minimal_dll = shared_library('mcfgthread-minimal',
      c_pch: 'mcfgthread/xprecompiled.h',
      c_args: [ '-ffreestanding', '-fasynchronous-unwind-tables', '-DDLL_EXPORT' ],
      link_args: [ '-nostdlib', '-Wl,--entry,__MCF_dll_startup@@Z',
                   '-Wl,--enable-auto-image-base', '-Wl,--subsystem,windows:6.1',
                   '-Wl,--exclude-all-symbols', '-Wl,--kill-at' ],
      sources: [ mcfgthread_src_min, mcfgthread_version_o ],
      dependencies: [ dep_kernel32, dep_ntdll ],
      soversion: ver.get('abi_major'),
      version: '.'.join([ ver.get('abi_major'), ver.get('abi_minor'), '0' ]),
      install: true)

lib_mcfgthread_dll = shared_library('mcfgthread',
      c_pch: 'mcfgthread/xprecompiled.h',
      c_args: [ '-ffreestanding', '-fasynchronous-unwind-tables', '-DDLL_EXPORT' ],
      link_args: [ '-nostdlib', '-Wl,--entry,__MCF_dll_startup@@Z',
                   '-Wl,--enable-auto-image-base', '-Wl,--subsystem,windows:6.1',
                   '-Wl,--exclude-all-symbols', '-Wl,--kill-at' ],
      sources: [ mcfgthread_src_min, mcfgthread_src_ex, mcfgthread_version_o ],
      dependencies: [ dep_kernel32, dep_ntdll ],
      soversion: ver.get('abi_major'),
      version: '.'.join([ ver.get('abi_major'), ver.get('abi_minor'), '0' ]),
      install: true)

lib_mcfgthread_a = static_library('mcfgthread',
      c_pch: 'mcfgthread/xprecompiled.h',
      c_args: [ '-ffreestanding', '-fasynchronous-unwind-tables' ],
      sources: [ mcfgthread_src_min, mcfgthread_src_ex, ],
      dependencies: [ dep_kernel32, dep_ntdll ],
      install: true)

import('pkgconfig').generate(lib_mcfgthread_dll,
      name: 'mcfgthread',
      url: 'https://github.com/lhmouse/mcfgthread',
      description: 'Cornerstone library for C++11 threading on mingw-w64',
      libraries: '-lntdll')

#===========================================================
# Rules for tests
#===========================================================
foreach src: test_src
  test_c_args = []
  test_cpp_args = []
  test_dependencies = []
  test_link_with = lib_mcfgthread_dll

  if src in [ 'test/win8.c', 'test/win10.c', 'test/self_oom.c' ]
    test_link_with = lib_mcfgthread_a
  endif

  if src == 'test/memory.c'
    test_dependencies = [ dep_advapi32, dep_ntdll ]
  endif

  if src == 'test/gthr_c89_pedantic.c'
    test_c_args = [ '-std=c89', '-Wpedantic', '-Wno-variadic-macros',
                    '-Wno-long-long', '-Werror=declaration-after-statement' ]
  endif

  if src == 'test/c11_c99_pedantic.c'
    test_c_args = [ '-std=c99', '-Wpedantic' ]
  endif

  if src == 'test/cxx11_no_exceptions.cpp'
    test_cpp_args = [ '-std=c++11', '-fno-rtti', '-fno-exceptions' ]
  endif

  if src == 'test/cxx11_pedantic.cpp'
    test_cpp_args = [ '-std=c++11', '-Wpedantic' ]
  endif

  test_exe = executable(src.underscorify(), src,
                        c_args: test_c_args,
                        cpp_args: test_cpp_args,
                        link_args: [ '-static-libgcc', '-static-libstdc++' ],
                        dependencies: test_dependencies,
                        link_with: test_link_with,
                        install: false)

  test('..' / src, test_exe)
endforeach
