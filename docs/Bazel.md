
# Bazel

Bazel is a relatively new build system open sourced by google. The main difference
with traditional Makefiles is that it checks to make sure that dependences are not
lost and the builds are reproducible and hermetic. This document explains how
to use Bazel in the LGraph project.

Build targets are referred to using the syntax `//<relative path to BUILD file>:<executable>`, where
`//` is the path of the root livehd directory.

To build the LiveHD shell and supporting libraries, the target would be `//main:all`.
To build every target in LiveHD (helpful for checking if changes cause compilation failures), the target would be `//:...`.  For more details on target syntax, see [this](https://docs.bazel.build/versions/master/guide.html#target-patterns) page.

## List bazel targets starting from top directory
```
$ bazel query <target>
```
## List bazel targets starting from any directory
```
$ bazel query <target>
```
## List files needed for a given target
```
$ bazel query "deps(<target>)"
```
## List all the passes that use core (those should be listed at main/BUILD deps)
```
$ bazel query "rdeps(//pass/..., //core:all)" | grep pass_
```
## Release vs fastbuild (default) vs debug
 - Fast build: no optimization, minimal debugging information (no local variable information), assertions turned on (default)
```    
$ bazel build <target>
```
 - Debug build: some optimization, full debugging information, assertions turned on
```
$ bazel build -c dbg <target>
```

or use address sanitizer to detect memory leaks
```
$ bazel build -c dbg --config asan //...
```

 - Release build: most optimization, no debug symbols, assertions turned off
```
$ bazel build -c opt <target>
```
 - Benchmarking build: aggressive optimization for the current architecture (binary may not be portable!)
```
$ bazel build --config=bench <target>
```



## Clear out cache (not needed in most cases)

This command is useful for benchmarking build time, and when system parameters change (the compiler gets upgraded, for example)
```
$ bazel clean --expunge
```

## See the commands executed
```
$ bazel build -s //main:all
```

## To run SHORT tests

The SHORT tests should have less than 15 minutes execution time for each
individual test. Those tests are used to gate commits to the main repo,
and to generate coverage reports.
```
$ bazel test <target>
```
To debug errors in the testing environment, you may want to keep the sandbox
files to check what may be going wrong. Use:
```
$ bazel test <target> --sandbox_debug --verbose_failures --keep_state_after_build
```
## To run FIXME tests

Many times, we have new tests that make the regression fail. We use "fixme" if
the test is a new one and LGraph is still not patched. We want the test in the system,
but we do not want to make fail the regressions.

Those tests are marked with tags "fixme" in the BUILD. E.g:

    sh_test(
        name = "my_test.sh",
        tags = ["fixme"],  # This is a fixme test. It fails, but we should fix it
        srcs = ["tests/pyrope_test.sh"],

To run all the fixme tests
```
$ bazel test --test_tag_filters "fixme" <target>
```
To list all the fixme tests (the goal is to have zero)
```
$ bazel query 'attr(tags, fixme, tests(<target>))'
```
## To run LONG tests

In addition to the short tests, there are sets of long tests that are run frequently
but not before every push to main line. The reason is that those are multi-hour
tests.
```
$ bazel test --test_tag_filters "long1" <target>
```
There are up to 8 long tests categories (long1, long2, long3...). Each of those
tests groups should last less than 4 hours when running in a dual core machine
(travis or azure).

To list the tests under each tag. E.g., to list all the tests with long1 tag.
```
$ bazel query 'attr(tags, long1, tests(<target>))'
```
## Debugging with bazel

First run the tests to see the failing one. Then run with debug options
the failing test. E.g:
```
$ bazel run -c dbg //eprp:all
```
Increase logging level if wanted
```
$ LGRAPH_LOG=info bazel run -c dbg //eprp:all
```
To run with gdb
```
$ bazel build -c dbg //eprp:eprp_test
$ gdb bazel-bin/eprp/eprp_test
(gdb) b Eprp::run
(gdb) r
```
(lldb is also supported.)

## Debugging with Yosys verilog code generation

create local yosys binary 
```
$ git clone https://github.com/YosysHQ/yosys.git
$ cd yosys
$ make -j<number of CPU cores * 2>
```
launch gdb with this new installed yosys binary and lgraph-yosys plugin
```
$ cd ~/your/work_path/lgraph
$ gdb --args ~/yosys/yosys -m ./bazel-bin/inou/yosys/liblgraph_yosys.so
(gdb) r
yosys> lg2yosys -name your_lgraph_name
```
set break point at the line of assertion failure

## Code coverage for all the tests used
```
$ bazel build --collect_code_coverage ...
```
or better with runs
```
$ bazel test --collect_code_coverage --test_output=all --nocache_test_results ...
```
To run locally, and push the coverage reports
```
$ COVERAGE_RUN=coverage LGRAPH_SRC=`pwd` LGRAPH_BUILD_MODE=dbg LGRAPH_COMPILER=g++ RUN_TYPE=long ./scripts/build-and-run.sh
```

## To download the dependent packages and apply patches (abc, bm,...)

No need to run this, as the bazel build will do it.
```
$ bazel fetch ...
```

The downloaded code will be at bazel-lgraph/external/

## To create a fully static binary

In the cc_binary of the relevant BUILD file, add `linkopts = ['-static']`

Notice that the lgshell still needs the directory inside
`bazel-bin/main/lgshell.runfiles when using inou.yosys.\*`
