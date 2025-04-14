#!/bin/bash -e

# setup
export CFLAGS="-D_WIN32_WINNT=0x0601 -Ibuild_debug -std=c99"
export CXXFLAGS="-D_WIN32_WINNT=0x0601 -Ibuild_debug -std=c++11"

if test -z ${CC}
then
  if grep -Eoi '^(mingw|msys|cygwin)' /proc/version &>/dev/null
  then
    CC=gcc
  else
    CC=x86_64-w64-mingw32-gcc
  fi
fi

for _f in $(find ~+/"mcfgthread" -name "*.[hc]")
do
  echo "Checking includes:  ${_f}"
  ${CC} ${CFLAGS} -x c ${_f} -S -o /dev/null
done

if test -z ${CXX}
then
  if grep -Eoi '^(mingw|msys|cygwin)' /proc/version &>/dev/null
  then
    CXX=g++
  else
    CXX=x86_64-w64-mingw32-g++
  fi
fi

for _f in $(find ~+/"mcfgthread" -name "*.[hc]pp")
do
  echo "Checking includes:  ${_f}"
  ${CXX} ${CXXFLAGS} -x c++ ${_f} -S -o /dev/null
done
