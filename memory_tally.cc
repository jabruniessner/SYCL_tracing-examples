#include "hipSYCL/sycl/tracer_utils.hpp"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <chrono>

template<typename T>
struct TD;

using Time_Point = decltype(std::chrono::high_resolution_clock::now());
using Time_Span = std::chrono::duration<double>;

#ifdef __cplusplus
extern "C" {
#endif

struct state_t {
  Time_Point start_time, end_time;

  std::size_t alloc_count = 0;
  std::size_t free_count = 0;
  
  Time_Span alloc_time{};
  Time_Span free_time{};
};

auto malloc_start = [](void *usr_state) {
  ((state_t *) usr_state)-> start_time = std::chrono::high_resolution_clock::now();
  
};

auto malloc_end = [](void *usr_state, void *ptr) {
  Time_Point end_time = std::chrono::high_resolution_clock::now();
  Time_Point start_time = ((state_t*) usr_state)->start_time;
  ((state_t *) usr_state)-> alloc_time += end_time-start_time;
  ((state_t *) usr_state)-> alloc_count++;
};

auto free_start = [](void *usr_state) {
  ((state_t*) usr_state) -> start_time = std::chrono::high_resolution_clock::now();
};
auto free_end = [](void *usr_state, void *ptr) {
  Time_Point end_time = std::chrono::high_resolution_clock::now();
  Time_Point start_time = ((state_t*) usr_state)->start_time;
  ((state_t *) usr_state)-> free_time += end_time-start_time;
  ((state_t *) usr_state)-> free_count++;

};

void finalize(void *usr_state) {
  std::cout << "The number of allocations made is: "<< ((state_t*) usr_state)->alloc_count<<
    " The time spent is was "<< ((state_t*) usr_state)->alloc_time.count()<<" seconds";

  std::cout << "The number of frees done is: "<< ((state_t*) usr_state)->free_count<<
    " The time spent is was "<< ((state_t*) usr_state)->alloc_time.count()<<" seconds";


}

void init_register() {
  state_t *state = new state_t;

  init_states(state);
  init_malloc_device_start(malloc_start);
  init_malloc_device_end(malloc_end);
  init_malloc_shared_start(malloc_start);
  init_malloc_shared_end(malloc_end);
  init_malloc_host_start(malloc_start);
  init_malloc_host_end(malloc_end);
  init_free_start(free_start);
  init_free_end(free_end);

  init_finalize(finalize);
}

#ifdef __cplusplus
}
#endif
