sudo perf record -F 99 -a -g ./build/tester
sudo perf script > out.perf

./build/FlameGraph/stackcollapse-perf.pl out.perf > out.folded
./build/FlameGraph/flamegraph.pl out.folded > kernel.svg


rm out.perf
rm out.folded
rm perf.data
