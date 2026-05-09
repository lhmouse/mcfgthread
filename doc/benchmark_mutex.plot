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

set terminal png
set output 'benchmark_result_OS_VER_CPU_MODEL.png'
set xlabel 'Number of threads'
set ylabel 'Nanoseconds per iteration (lower is better)'
set logscale y
set format y '%.0f'
set grid ytics

plot "benchmark_result.txt" index 1 using 0:3:xticlabels(2) with linespoints title 'CRITICAL\_SECTION', \
     "benchmark_result.txt" index 0 using 0:3:xticlabels(2) with linespoints title 'SRWLOCK', \
     "benchmark_result.txt" index 2 using 0:3:xticlabels(2) with linespoints title 'WINPTHREAD', \
     "benchmark_result.txt" index 3 using 0:3:xticlabels(2) with linespoints title 'MCFGTHREAD'
