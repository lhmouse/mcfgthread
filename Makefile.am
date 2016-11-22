ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = no-exeext foreign subdir-objects
AM_CPPFLAGS = -Wall -Wextra -pedantic -pedantic-errors -Werror	\
	-Wwrite-strings -Wconversion -Wsign-conversion -Wsuggest-attribute=noreturn -Wundef -Wshadow -Wstrict-aliasing=2 -Wstrict-overflow=5	\
	-pipe -mno-stack-arg-probe -mno-accumulate-outgoing-args -mpush-args -masm=intel
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
	echo "#include \"$<\"" > $@

__pch.h.gch: __pch.h
	${CC} -x c-header @DEFS@ ${AM_CPPFLAGS} ${CPPFLAGS} $$(echo "" "${AM_CFLAGS}" | sed "s/-include __pch\\.h//") ${CFLAGS} $< -o $@

pkginclude_HEADERS = \
	src/gthread.h	\
	src/c11thread.h

pkginclude_predir = ${pkgincludedir}/pre
pkginclude_pre_HEADERS = \
	src/pre/module.h

pkginclude_envdir = ${pkgincludedir}/env
pkginclude_env_HEADERS = \
	src/env/avl_tree.h	\
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
	src/env/_crtdef.h	\
	src/env/_gthread_inl.h	\
	src/env/_c11thread_inl.h	\
	src/env/_seh_top.h	\
	src/env/_nt_timeout.h

pkginclude_extdir = ${pkgincludedir}/ext
pkginclude_ext_HEADERS = \
	src/ext/assert.h	\
	src/ext/pp.h	\
	src/ext/expect.h	\
	src/ext/itow.h	\
	src/ext/wcpcpy.h

mcfgthread_pre_sources = \
	src/pre/module.c

mcfgthread_sources = \
	src/env/avl_tree.c	\
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
	src/env/_seh_top.c	\
	src/env/_nt_timeout.c	\
	src/ext/assert.c	\
	src/ext/itow.c	\
	src/ext/wcpcpy.c

bin_PROGRAMS = \
	mcfgthread-10.dll

mcfgthread_10_dll_SOURCES = \
	src/dll_startup.c	\
	${mcfgthread_pre_sources}	\
	${mcfgthread_sources}

mcfgthread_10_dll_LDFLAGS = \
	-shared -static -nostdlib	\
	-Wl,-e@__MCFCRT_DllStartup,--subsystem,windows,--disable-runtime-pseudo-reloc,--disable-auto-import	\
	-Wl,--export-all-symbols,--exclude-symbols,@__MCFCRT_DllStartup,--exclude-libs,ALL	\
	-Wl,--disable-stdcall-fixup,--enable-auto-image-base,--out-implib,libmcfgthread.dll.a

mcfgthread_10_dll_LDADD = \
	-lkernel32 -lntdll

lib_LIBRARIES = \
	libmcfgthread.dll.a	\
	libmcfgthread.a

libmcfgthread_dll_a_SOURCES =
libmcfgthread.dll.a: mcfgthread-10.dll

libmcfgthread_a_SOURCES =
##libmcfgthread_a_SOURCES = \
##	${mcfgthread_sources}

## Due to Windows limitation we can't link this library statically.
libmcfgthread.a: mcfgthread-10.dll
	cp --no-preserve=timestamps libmcfgthread.dll.a $@
