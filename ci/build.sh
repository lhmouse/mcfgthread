#!/bin/bash -e

# setup
export CC=${CC:-"gcc"}
export CCFLAGS='-O2 -g0'

_fail=1

while test $# -gt 0
do
  case $1 in
    --disable-make-check )
      _fail=0
      shift 1
      ;;

    * )
      echo "WARNING: unknown option -- '$1'" >&2
      shift 1
      ;;
  esac
done

# build
${CC} --version
mkdir -p m4
autoreconf -ifv

cd $(mktemp -d)
trap 'rm -rf ~+ || true' EXIT
~-/configure --disable-silent-rules --enable-debug-checks --disable-static

# test
if ! make -j$(nproc) distcheck
then
  cat ./test-suite.log
  exit ${_fail}
fi
