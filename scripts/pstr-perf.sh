#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.

CXX=clang++ CC=clang bazel build -c opt //mmap_lib:all
./bazel-bin/mmap_lib/bench_pstr_use

# Make sure only DATACOLLECT is defined in bench_pstr_use first 
#./bazel-bin/mmap_lib/bench_pstr_use 50 20

exit 2

