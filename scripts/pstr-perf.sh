#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.

CXX=clang++ CC=clang bazel build -c opt //mmap_lib:all
./bazel-bin/mmap_lib/bench_pstr_use

# Make sure DATACOLLECT_CMP is set to 1 in bench_pstr_use first 
#./bazel-bin/mmap_lib/bench_pstr_use 50 20



exit 2

