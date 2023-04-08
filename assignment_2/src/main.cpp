#include <iostream>
#include <chrono>
#include <vector>
#include "parallel_library.hpp"
#include "benchmark/benchmark.h"

// sequential reduction
static void benchmark_sequential_reduce(benchmark::State& s) {
  size_t counts = s.range(0);
  
  std::vector<int> vec(counts);
  for (auto& v : vec) {
    v = ::rand()%10;
  }
  
  // Timing loop
  for (auto _ : s) {
    int r = seq_reduce(vec, 100);
    //r = std::accumulate(vec.begin(), vec.end(), 100, [](int a, int b){ return a+b; });
    //std::cout << r << '\n';
  }
  //std::cout << "r = " << r << '\n';
}

BENCHMARK(benchmark_sequential_reduce)
  ->RangeMultiplier(10)
  ->Range(10, 100000000)
  ->UseRealTime()
  ->Unit(benchmark::kMillisecond);



// parallel reduction with static scheduling
static void benchmark_parallel_reduce_static(benchmark::State& s) {
  size_t counts = s.range(0);

  std::vector<int> vec(counts);
  for (auto& v : vec) {
    v = ::rand()%10;
  }

  Threadpool threadpool(s.range(1));
  size_t chunk_size = s.range(2);
 
  // Timing loop
  for (auto _ : s) {
    int r = par_reduce_static(vec, 100, chunk_size, threadpool);
  }

  if (s.thread_index() == 0) {
    threadpool.shutdown();
  }
}

BENCHMARK(benchmark_parallel_reduce_static)
  ->Args({10,1,2})
  ->Args({100,1,2})
  ->Args({1000,1,2})
  ->Args({10000,1,2})
  ->Args({100000,1,2})
  ->Args({1000000,1,2})
  ->Args({10000000,1,2})
  ->Args({100000000,1,2})
  ->Args({10,2,2})
  ->Args({100,2,2})
  ->Args({1000,2,2})
  ->Args({10000,2,2})
  ->Args({100000,2,2})
  ->Args({1000000,2,2})
  ->Args({10000000,2,2})
  ->Args({100000000,2,2})
  ->Args({10,4,2})
  ->Args({100,4,2})
  ->Args({1000,4,2})
  ->Args({10000,4,2})
  ->Args({100000,4,2})
  ->Args({1000000,4,2})
  ->Args({10000000,4,2})
  ->Args({100000000,4,2})
  ->Args({10,8,2})
  ->Args({100,8,2})
  ->Args({1000,8,2})
  ->Args({10000,8,2})
  ->Args({100000,8,2})
  ->Args({1000000,8,2})
  ->Args({10000000,8,2})
  ->Args({100000000,8,2})
  ->Args({10,1,64})
  ->Args({100,1,64})
  ->Args({1000,1,64})
  ->Args({10000,1,64})
  ->Args({100000,1,64})
  ->Args({1000000,1,64})
  ->Args({10000000,1,64})
  ->Args({100000000,1,64})
  ->Args({10,2,64})
  ->Args({100,2,64})
  ->Args({1000,2,64})
  ->Args({10000,2,64})
  ->Args({100000,2,64})
  ->Args({1000000,2,64})
  ->Args({10000000,2,64})
  ->Args({100000000,2,64})
  ->Args({10,4,64})
  ->Args({100,4,64})
  ->Args({1000,4,64})
  ->Args({10000,4,64})
  ->Args({100000,4,64})
  ->Args({1000000,4,64})
  ->Args({10000000,4,64})
  ->Args({100000000,4,64})
  ->Args({10,8,64})
  ->Args({100,8,64})
  ->Args({1000,8,64})
  ->Args({10000,8,64})
  ->Args({100000,8,64})
  ->Args({1000000,8,64})
  ->Args({10000000,8,64})
  ->Args({100000000,8,64})
  ->Args({10,1,1024})
  ->Args({100,1,1024})
  ->Args({1000,1,1024})
  ->Args({10000,1,1024})
  ->Args({100000,1,1024})
  ->Args({1000000,1,1024})
  ->Args({10000000,1,1024})
  ->Args({100000000,1,1024})
  ->Args({10,2,1024})
  ->Args({100,2,1024})
  ->Args({1000,2,1024})
  ->Args({10000,2,1024})
  ->Args({100000,2,1024})
  ->Args({1000000,2,1024})
  ->Args({10000000,2,1024})
  ->Args({100000000,2,1024})
  ->Args({10,4,1024})
  ->Args({100,4,1024})
  ->Args({1000,4,1024})
  ->Args({10000,4,1024})
  ->Args({100000,4,1024})
  ->Args({1000000,4,1024})
  ->Args({10000000,4,1024})
  ->Args({100000000,4,1024})
  ->Args({10,8,1024})
  ->Args({100,8,1024})
  ->Args({1000,8,1024})
  ->Args({10000,8,1024})
  ->Args({100000,8,1024})
  ->Args({1000000,8,1024})
  ->Args({10000000,8,1024})
  ->Args({100000000,8,1024})
  ->UseRealTime()
  ->Unit(benchmark::kMillisecond);


