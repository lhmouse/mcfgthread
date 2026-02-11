set terminal png
set output 'benchmark_result.png'
set xlabel 'Number of threads'
set ylabel 'Nanoseconds per iteration (lower is better)'
set logscale y
set format y '%.0f'
set grid ytics

plot "benchmark_result.txt" index 1 using 0:3:xticlabels(2) with linespoints title 'CRITICAL\_SECTION', \
     "benchmark_result.txt" index 0 using 0:3:xticlabels(2) with linespoints title 'SRWLOCK', \
     "benchmark_result.txt" index 2 using 0:3:xticlabels(2) with linespoints title 'WINPTHREAD', \
     "benchmark_result.txt" index 3 using 0:3:xticlabels(2) with linespoints title 'MCFGTHREAD'
