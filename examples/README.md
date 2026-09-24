# Example outputs

Reference outputs from running `first_trial` with each tool library attached
via `SYCL_TOOL_LIBRARIES`, captured from a working `ACPP_TRACING=ON` build.
Intended as a shape/format reference (and a starting point for a CI
pipeline), not something to diff byte-for-byte: timestamps, thread ids,
pointer addresses and absolute timings will differ on every run and every
machine. A CI check built on these should validate structure (e.g. valid
JSON, expected fields present, process exits cleanly) rather than exact
equality.

- `tracer_lib_output.json` - `libtracer_lib.so`. Trace Event Format JSON,
  openable in [perfetto.ui](https://ui.perfetto.dev).
- `mem_leak_output.txt` - `libmem_leak.so`. stdout; reports the leaked
  `malloc_shared` allocation `first_trial` never frees.
- `print_dag_output.json` - `libprint_dag.so`. Trace Event Format JSON
  describing the task graph, also openable in perfetto.ui.
- `memory_tally_output.txt` - `libmemory_tally.so`. stdout; allocation/free
  counts and timing.
- `empty_lib_output.txt` - `libempty_lib.so`. stdout; the no-op tracer
  produces no tracer-specific output of its own, only `first_trial`'s own
  prints and the initializer's one-time "Hello World" message.
  `libchecker_lib.so` is built from the same source (`empty_lib.cc`) and
  produces identical output.

Regenerate by building with `-DUSE_BOOST_STACKTRACE=ON` against an
`ACPP_TRACING=ON` AdaptiveCpp install, then running
`SYCL_TOOL_LIBRARIES=./lib<tool>.so ./first_trial`.
