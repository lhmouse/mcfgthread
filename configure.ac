## AC_INIT(package, version, [bug-report], [tarname], [url])
AC_INIT([mcfgthread], [master], [lh_mouse@126.com], [mcfgthread], [https://github.com/lhmouse/mcfgthread])
AC_LANG([C])
AC_CONFIG_SRCDIR([src/gthr.c])
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
AS_VAR_SET([abi_minor], [2])
AS_VAR_SET([abi_suffix], [ga.2])

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

AC_CONFIG_FILES([Makefile] [src/version.h])
AC_OUTPUT
