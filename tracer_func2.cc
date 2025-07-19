#include "hipSYCL/sycl/tracer_utils.hpp"
#include <boost/stacktrace.hpp>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#ifdef __cplusplus
extern "C" {
#endif

using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

std::ofstream outfile("outfile.json");

struct state_t {
  time_point start_timer;
  std::ofstream outfile;

  std::unordered_map<void *, std::string> pointer_map;

  std::array<int, 13> num_starts{};
  std::array<int, 13> num_ends{};

  ~state_t() {
    std::cout << "Hello world from inside the state_t destructor" << std::endl;
    std::cout << "The size of the pointer map is: " << this->pointer_map.size() << std::endl;
  }
};

static state_t my_state;

void start(void *state_ptr, int num, std::string type) {
  state_t &state = *((state_t *)state_ptr);
  state.num_starts[num]++;

  auto start_time = std::chrono::high_resolution_clock::now();

  auto duration_micros =
      std::chrono::duration_cast<std::chrono::microseconds>(start_time - state.start_timer);

  std::string id_string;
  std::stringstream sid_string;
  sid_string << std::this_thread::get_id();

  nlohmann::json a{{"ph", "B"},    {"tid", id_string}, {"pid", "0"},
                   {"name", type}, {"cat", "cpu_op"},  {"ts", duration_micros.count()},
                   {"id", 0}};

  outfile << a.dump() << "," << std::endl;

  std::cout << "Hello World from the " << type << "_start function!" << std::endl;
}

void end(void *state_ptr, int num, std::string type) {
  state_t &state = *((state_t *)state_ptr);
  state.num_ends[num]++;

  auto start_time = std::chrono::high_resolution_clock::now();

  auto duration_micros =
      std::chrono::duration_cast<std::chrono::microseconds>(start_time - state.start_timer);

  std::string id_string;
  std::stringstream sid_string;
  sid_string << std::this_thread::get_id();

  nlohmann::json a{{"ph", "E"},    {"tid", id_string}, {"pid", "0"},
                   {"name", type}, {"cat", "cpu_op"},  {"ts", duration_micros.count()},
                   {"id", 0}};

  outfile << a.dump() << "," << std::endl;

  // std::cout << "Hello World from the " << type << "_end function!" << std::endl;
}

void finalize(void *usr_state) {
  outfile << "]}";
  outfile.close();

  std::cout << "Hello World from inside the finalize function! " << std::endl;

  std::cout << "The size of the pointer map is: " << ((state_t *)usr_state)->pointer_map.size()
            << std::endl;

  //  for (const auto &i : ((state_t *)usr_state)->pointer_map) {
  //    //    std::cout << "Pointer allocated at: " << i.second << "never deallocated" << std::endl;
  //    std::cout << "Hello World!" << std::endl;
  //}
};

auto submission_start = [](void *usr_state) { start(usr_state, 0, "submission"); };
auto submission_end = [](void *usr_state) { end(usr_state, 0, "submission"); };
auto single_task_start = [](void *usr_state) { start(usr_state, 1, "single_task"); };
auto single_task_end = [](void *usr_state) { end(usr_state, 1, "single_task"); };
auto parallel_for_start = [](void *usr_state) { start(usr_state, 2, "parallel_for"); };
auto parallel_for_end = [](void *usr_state) { end(usr_state, 2, "parallel_for"); };
auto parallel_for_work_group_start = [](void *usr_state) {
  start(usr_state, 3, "parallel_for_work_group");
};
auto parallel_for_work_group_end = [](void *usr_state) {
  end(usr_state, 3, "parallel_for_work_group");
};
auto wait_start = [](void *usr_state) { start(usr_state, 4, "wait"); };
auto wait_end = [](void *usr_state) { end(usr_state, 4, "wait"); };
auto memcpy_start = [](void *usr_state) { start(usr_state, 5, "memcpy"); };
auto memcpy_end = [](void *usr_state) { end(usr_state, 5, "memcpy"); };
auto memset_start = [](void *usr_state) { start(usr_state, 6, "memset"); };
auto memset_end = [](void *usr_state) { end(usr_state, 6, "memset"); };
auto fill_start = [](void *usr_state) { start(usr_state, 7, "fill"); };
auto fill_end = [](void *usr_state) { end(usr_state, 7, "fill"); };
auto copy_start = [](void *usr_state) { start(usr_state, 8, "copy"); };
auto copy_end = [](void *usr_state) { end(usr_state, 8, "copy"); };

auto malloc_device_start = [](void *usr_state) { start(usr_state, 10, "malloc_device"); };
auto malloc_device_end = [](void *usr_state, void *ptr) {
  // end(usr_state, 10, "malloc_device");
  boost::stacktrace::stacktrace st;
  std::stringstream ss;
  ss << st << std::endl;

  int i = 0;
  for (std::string line; std::getline(ss, line, '\n');)
    if (i++ == 5) {
      ((state_t *)usr_state)->pointer_map[ptr] = line.substr(4, line.size() - 4);
    }
};

auto free_start = [](void *usr_state) { // start(usr_state, 11, "sycl::free");
};

auto free_end = [](void *usr_state, void *ptr) {};

const auto i = std::atexit(
    []() { std::cout << "Hello World from inside the std::atexit function" << std::endl; });

void init_register() {

  outfile << "{ \"traceEvents\": [" << std::endl;

  auto tracer_start_time = std::chrono::high_resolution_clock::now();

  my_state.start_timer = tracer_start_time;

  state_t *state = &my_state;

  init_state(state);
  //  init_parallel_for_work_group_start(parallel_for_work_group_start);
  //  init_parallel_for_work_group_end(parallel_for_work_group_end);
  //  init_memset_start(memset_start);
  //  init_memset_end(memset_end);
  //  init_memcpy_start(memcpy_start);
  //  init_memcpy_end(memcpy_end);
  init_wait_start(wait_start);
  init_wait_end(wait_end);
  // init_single_task_start(single_task_start);
  //  init_single_task_end(single_task_end);
  // init_parallel_for_start(parallel_for_start);
  // init_parallel_for_end(parallel_for_end);
  // init_submit_start(submission_start);
  // init_submit_end(submission_end);
  //  init_copy_start(copy_start);
  //  init_copy_end(copy_end);
  //  init_fill_start(fill_start);
  //  init_fill_end(fill_end);
  init_malloc_device_start(malloc_device_start);
  init_malloc_device_end(malloc_device_end);
  // init_free_start(free_start);
  // init_free_end(free_end);
}

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */
