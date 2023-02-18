#pragma once

#include <iostream>
#include <vector>
#include <future>
#include <queue>
#include "threadpool.hpp"


// sequential matrix multiplication
void matmul_sequential(
  size_t N, size_t K, size_t M,
  const std::vector<int>& A,
  const std::vector<int>& B,
  std::vector<int>& C
) {

  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < M; j++) {
      //C[i*M + j] = 0;
      for (size_t k = 0; k < K; k++) {
        C[i*M + j] += A[i*K + k] * B[k*M + j];
      }
    }
  }
}


// parallel matrix multiplication
// centralized queue
// N tasks are created
void matmul_parallel_centralized_N_tasks(
  size_t N, size_t K, size_t M,
  const std::vector<int>& A,
  const std::vector<int>& B,
  std::vector<int>& C,
  Threadpool& threadpool
) {

  std::vector<std::future<void>> futures;
  
  for (size_t i = 0; i < N; i++) {
    futures.emplace_back(
      threadpool.insert([=, &A, &B, &C](){
        for (size_t j = 0; j < M; j++) {
          for (size_t k = 0; k < K; k++) {
            C[i*M + j] += A[i*K + k] * B[k*M + j];
          }
        }
      })
    );
  }
  
  // synchronize the execution on the N*M inner products
  for(auto& fu : futures) {
    fu.get();
  }
}


// parallel matrix multiplication
// centralized queue
// (N/num_threads) tasks are created
void matmul_parallel_centralized_less_tasks(
  size_t N, size_t K, size_t M,
  const std::vector<int>& A,
  const std::vector<int>& B,
  std::vector<int>& C,
  Threadpool& threadpool,
  const size_t T
) {

  std::vector<std::future<void>> futures;
  
  for (size_t i = 0; i < T; i++) {
    futures.emplace_back(
      threadpool.insert([=, &A, &B, &C](){
        for (size_t t = 0; t < N/T; t++) {
          for (size_t j = 0; j < M; j++) {
            for (size_t k = 0; k < K; k++) {
              C[(N/T*i+t)*M + j] += A[(N/T*i+t)*K + k] * B[k*M + j];
            }
          }
        }
      })
    );
  }
  
  // synchronize the execution on the N*M inner products
  for(auto& fu : futures) {
    fu.get();
  }
}
