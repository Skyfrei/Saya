sudo perf record -F 99 -a -g ./build/tester
sudo perf script > out.perf

perl ./build/FlameGraph/stackcollapse-perf.pl out.perf > out.folded
perl ./build/FlameGraph/flamegraph.pl out.folded > kernel.svg


rm out.perf
rm out.folded
rm perf.data
