perf record --call-graph dwarf -F 99 ./result/tester
perf script > out.perf

perl FlameGraph/stackcollapse-perf.pl out.perf > out.folded
perl FlameGraph/flamegraph.pl out.folded > kernel.svg

rm out.perf
rm out.folded
rm perf.data
