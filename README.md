# SYCL tracing examples

This repository contains a set of example tracer libraries and demo programs
that exercise the SYCL tracing interface implemented in
[AdaptiveCpp](https://github.com/AdaptiveCpp/AdaptiveCpp) (tracing branch).

## Tracer libraries

- `tracer_lib` (`libtracer_lib.so`): A simple time tracer for SYCL API calls.
  It outputs a file in the Trace Event Format (JSON), which can be opened and
  read with [perfetto.ui](https://ui.perfetto.dev).
- `mem_leak` (`libmem_leak.so`): A simple memory leak detector, capable of
  finding leaks for memory allocated with `sycl::malloc_*` functions. Based
  on Boost.Stacktrace. Only built when `USE_BOOST_STACKTRACE=ON`.
- `print_dag` (`libprint_dag.so`): A tracer that visualizes the task graph in
  perfetto.ui, again using the Trace Event Format. Only built when
  `USE_BOOST_STACKTRACE=ON`.
- `memory_tally` (`libmemory_tally.so`): Tallies the number of
  `sycl::malloc_*`/`sycl::free` calls made and the time spent in each.
- `empty_lib` (`libempty_lib.so`): A no-op tracer, to check that the call
  overhead itself is negligible.
- `checker_lib` (`libchecker_lib.so`): Like `empty_lib`, but logs every
  callback it receives (e.g. `"The start submission function was called"`),
  to confirm the tracing hooks are actually firing.

Tracers that output JSON use the [nlohmann/json](https://github.com/nlohmann/json)
library, fetched automatically at configure time via CMake's `FetchContent`
- no manual installation needed.

## Demo programs

- `first_trial`, `interesting_dag`, `interesting_dag_benchmark`, `benchmarks`:
  standalone SYCL programs used to exercise the tracer libraries above. Run
  one with `SYCL_TOOL_LIBRARIES=/path/to/libtracer_lib.so ./first_trial` (or
  any other tracer library) to trace it.

## Build instructions

Make sure an AdaptiveCpp build with tracing support is on your `PATH`. You
can use `load_path.sh` for this - set `ADAPTIVECPP_TRACING_INSTALL_DIR` to
your AdaptiveCpp (tracing branch) install prefix, then `source load_path.sh`.

Building is then:

```sh
cmake $PATH_TO_SRC_DIR [-DUSE_BOOST_STACKTRACE=ON] && make
```

`-DUSE_BOOST_STACKTRACE=ON` is only needed if you want to build `mem_leak`
and `print_dag`, which require Boost.Stacktrace (with the addr2line backend)
to be available on your system.
