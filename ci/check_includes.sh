#!/bin/bash -e

# setup
export CC=${CC:-"gcc"}
export CXX=${CXX:-"g++"}
export CPPFLAGS="-D_FILE_OFFSET_BITS=64 -D_POSIX_C_SOURCE=200809 -D_GNU_SOURCE -D_WIN32_WINNT=0x0601"

for _file in $(find -L "src" -name "*.h")
do
  echo "Checking headers as C89:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c -std=c99 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done

for _file in $(find -L "src" -name "*.h")
do
  echo "Checking headers as C++:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c++ -std=c++98 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done

for _file in $(find -L "src" -name "*.c")
do
  echo "Checking sources as C99:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c -std=c99 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done
