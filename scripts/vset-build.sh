#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.

bazel build -c dbg //mmap_lib:all
#./bazel-bin/mmap_lib/bench_set_use vset
echo -e "\nmy vset bench"
time ./bazel-bin/mmap_lib/bench_vset_use vset
echo -e "\nmy mmap bench"
time ./bazel-bin/mmap_lib/bench_vset_use mmap

exit 2

