#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.

bazel build -c opt //mmap_lib:all
#./bazel-bin/mmap_lib/bench_set_use std
echo -e "\nvset bench"
time ./bazel-bin/mmap_lib/bench_set_use vset > out.txt
echo -e "\nmmap bench"
time ./bazel-bin/mmap_lib/bench_set_use mmap > out.txt
rm out.txt

exit 2

