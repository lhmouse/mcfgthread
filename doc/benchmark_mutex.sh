#!/bin/bash -e
## No Copyright
##
## Permission to use, copy, modify, and/or distribute this software for
## any purpose with or without fee is hereby granted.
##
## THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
## WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
## OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
## FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
## DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
## AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
## OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

if ! test -f ../doc/benchmark_mutex.c || ! test -f lib/libmcfgthread.a
then
  echo 'Please run this script in a build directory.'
  exit 1
fi

if test -z ${CC}
then
  if grep -Eoi '^(mingw|msys|cygwin)' /proc/version &>/dev/null
  then
    CC=gcc
  else
    CC=x86_64-w64-mingw32-gcc
  fi
fi

printf 'Using compiler:\n'
${CC} --version

rm -f benchmark_result.txt

function do_benchmark_nth()
{
  ${CC} -DUSE_$1 -DNTHRD=$2 -DNITER=$3 -I. -I.. -std=c99 -Wall -Wextra  \
      ../doc/benchmark_mutex.c -pthread lib/libmcfgthread.a -lntdll  \
      -static -O2 -fno-inline  \
      -o bin/benchmark_mutex_$1_$2_$3.exe

  bin/benchmark_mutex_$1_$2_$3.exe >> benchmark_result.txt
}

function do_benchmark()
{
  do_benchmark_nth $1    1 20000000
  do_benchmark_nth $1    2 10000000
  do_benchmark_nth $1    4  5000000
  do_benchmark_nth $1    8  2000000
  do_benchmark_nth $1   12  1000000
  do_benchmark_nth $1   16   800000
  do_benchmark_nth $1   24   300000
  do_benchmark_nth $1   32   300000
  do_benchmark_nth $1   48   200000
  do_benchmark_nth $1   64   150000
  do_benchmark_nth $1   96   100000
  do_benchmark_nth $1  128    70000

  printf '\n\n' >> benchmark_result.txt
}

do_benchmark SRWLOCK
do_benchmark CRITICAL_SECTION
do_benchmark WINPTHREAD
do_benchmark MCFGTHREAD

gnuplot ../doc/benchmark_mutex.plot
