#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.

CXX=clang++ CC=clang bazel build -c opt //mmap_lib:all
./bazel-bin/mmap_lib/mmap_str_test

exit 2

