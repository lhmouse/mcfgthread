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

## AC_INIT(package, version, [bug-report], [tarname], [url])
AC_INIT([mcfgthread], [master], [lh_mouse@126.com], [mcfgthread], [https://github.com/lhmouse/mcfgthread])
AC_LANG([C])
AC_CONFIG_SRCDIR([mcfgthread/gthr.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC

LT_INIT([win32-dll])
LT_LANG([C])
LT_LANG([Windows Resource])

AM_INIT_AUTOMAKE([foreign subdir-objects])
#AM_SILENT_RULES([yes])

## Define ABI information
AS_VAR_SET([abi_major], [1])
AS_VAR_SET([abi_minor], [5])
AS_VAR_SET([abi_suffix], [beta.1])

## Check for assertions
AC_ARG_ENABLE([debug-checks], AS_HELP_STRING([--enable-debug-checks], [enable assertions]))
AM_CONDITIONAL([enable_debug_checks], [test "${enable_debug_checks}" == "yes"])
AM_COND_IF([enable_debug_checks], [AC_DEFINE([__MCF_DEBUG], 1, [Define to 1 to enable assertions])])

## Check for pre-compiled headers
AC_ARG_ENABLE([pch], AS_HELP_STRING([--disable-pch], [do not use pre-compiled headers]))
AM_CONDITIONAL([enable_pch], [test "${enable_pch}" != "no"])

## Finish
AC_SUBST([abi_major])
AC_SUBST([abi_minor])
AC_SUBST([abi_suffix])

AC_CONFIG_FILES([Makefile mcfgthread/version.h])
AC_OUTPUT
