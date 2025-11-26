#include "hipSYCL/sycl/queue.hpp"
#include "hipSYCL/sycl/usm.hpp"
#include <chrono>
#include <iostream>
#include <sycl/sycl.hpp>

// void cool_tracer(Tracer_utils::tracer_type type,
//                  Tracer_utils::start_end state) {
//   std::cout << "Hello World!" << std::endl;
// }

int main() {

  sycl::gpu_selector selector;
  sycl::queue q{selector,
                sycl::property_list{sycl::property::queue::in_order{}}};

  sycl::host_selector selector2;
  sycl::queue q2{selector2,
                 sycl::property_list{sycl::property::queue::in_order{}}};

  auto dev = q.get_device();
  auto dev2 = q2.get_device();

  std::cout << "Running on device: " << dev.get_info<sycl::info::device::name>()
            << std::endl;
  std::cout << "Running on device with q2: "
            << dev2.get_info<sycl::info::device::name>() << std::endl;

  // Tracer_utils::initialize_tracer(cool_tracer);

  constexpr std::size_t num = 1;

  std::array<int, num> numbers;
  for (int i = 1; i <= num; i++)
    numbers[i - 1] = i;

  int *numbers_device = sycl::malloc_shared<int>(num, q);
  // q.wait();
  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++)
      q.memcpy(numbers_device, numbers.data(), sizeof(int) * num);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start);
    std::cout << "Bencmark for memcpy: " << duration.count() << std::endl;
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++)
      q.memset(numbers_device, 0, sizeof(int) * num);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start);
    std::cout << "Benchmark for memset: " << duration.count() << std::endl;
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++)
      q.wait();
    //  q.wait();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start);
    std::cout << "Benchmark for wait: " << duration.count() << std::endl;
  }

  sycl::event f{};
  {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 10000; i++) {
      auto e = q.fill(numbers_device, 42, num);
      // e.wait();
      f = q2.copy(numbers_device, numbers.data(), num, e);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start);
    std::cout << "Benchmark for fill and copy: " << duration.count()
              << std::endl;
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    // q.wait();
    for (int i = 0; i < 10000; i++) {
      auto g = q.submit([&](sycl::handler &h) {
        h.depends_on(f);
        h.single_task([=]() {
          //  int i = 0;
          //  for (int j = 0; j < num; j++) {
          //    i++;
          //  }
        });
      });
      f = g;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start);
    std::cout << "Benchmark for single task plus depends on is: "
              << duration.count() << std::endl;
  }

  f.wait();

  // q.wait();
  {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 10000; i++)
      q.parallel_for(sycl::range<1>(1),
                     [=](sycl::id<1> I) { const int i = 0; });

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start);
    std::cout << "Benchmark for parallel_for is: " << duration.count()
              << std::endl;
  }

  q.wait();

  std::cout << "Hello World!" << std::endl;
}
