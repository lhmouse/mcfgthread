#!/bin/bash -e

# setup
export CC=${CC:-"gcc"}
export CPPFLAGS="-D_WIN32_WINNT=0x0601 -Wall -Wextra "${CPPFLAGS}

for _file in $(find -L "src" -name "*.h")
do
  echo "Checking header as C89:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c-header -std=c89 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done

for _file in $(find -L "src" -name "*.h")
do
  echo "Checking header as C++98:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c++-header -std=c++98 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done

for _file in $(find -L "src" -name "*.h")
do
  echo "Checking header as C++11:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c++-header -std=c++11 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done

for _file in $(find -L "src" -name "*.h")
do
  echo "Checking header as C++14:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c++-header -std=c++14 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done

for _file in $(find -L "src" -name "*.c")
do
  echo "Checking source as C99:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c -std=c99 -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done
