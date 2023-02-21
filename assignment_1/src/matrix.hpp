#pragma once

#include <iostream>
#include <vector>
#include <future>
#include <queue>
#include "threadpool.hpp"

// A is N * K
// B is K * M
// C is N * M

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
// false sharing 
void matmul_parallel_false_sharing(
  size_t N, size_t K, size_t M,
  const std::vector<int>& A,
  const std::vector<int>& B,
  std::vector<int>& C,
  Threadpool_C& threadpool
) {

  std::vector<std::future<void>> futures;
  
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < M; j++) {
      futures.emplace_back(
        threadpool.insert([=, &A, &B, &C](){
          for (size_t k = 0; k < K; k++) {
            C[i*M + j] += A[i*K + k] * B[k*M + j];
          }
        })
      );
    }
  }
  
  for(auto& fu : futures) {
    fu.get();
  }
}

// parallel matrix multiplication
// no false sharing
void matmul_parallel_no_false_sharing(
  size_t N, size_t K, size_t M,
  const std::vector<int>& A,
  const std::vector<int>& B,
  std::vector<int>& C,
  Threadpool_C& threadpool
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
  
  for(auto& fu : futures) {
    fu.get();
  }
}


// parallel matrix multiplication
// block multiplication
// block size is 16
void matmul_parallel_block_matrix(
  size_t N, size_t K, size_t M,
  const std::vector<int>& A,
  const std::vector<int>& B,
  std::vector<int>& C,
  Threadpool_C& threadpool,
  const size_t T
) {

  std::vector<std::future<void>> futures;
  size_t block = 16;
 
  for (size_t i = 0; i < N; i+=block) {
    for (size_t j = 0; j < M; j+=block) {
      for (size_t k = 0; k < K; k+=block) {
        futures.emplace_back(
          threadpool.insert([=,&A,&B,&C](){
            for (size_t bi = i; bi < i+block; ++bi) {
              for (size_t bj = j; bj < j+block; ++bj) {
                size_t sum = 0;
                for (size_t bk = k; bk < k+block; ++bk) {
                  sum += A[bi*K+bk] * B[bk*M+bj];
                }
                C[bi*M+bj] += sum;
              }
            }
          })
        );
      }
    }
  }
  
  // synchronize the execution on the N*M inner products
  for(auto& fu : futures) {
    fu.get();
  }
}


// parallel matrix multiplication
// decentralized queue
void matmul_parallel_decentralized(
  size_t N, size_t K, size_t M,
  const std::vector<int>& A,
  const std::vector<int>& B,
  std::vector<int>& C,
  Threadpool_D& threadpool
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
  
  for(auto& fu : futures) {
    fu.get();
  }
}

// parallel matrix multiplication
// decentralized queues
// block multiplication
// block size is 16
void matmul_parallel_decentralized_block_matrix(
  size_t N, size_t K, size_t M,
  const std::vector<int>& A,
  const std::vector<int>& B,
  std::vector<int>& C,
  Threadpool_D& threadpool,
  const size_t T
) {

  std::vector<std::future<void>> futures;
  size_t block = 16;
 
  for (size_t i = 0; i < N; i+=block) {
    for (size_t j = 0; j < M; j+=block) {
      for (size_t k = 0; k < K; k+=block) {
        futures.emplace_back(
          threadpool.insert([=,&A,&B,&C](){
            for (size_t bi = i; bi < i+block; ++bi) {
              for (size_t bj = j; bj < j+block; ++bj) {
                size_t sum = 0;
                for (size_t bk = k; bk < k+block; ++bk) {
                  sum += A[bi*K+bk] * B[bk*M+bj];
                }
                C[bi*M+bj] += sum;
              }
            }
          })
        );
      }
    }
  }
  
  // synchronize the execution on the N*M inner products
  for(auto& fu : futures) {
    fu.get();
  }
}
