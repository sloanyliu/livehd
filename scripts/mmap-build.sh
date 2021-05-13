#!/bin/bash
# This file is distributed under the BSD 3-Clause License. See LICENSE for details.

CXX=clang++ CC=clang bazel build -c dbg //mmap_lib:all

exit 2

