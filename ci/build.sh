#!/bin/bash -e

# setup
export CC=${CC:-"gcc"}
export CCFLAGS='-O2 -g0'

# build
${CC} --version
mkdir -p m4
autoreconf -ifv

cd $(mktemp -d)
trap 'rm -rf ~+ || true' EXIT
~-/configure --disable-silent-rules --enable-debug-checks ${CONFIGURE_OPTIONS}

# test
if ! make -j$(nproc) distcheck
then
  cat ./test-suite.log
  exit 1
fi
