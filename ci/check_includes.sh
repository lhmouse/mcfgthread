#!/bin/bash -e

# setup
export CC=${CC:-"gcc"}
export CPPFLAGS="-D_FILE_OFFSET_BITS=64 -D_POSIX_C_SOURCE=200809 -D_GNU_SOURCE -D_WIN32_WINNT=0x0600"
export CFLAGS='-std=c99'

for _file in $(find -L "src" -name "*.[hc]")
do
  echo "Checking \`#include\` directives:  ${_cmd}  \"${_file}\""
  ${CC} ${CPPFLAGS} ${CFLAGS} -x c -fsyntax-only -DHAVE_CONFIG_H -I. ${_file}
done
