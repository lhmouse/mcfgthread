ACLOCAL_AMFLAGS = -I m4
NOTHING =

## Compiler options
AM_CPPFLAGS = -pipe -pthread -Wall -Wextra -fPIC -DPIC -U_FORTIFY_SOURCE -DDLL_EXPORT  \
  -D_FILE_OFFSET_BITS=64 -D_POSIX_C_SOURCE=200809 -D_GNU_SOURCE -D_WIN32_WINNT=0x0600  \
  -f{strict-{aliasing,overflow},merge-all-constants}  \
  -fno-{math-errno,trapping-math,devirtualize-speculatively,stack-protector}  \
  -Werror={{,sign-}conversion,sign-compare,write-strings,return-type}  \
  -Werror={double-promotion,aggressive-loop-optimizations,missing-declarations}  \
  -W{invalid-pch,switch-enum,suggest-attribute=noreturn,shadow,missing-field-initializers}  \
  -Wunused-{function,label,local-typedefs,{,but-set-}{variable,parameter}}

AM_CFLAGS = -std=c99 -Wstrict-prototypes -Wno-old-style-declaration

AM_DEFAULT_SOURCE_EXT = .cpp
LDADD = lib/libmcfgthread.la

## Initialization
EXTRA_DIST =
noinst_LIBRARIES =
noinst_LTLIBRARIES =
TESTS = ${check_PROGRAMS}

include_HEADERS =
lib_LIBRARIES =
lib_LTLIBRARIES =
bin_PROGRAMS =

check_HEADERS =
check_LIBRARIES =
check_LTLIBRARIES =
check_PROGRAMS =

## Programs and libraries
include src/Makefile.inc.am

## Tests
include test/Makefile.inc.am
