#!/bin/bash -ex

function do_bench_nth()
{
  rm -f benchmark_mutex.exe
  x86_64-w64-mingw32-gcc -DUSE_$1 -DNTHRD=$2 -DNITER=$3   \
      benchmark_mutex.c -lpthread -lmcfgthread -o benchmark_mutex.exe  \
      -O2 -fno-inline
  printf '%s\t%s\t%s\n' $1 $2 $(./benchmark_mutex.exe) >> benchmark_result.txt
}

function do_bench()
{
  do_bench_nth $1    1 20000000
  do_bench_nth $1    2 10000000
  do_bench_nth $1    4  5000000
  do_bench_nth $1    8  2000000
  do_bench_nth $1   12  1000000
  do_bench_nth $1   16   800000
  do_bench_nth $1   24   300000
  do_bench_nth $1   32   300000
  do_bench_nth $1  100   100000
  do_bench_nth $1  200    60000
  printf '\n\n' >> benchmark_result.txt
}

rm -f benchmark_result.txt
do_bench SRWLOCK
do_bench CRITICAL_SECTION
do_bench WINPTHREAD
do_bench MCFGTHREAD