// parallel reduction with guided scheduling
static void benchmark_parallel_reduce_guided(benchmark::State& s) {
  size_t counts = s.range(0);

  std::vector<int> vec(counts);
  for (auto& v : vec) {
    v = ::rand()%10;
  }

  Threadpool threadpool(s.range(1));
  size_t chunk_size = s.range(2);
 
  // Timing loop
  for (auto _ : s) {
    int r = par_reduce_static(vec, 100, chunk_size, threadpool);
  }

  if (s.thread_index() == 0) {
    threadpool.shutdown();
  }
}

BENCHMARK(benchmark_parallel_reduce_guided)
  ->Args({10,1,2})
  ->Args({100,1,2})
  ->Args({1000,1,2})
  ->Args({10000,1,2})
  ->Args({100000,1,2})
  ->Args({1000000,1,2})
  ->Args({10000000,1,2})
  ->Args({100000000,1,2})
  ->Args({10,2,2})
  ->Args({100,2,2})
  ->Args({1000,2,2})
  ->Args({10000,2,2})
  ->Args({100000,2,2})
  ->Args({1000000,2,2})
  ->Args({10000000,2,2})
  ->Args({100000000,2,2})
  ->Args({10,4,2})
  ->Args({100,4,2})
  ->Args({1000,4,2})
  ->Args({10000,4,2})
  ->Args({100000,4,2})
  ->Args({1000000,4,2})
  ->Args({10000000,4,2})
  ->Args({100000000,4,2})
  ->Args({10,8,2})
  ->Args({100,8,2})
  ->Args({1000,8,2})
  ->Args({10000,8,2})
  ->Args({100000,8,2})
  ->Args({1000000,8,2})
  ->Args({10000000,8,2})
  ->Args({100000000,8,2})
  ->Args({10,1,64})
  ->Args({100,1,64})
  ->Args({1000,1,64})
  ->Args({10000,1,64})
  ->Args({100000,1,64})
  ->Args({1000000,1,64})
  ->Args({10000000,1,64})
  ->Args({100000000,1,64})
  ->Args({10,2,64})
  ->Args({100,2,64})
  ->Args({1000,2,64})
  ->Args({10000,2,64})
  ->Args({100000,2,64})
  ->Args({1000000,2,64})
  ->Args({10000000,2,64})
  ->Args({100000000,2,64})
  ->Args({10,4,64})
  ->Args({100,4,64})
  ->Args({1000,4,64})
  ->Args({10000,4,64})
  ->Args({100000,4,64})
  ->Args({1000000,4,64})
  ->Args({10000000,4,64})
  ->Args({100000000,4,64})
  ->Args({10,8,64})
  ->Args({100,8,64})
  ->Args({1000,8,64})
  ->Args({10000,8,64})
  ->Args({100000,8,64})
  ->Args({1000000,8,64})
  ->Args({10000000,8,64})
  ->Args({100000000,8,64})
  ->Args({10,1,1024})
  ->Args({100,1,1024})
  ->Args({1000,1,1024})
  ->Args({10000,1,1024})
  ->Args({100000,1,1024})
  ->Args({1000000,1,1024})
  ->Args({10000000,1,1024})
  ->Args({100000000,1,1024})
  ->Args({10,2,1024})
  ->Args({100,2,1024})
  ->Args({1000,2,1024})
  ->Args({10000,2,1024})
  ->Args({100000,2,1024})
  ->Args({1000000,2,1024})
  ->Args({10000000,2,1024})
  ->Args({100000000,2,1024})
  ->Args({10,4,1024})
  ->Args({100,4,1024})
  ->Args({1000,4,1024})
  ->Args({10000,4,1024})
  ->Args({100000,4,1024})
  ->Args({1000000,4,1024})
  ->Args({10000000,4,1024})
  ->Args({100000000,4,1024})
  ->Args({10,8,1024})
  ->Args({100,8,1024})
  ->Args({1000,8,1024})
  ->Args({10000,8,1024})
  ->Args({100000,8,1024})
  ->Args({1000000,8,1024})
  ->Args({10000000,8,1024})
  ->Args({100000000,8,1024})
  ->UseRealTime()
  ->Unit(benchmark::kMillisecond);



