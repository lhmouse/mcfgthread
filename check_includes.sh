#!/bin/bash -e

# setup
export CC=${CC:-"gcc"}
export CFLAGS="-D_WIN32_WINNT=0x0601 -Ibuild_debug -std=c99"

export CXX=${CXX:-"g++"}
export CXXFLAGS="-D_WIN32_WINNT=0x0601 -Ibuild_debug -std=c++11"

# note: `sem` is not always available
_sem="parallel --will-cite --semaphore --halt soon,fail=1"

for _f in $(find "mcfgthread" -name "*.[hc]")
do
  echo "Checking includes:  ${_f}"
  ${_sem} -j+0 -- ${CC} ${CFLAGS} -x c ${_f} -S -o /dev/null
done
${_sem} --wait

for _f in $(find "mcfgthread" -name "*.[hc]pp")
do
  echo "Checking includes:  ${_f}"
  ${_sem} -j+0 -- ${CXX} ${CXXFLAGS} -x c++ ${_f} -S -o /dev/null
done
${_sem} --wait
