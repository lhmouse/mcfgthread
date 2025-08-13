#!/bin/bash -e

if ! grep -Eoi '^(mingw|msys|cygwin)' /proc/version &>/dev/null
then
  _cross_file='--cross-file meson_cross/gcc.x86_64-w64-mingw32'
fi

meson setup ${_cross_file}  \
  -Ddebug=true -Doptimization=0 -Denable-debug-checks=true  \
  build_debug

meson setup ${_cross_file}  \
  -Ddebug=true -Doptimization=s  \
  build_release

meson setup ${_cross_file}  \
  -Ddebug=true -Doptimization=3  \
  build_optimized
