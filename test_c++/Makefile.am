ACLOCAL_AMFLAGS = -I m4
END =

## Compiler options
AM_CPPFLAGS = -pipe -Wall -Wextra -Wpedantic  \
  -Werror={conversion,sign-{compare,conversion},write-strings}  \
  -Werror={return-type,double-promotion,missing-declarations}  \
  -W{missing-field-initializers,suggest-attribute=noreturn,shadow}  \
  -W{switch-enum,unused-{function,label,local-typedefs}}  \
  -Wunused-but-set-{variable,parameter}

AM_CXXFLAGS = -std=c++11 -Wpedantic

AM_DEFAULT_SOURCE_EXT = .cpp
LDADD = ../lib/libmcfgthread.la
TESTS = ${check_PROGRAMS}

check_PROGRAMS =  \
  %reldir%/mutex.test  \
  %reldir%/mutex_ctor_constexpr.test  \
  %reldir%/mutex_nonrecursive.test  \
  %reldir%/mutex_try_lock.test  \
  %reldir%/timed_mutex_try_lock_until.test  \
  %reldir%/timed_mutex_try_lock_for.test  \
  %reldir%/condition_variable_consumers.test  \
  %reldir%/condition_variable_wait_until.test  \
  %reldir%/condition_variable_wait_for.test  \
  %reldir%/call_once_returning.test  \
  %reldir%/call_once_exceptional.test  \
  %reldir%/recursive_mutex.test  \
  %reldir%/recursive_mutex_try_lock.test  \
  %reldir%/recursive_timed_mutex_try_lock_until.test  \
  %reldir%/recursive_timed_mutex_try_lock_for.test  \
  %reldir%/thread_dtor_terminate.test  \
  %reldir%/thread_move_assign_terminate.test  \
  %reldir%/thread_move_assign_joined.test  \
  %reldir%/thread_join_deadlock.test  \
  %reldir%/thread_id.test  \
  %reldir%/tls_dtor.test  \
  %reldir%/tls_many.test  \
  %reldir%/tls_set.test  \
  ${END}
