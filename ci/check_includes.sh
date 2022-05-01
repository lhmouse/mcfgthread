#!/bin/bash -e

# setup
export CC=${CC:-"gcc"}
export CXX=${CXX:-"g++"}
export CPPFLAGS="-D_WIN32_WINNT=0x0601 -Wall -Wextra"

for _file in $(find -L "src" -name "*.h")
do
  echo "Checking header as C89:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c -std=c89 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done

for _file in $(find -L "src" -name "*.h")
do
  echo "Checking header as C++98:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c++ -std=c++98 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done

for _file in $(find -L "src" -name "*.h")
do
  echo "Checking header as C++11:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c++ -std=c++11 -Wpedantic -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done

for _file in $(find -L "src" -name "*.c")
do
  echo "Checking source as C99:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c -std=c99 -Wpedantic -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done
