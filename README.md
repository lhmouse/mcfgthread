# The MCF gthread library

|CI            |Category                   |Host OS         |Build Status     |Remarks          |
|:-------------|:--------------------------|:---------------|:----------------|:----------------|
|[**AppVeyor**](https://ci.appveyor.com/project/lhmouse/mcfgthread) |:1st_place_medal:Primary |Windows (MSYS2) |[![Build Status](https://ci.appveyor.com/api/projects/status/github/lhmouse/mcfgthread?branch=master&svg=true)](https://ci.appveyor.com/project/lhmouse/mcfgthread) |Standard I/O not in **UTF-32**.  |

I decide to recreate everything from scratch. Apologies for the trouble.

# How to build

You need to run these commands in a native MSYS2 shell (**MINGW32** or **MINGW64** is recommended):

```sh
$ autoreconf -i  # requires autoconf, automake and libtool
$ ./configure
$ make -j$(nproc)
```
