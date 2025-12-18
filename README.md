# SYCL tracing example

This repository consists of couple examples for tracer that can be used in combination with the SYCL tracing as implemented in acpp
The following examples are implemented.

- `libtracer_lib.so`: A simple time tracer for SYCL API calls. It output is an output file in the Trace Event Format json format, which can be opened and read with perfetto.ui
- `libmem_leak.so`: A simple memory leak detector, which is capable of finding simple memory leaks for memory allocated with `sycl::malloc_*`-functions. It is based on boosts stacktrace utility
- `checker_lib`: An empty tracer, to check that the call overhead is negligible
- `libprint_dag.so`: A tracer to visualize the task graph in perfetto.ui. The output format is again the Trace Event Format json format. 

The tracer that output their results into a json format also require the [nlohman/json](https://github.com/nlohmann/json) library. 


## Build instrcution

Building is quite simple. Just make sure the AdaptiveCpp version that has the tracing capabilities is in you path. Building should then be as easy as doing

`cmake $PATH_TO_SRC_DIR && make`

