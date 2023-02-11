## Copyright (C) 2022 - 2023 by LH_Mouse <lh_mouse@126.com>
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

ACLOCAL_AMFLAGS = -I m4
END =

## Compiler options
AM_CPPFLAGS = -pipe -Wall -Wextra -Isrc  \
  -U_FORTIFY_SOURCE -D_WIN32_WINNT=0x0601 -D_POSIX_C_SOURCE=200809  \
  -f{strict-{aliasing,overflow},merge-all-constants,fast-math}  \
  -fno-{stack-protector,align-{functions,jumps,loops},ident}  \
  -Werror={conversion,sign-{compare,conversion},write-strings}  \
  -Werror={return-type,double-promotion,missing-declarations}  \
  -W{missing-field-initializers,suggest-attribute=noreturn,shadow}  \
  -W{switch-enum,unused-{function,label,local-typedefs}}  \
  -Wunused-but-set-{variable,parameter}

AM_CFLAGS = -std=c99  \
  -Werror={discarded-qualifiers,incompatible-pointer-types}  \
  -Werror={strict-prototypes,int-conversion}  \
  -Werror=implicit-{function-declaration,int}

AM_RCFLAGS = -Isrc -c65001

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