BENCHMARK_MAIN();





/*
int main(int argc, char* argv[]) {

  // usage: ./a.out thread counts model
  if(argc != 3) {
    std::cerr << "usage: ./a.out thread counts model\n";
    std::exit(EXIT_FAILURE);
  }

  size_t thread = std::atoi(argv[1]);
  size_t counts = std::atoi(argv[2]);
  std::string model = argv[3];
  size_t rounds = 5;
  int initial = 0;

  // create a thread pool of thread workers
  Threadpool threadpool(thread);

  std::vector<int> vec(counts);
  for(auto& v : vec) {
    v = (::rand())%10;
  }

  std::vector<int> gold(rounds, 0);
  for (size_t r = 0; r < rounds; ++r) {
    gold[r] = seq_reduce(vec, initial+r);
  }
  
  std::cout << std::setw(12) << "Array Size"
            << std::setw(12) << "Runtime (ms)"
            << '\n';

  size_t counts = 1 << 20;
  for (size_t c = 16; c <= counts; c*=2) {
    double elapsed{0.0};    

    for (size_t r = 0; r < rounds; ++r) {
      if (model == "seq") {
        elapsed += seq_reduce(vec, initial+r);
      }

      else if (model == "static") {
        elapsed += par_reduce_static(vec, initial+r, threadpool);
      }

      else if (model == "guided") {
        elapsed += par_reduce_guided(vec, initial+r, threadpool);
      }
    }

    std::cout << std::setw(12) << c
              << std::setw(12) << elapsed/1000/rounds
              << '\n'; 
  }


  // run reduction sequentially
  std::vector<int> seq_results(rounds, 0);
  std::cout << "Running sequential reduction takes ";
  auto beg = std::chrono::steady_clock::now();
  for (size_t r = 0; r < rounds; ++r) {
    seq_results[r] = seq_reduce(vec, initial+r);
  }
  auto end = std::chrono::steady_clock::now();
  std::cout << (std::chrono::duration_cast<std::chrono::microseconds>(end-beg).count())/(double)rounds
            << " us\n"; 
            
  
  // run reduction parallelly with static scheduling
  std::vector<int> par_results_static(rounds, 0);
  std::cout << "Running parallel reduction with static scheduling takes ";
  beg = std::chrono::steady_clock::now();
  for (size_t r = 0; r < rounds; ++r) {
    par_results_static[r] = par_reduce_static(vec, initial+r, threadpool);
  }
  end = std::chrono::steady_clock::now();
  std::cout << (std::chrono::duration_cast<std::chrono::microseconds>(end-beg).count())/(double)rounds
            << " us\n"; 

  // run reduction parallelly with guided scheduling
  std::vector<int> par_results_guided(rounds, 0);
  std::cout << "Running parallel reduction with guided scheduling takes ";
  beg = std::chrono::steady_clock::now();
  for (size_t r = 0; r < rounds; ++r) {
    par_results_guided[r] = par_reduce_guided(vec, initial+r, threadpool);
  }
  end = std::chrono::steady_clock::now();
  std::cout << (std::chrono::duration_cast<std::chrono::microseconds>(end-beg).count())/(double)rounds
            << " us\n"; 

  // shut down the threadpool
  threadpool.shutdown();

  // verify the reuslts
  for (size_t i = 0; i < seq_results.size(); ++i) {
    if (seq_results[i] != par_results_static[i]) {
      std::cerr << "Wrong par_results_static\n";
      break;
    }
  }

  for (size_t i = 0; i < seq_results.size(); ++i) {
    if (seq_results[i] != par_results_guided[i]) {
      std::cout << "seq[" << i << "]=" << seq_results[i] << ", par[" << i << "]=" << par_results_guided[i] << '\n';
      std::cerr << "Wrong par_results_guided\n";
      //break;
    }
  }


  return 0;
}
*/
