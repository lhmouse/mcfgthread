ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = no-exeext foreign subdir-objects
AM_CPPFLAGS = -Wall -Wextra -pedantic -pedantic-errors -Werror -Wno-error=unused-parameter -Winvalid-pch	\
	-Wwrite-strings -Wconversion -Wsign-conversion -Wdouble-promotion -Wsuggest-attribute=noreturn -Wundef -Wshadow -Wstrict-aliasing=2	\
	-pipe -mno-stack-arg-probe -masm=intel -municode -ffreestanding	\
	-D__MCFCRT_NO_GENERAL_INCLUDES
AM_CFLAGS = -include __pch.h -std=c11 -Wstrict-prototypes

## I think you GNU people should just STFU and stop confusing the linker.
EXEEXT =

BUILT_SOURCES = \
	__pch.h	\
	__pch.h.gch

CLEANFILES = \
	__pch.h	\
	__pch.h.gch

__pch.h: ${srcdir}/src/env/_crtdef.h
	echo "#include \"$$(cygpath -m "$<" || readlink -f "$<")\"" > $@

__pch.h.gch: __pch.h
	${CC} -x c-header @DEFS@ ${AM_CPPFLAGS} ${CPPFLAGS} $$(echo "" "${AM_CFLAGS}" | sed "s/-include __pch\\.h//") ${CFLAGS} -Wno-error $< -o $@

pkginclude_HEADERS = \
	src/mcfcrt.h	\
	src/gthread.h	\
	src/c11thread.h

pkginclude_envdir = ${pkgincludedir}/env
pkginclude_env_HEADERS = \
	src/env/_crtdef.h	\
	src/env/_seh_top.h	\
	src/env/_nt_timeout.h	\
	src/env/_mopthread.h	\
	src/env/_tls_common.h	\
	src/env/_atexit_queue.h	\
	src/env/avl_tree.h	\
	src/env/xassert.h	\
	src/env/pp.h	\
	src/env/expect.h	\
	src/env/bail.h	\
	src/env/clocks.h	\
	src/env/condition_variable.h	\
	src/env/gthread.h	\
	src/env/c11thread.h	\
	src/env/heap.h	\
	src/env/mcfwin.h	\
	src/env/mutex.h	\
	src/env/once_flag.h	\
	src/env/thread.h	\
	src/env/crt_module.h	\
	src/env/tls.h	\
	src/env/inline_mem.h

pkginclude_extdir = ${pkgincludedir}/ext
pkginclude_ext_HEADERS = \
	src/ext/itow.h	\
	src/ext/wcpcpy.h

mcfgthread_sources = \
	src/mcfcrt.c	\
	src/env/_seh_top.c	\
	src/env/_nt_timeout.c	\
	src/env/_mopthread.c	\
	src/env/_tls_common.c	\
	src/env/avl_tree.c	\
	src/env/xassert.c	\
	src/env/bail.c	\
	src/env/clocks.c	\
	src/env/condition_variable.c	\
	src/env/gthread.c	\
	src/env/c11thread.c	\
	src/env/heap.c	\
	src/env/mutex.c	\
	src/env/once_flag.c	\
	src/env/thread.c	\
	src/env/crt_module.c	\
	src/env/tls.c	\
	src/ext/itow.c	\
	src/ext/wcpcpy.c

bin_PROGRAMS = \
	mcfgthread-12.dll

mcfgthread_12_dll_SOURCES = \
	src/dll_startup.c	\
	${mcfgthread_sources}

mcfgthread_12_dll_LDFLAGS = \
	-shared -static -nostdlib	\
	-Wl,-e@__MCFCRT_DllStartup,--subsystem,windows:6.0,--disable-runtime-pseudo-reloc,--disable-auto-import	\
	-Wl,--export-all-symbols,--exclude-symbols,@__MCFCRT_DllStartup,--exclude-libs,ALL	\
	-Wl,--disable-stdcall-fixup,--enable-auto-image-base,--out-implib,libmcfgthread.dll.a

mcfgthread_12_dll_LDADD = \
	-lkernel32 -lntdll

lib_LIBRARIES = \
	libmcfgthread.dll.a	\
	libmcfgthread.a

libmcfgthread_dll_a_SOURCES =
libmcfgthread.dll.a: mcfgthread-12.dll

libmcfgthread_a_SOURCES =
##libmcfgthread_a_SOURCES = \
##	${mcfgthread_sources}

## TODO: Modify MinGW-w64 startup code to perform CRT initialization before __main().
libmcfgthread.a: mcfgthread-12.dll
	cp --no-preserve=timestamps libmcfgthread.dll.a $@
