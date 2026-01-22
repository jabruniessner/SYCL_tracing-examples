#include "hipSYCL/sycl/device_selector.hpp"
#include "hipSYCL/sycl/queue.hpp"
#include "hipSYCL/sycl/usm.hpp"
#include <chrono>
#include <iostream>
#include <sycl/sycl.hpp>

int main() {

  sycl::cpu_selector cpu_selector;

  sycl::queue q{cpu_selector};
  sycl::queue q2{cpu_selector,
                 sycl::property_list{sycl::property::queue::in_order{}}};

  std::array<int, 100> numbers;
  for (int i = 1; i <= 100; i++)
    numbers[i - 1] = i;

  int *numbers_device = sycl::malloc_device<int>(100, q);
  int *numbers_device2 = sycl::malloc_device<int>(100, q2);
  int *numbers_host = sycl::malloc_host<int>(100, q);
  // q.wait();

  q.memcpy(numbers_device, numbers.data(), sizeof(int) * 100);
  // q.memset(numbers_device, 0, sizeof(int) * 100);
  q.wait();
  q2.wait();

  auto start_time = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10000; i++) {
    auto e = q.fill(numbers_device, 42, 100);
    auto f = q2.fill(numbers_device2, 21, 100);

    auto g = q2.copy(numbers_device2, numbers_host, 100, e);
    auto m = q.copy(numbers_device, numbers.data(), 100, f);

    q.submit([&](sycl::handler &h) {
      h.depends_on(g);
      h.single_task([=]() {
        int i = 0;
        for (int j = 0; j < 100; j++) {
          i++;
        }
      });
    });

    q2.submit([&](sycl::handler &h) {
      h.depends_on(m);
      h.single_task([=]() {
        int i = 0;
        for (int j = 0; j < 100; j++) {
          i++;
        }
      });
    });

    q.wait();
    q2.wait();
  }

  q.wait();
  q2.wait();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto time_span = std::chrono::duration<double>(end_time - start_time);

  std::cout << "The required time was: " << time_span.count() << std::endl;

  std::cout << "Hello World!" << std::endl;
}
