# The MCF gthread library

|CI            |Category                   |Host OS         |Build Status     |Remarks          |
|:-------------|:--------------------------|:---------------|:----------------|:----------------|
|[**AppVeyor**](https://ci.appveyor.com/project/lhmouse/mcfgthread) |:1st_place_medal:Primary |Windows (MSYS2) |[![Build Status](https://ci.appveyor.com/api/projects/status/github/lhmouse/mcfgthread?branch=master&svg=true)](https://ci.appveyor.com/project/lhmouse/mcfgthread) ||

MCF gthread is a gthread implementation for Windows 7 and above that meets the requirements of gthread interfaces, playing a role as a replacement for the (currently working but smelly and inefficient) gthr-win32 implementation when porting GCC.

I decide to recreate everything from scratch. Apologies for the trouble.

# How to build

You need to run these commands in a native MSYS2 shell (**MINGW32** or **MINGW64** is recommended):

```sh
autoreconf -i  # requires autoconf, automake and libtool
./configure
make -j$(nproc)
make -j$(nproc) check
```

Cross-compiling from Linux is also supported:

```sh
autoreconf -i  # requires autoconf, automake and libtool
# Install cross-compilers first.
# On Debian this can be done with `sudo aptitude install gcc-mingw-w64-{i686,x86-64}`.
./configure --host=i686-w64-mingw32  # or `x86_64-w64-mingw32` for 64-bit builds
make -j$(nproc)
```

# Notes

This project is developed and tested under x86 and x64 and hasn't been tested on other CPU architectures.

This project uses some undocumented NT system calls and might be broken in future Windows versions. The author gives no warranty for this project. Use it at your own risk.
