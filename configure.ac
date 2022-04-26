## AC_INIT(package, version, [bug-report], [tarname], [url])
AC_INIT([mcfgthread], [master], [lh_mouse@126.com], [mcfgthread], [https://github.com/lhmouse/mcfgthread])
AC_LANG([C])
AC_CONFIG_SRCDIR([src/startup.c])
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
AS_VAR_SET([ABI_MAJOR], [1])
AS_VAR_SET([ABI_MINOR], [0])
AS_VAR_SET([ABI_SUFFIX], [beta.4])

## Check for assertions
AC_ARG_ENABLE([debug-checks], AS_HELP_STRING([--enable-debug-checks], [enable assertions]))
AM_CONDITIONAL([enable_debug_checks], [test "${enable_debug_checks}" == "yes"])
AM_COND_IF([enable_debug_checks], [AC_DEFINE([__MCF_DEBUG], 1, [Define to 1 to enable assertions])])

## Finish
AC_SUBST([ABI_MAJOR])
AC_SUBST([ABI_MINOR])
AC_SUBST([ABI_SUFFIX])

AC_CONFIG_FILES([Makefile] [src/version.h])
AC_OUTPUT
