#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.

bazel build -c dbg //mmap_lib:all
#./bazel-bin/mmap_lib/bench_set_use vset
./bazel-bin/mmap_lib/bench_vset_use

exit 2

