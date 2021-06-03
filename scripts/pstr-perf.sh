#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.
CXX=clang++ CC=clang bazel build -c opt //mmap_lib:all
#./bazel-bin/mmap_lib/bench_pstr_use

# Make sure DATACOLLECT_XX is set to 1 in bench_pstr_use first 
echo > lbench.trace
./bazel-bin/mmap_lib/bench_pstr_use 50 20
cp lbench.trace pstr_perf_analysis/pstr-data2.trace
python pstr_perf_analysis/data-avg.py

exit 2

