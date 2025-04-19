#!/bin/bash -e

if test -z ${CC}
then
  if grep -Eoi '^(mingw|msys|cygwin)' /proc/version &>/dev/null
  then
    CC=gcc
  else
    CC=x86_64-w64-mingw32-gcc
  fi
fi

if test -z ${CXX}
then
  if grep -Eoi '^(mingw|msys|cygwin)' /proc/version &>/dev/null
  then
    CXX=g++
  else
    CXX=x86_64-w64-mingw32-g++
  fi
fi

export CPPFLAGS="-D_WIN32_WINNT=0x0601 -Ibuild_debug"
export CC CXX

find ${PWD}/"mcfgthread" -name "*.[hc]"  \
  | xargs -n 1 -P $(nproc) --  \
    sh -xec '${CC} ${CPPFLAGS} -x c -std=c99 $1 -S -o /dev/null' 0

find ${PWD}/"mcfgthread" -name "*.[hc]pp"  \
  | xargs -n 1 -P $(nproc) --  \
    sh -xec '${CXX} ${CPPFLAGS} -x c++ -std=c++11 $1 -S -o /dev/null' 0
