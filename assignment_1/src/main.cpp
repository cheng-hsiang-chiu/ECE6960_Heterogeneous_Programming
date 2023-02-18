#include <iostream>
#include <vector>
#include "threadpool.hpp"
#include "matrix.hpp"
#include "benchmark/benchmark.h"

// sequential matrix multiplication
static void benchmark_matmul_sequential(benchmark::State& s) {
  size_t N, M, K;
  N = s.range(0);
  M = s.range(0);
  K = s.range(0);
  
  std::vector<int>A(N*K, 2);
  std::vector<int>B(M*K, 1);
  std::vector<int>C(N*M, 0);

  // Timing loop
  for (auto _ : s) {
    matmul_sequential(N, K, M, A, B, C);
    A.assign(N*K, 2);
    B.assign(N*K, 1);
    C.assign(N*K, 0);
  }
}

BENCHMARK(benchmark_matmul_sequential)
  ->RangeMultiplier(2)
  ->Range(16, 1024)
  ->UseRealTime()
  ->Unit(benchmark::kMillisecond);


// parallel matrix multiplication
// centralized queue
// N tasks
static void benchmark_matmul_parallel_centralized_N_tasks(benchmark::State& s) {
  size_t N, M, K;
  N = s.range(0);
  M = s.range(0);
  K = s.range(0);
  
  std::vector<int>A(N*K, 2);
  std::vector<int>B(M*K, 1);
  std::vector<int>C(N*M, 0);
  
  Threadpool threadpool(s.range(1));

  for (auto _ : s) {
    matmul_parallel_centralized_N_tasks(N,K,M,A,B,C,threadpool);
  }
  if (s.thread_index() == 0) {
    threadpool.shutdown();
    A.assign(N*K, 2);
    B.assign(N*K, 1);
    C.assign(N*K, 0);
  } 
}

BENCHMARK(benchmark_matmul_parallel_centralized_N_tasks)
  ->Args({16,1})
  ->Args({16,2})
  ->Args({16,4})
  ->Args({16,8})
  ->Args({32,1})
  ->Args({32,2})
  ->Args({32,4})
  ->Args({32,8})
  ->Args({64,1})
  ->Args({64,2})
  ->Args({64,4})
  ->Args({64,8})
  ->Args({128,1})
  ->Args({128,2})
  ->Args({128,4})
  ->Args({128,8})
  ->Args({256,1})
  ->Args({256,2})
  ->Args({256,4})
  ->Args({256,8})
  ->Args({512,1})
  ->Args({512,2})
  ->Args({512,4})
  ->Args({512,8})
  ->Args({1024,1})
  ->Args({1024,2})
  ->Args({1024,4})
  ->Args({1024,8})
  ->UseRealTime()
  ->Unit(benchmark::kMillisecond);


// parallel matrix multiplication
// centralized queue
// N/T tasks
static void benchmark_matmul_parallel_centralized_less_tasks(benchmark::State& s) {
  size_t N, M, K;
  N = s.range(0);
  M = s.range(0);
  K = s.range(0);
  
  std::vector<int>A(N*K, 2);
  std::vector<int>B(M*K, 1);
  std::vector<int>C(N*M, 0);
  
  Threadpool threadpool(s.range(1));

  for (auto _ : s) {
    matmul_parallel_centralized_less_tasks(N,K,M,A,B,C,threadpool,s.range(1));
  }
  if (s.thread_index() == 0) {
    threadpool.shutdown();
    A.assign(N*K, 2);
    B.assign(N*K, 1);
    C.assign(N*K, 0);
  } 
}

BENCHMARK(benchmark_matmul_parallel_centralized_less_tasks)
  ->Args({16,1})
  ->Args({16,2})
  ->Args({16,4})
  ->Args({16,8})
  ->Args({32,1})
  ->Args({32,2})
  ->Args({32,4})
  ->Args({32,8})
  ->Args({64,1})
  ->Args({64,2})
  ->Args({64,4})
  ->Args({64,8})
  ->Args({128,1})
  ->Args({128,2})
  ->Args({128,4})
  ->Args({128,8})
  ->Args({256,1})
  ->Args({256,2})
  ->Args({256,4})
  ->Args({256,8})
  ->Args({512,1})
  ->Args({512,2})
  ->Args({512,4})
  ->Args({512,8})
  ->Args({1024,1})
  ->Args({1024,2})
  ->Args({1024,4})
  ->Args({1024,8})
  ->UseRealTime()
  ->Unit(benchmark::kMillisecond);
BENCHMARK_MAIN();
