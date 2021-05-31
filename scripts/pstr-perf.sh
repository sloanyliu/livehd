#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.

CXX=clang++ CC=clang bazel build -c opt //mmap_lib:all
./bazel-bin/mmap_lib/bench_pstr_use

#for i in {1..10}
#do
#  echo "Test Number $i" >>strout
#  for k in {2..50}
#  do
#    ./bazel-bin/mmap_lib/bench_pstr_use $k >>strout
#  done
#done


exit 2

