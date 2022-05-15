ACLOCAL_AMFLAGS = -I m4
NOTHING =

## Compiler options
AM_CPPFLAGS = -pipe -Wall -Wextra -Isrc -U_FORTIFY_SOURCE -D_WIN32_WINNT=0x0601  \
  -f{strict-{aliasing,overflow},merge-all-constants}  \
  -fno-{math-errno,trapping-math,stack-protector,align-{functions,jumps,loops},ident}  \
  -Werror={conversion,sign-{compare,conversion},write-strings,return-type}  \
  -Werror={double-promotion,aggressive-loop-optimizations,missing-declarations}  \
  -W{switch-enum,suggest-attribute=noreturn,shadow,missing-field-initializers}  \
  -Wunused-{function,label,local-typedefs,{,but-set-}{variable,parameter}}

AM_CFLAGS = -std=c99  \
  -Werror={discarded-qualifiers,int-conversion,incompatible-pointer-types}  \
  -Werror={strict-prototypes,implicit-{function-declaration,int}}

AM_RCFLAGS = -Isrc -c65001
AM_DLLTOOLFLAGS =

AM_DEFAULT_SOURCE_EXT = .c
SUFFIXES = .rc .def
LDADD = lib/libmcfgthread.la

## Initialization
noinst_LIBRARIES =
noinst_LTLIBRARIES =

include_HEADERS =
lib_LIBRARIES =
lib_LTLIBRARIES =
bin_PROGRAMS =

check_HEADERS =
check_LIBRARIES =
check_LTLIBRARIES =
check_PROGRAMS =

BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =
TESTS = ${check_PROGRAMS}

## Programs and libraries
include src/Makefile.inc.am

## Tests
include test/Makefile.inc.am
