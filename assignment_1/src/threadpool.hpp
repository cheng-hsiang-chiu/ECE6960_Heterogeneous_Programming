#include <iostream>
#include <chrono>
#include <vector>
#include <future>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <type_traits>

#pragma once


template <typename T>
struct MoC {

  MoC(T&& rhs) : object(std::move(rhs)) {}
  MoC(const MoC& other) : object(std::move(other.object)) {}

  T& get() { return object; }

  mutable T object;
};

// ----------------------------------------------------------------------------
// Class definition for Threadpool with centralized queue
// ----------------------------------------------------------------------------

class Threadpool_C {

  public:
    
    // constructor tasks a unsigned integer representing the number of
    // workers you need
    Threadpool_C(size_t N) {

      for (size_t i = 0; i < N; i++) {

        threads.emplace_back([this](){
          // keep doing my job until the main thread sends a stop signal
          while(!stop) {
            std::function<void()> task;
            // my job is to iteratively grab a task from the queue
            {
              // Best practice: anything that happens inside the while continuation check
              // should always be protected by lock
              std::unique_lock lock(mtx);
              while(queue.empty() && !stop) {
                cv.wait(lock);
              }
              if(!queue.empty()) {
                task = queue.front();
                queue.pop();
              }
            }
            // and run the task...
            if(task) {
              task();
            }
          }
        });
      }
    }

    // destructor will release all threading resources by joining all of them
    ~Threadpool_C() {
      // I need to join the threads to release their resources
      for(auto& t : threads) {
        t.join();
      }
    }

    // shutdown the threadpool
    void shutdown() {
      std::scoped_lock lock(mtx);
      stop = true;
      cv.notify_all();
    }

    // insert a task "callable object" into the threadpool
    template <typename C>
    auto insert(C&& task) {
      std::promise<void> promise;
      auto fu = promise.get_future();
      {
        std::scoped_lock lock(mtx);
        queue.push(
          [moc=MoC{std::move(promise)}, task=std::forward<C>(task)] () mutable {
            task();
            moc.object.set_value();
          }
        );
      }
      cv.notify_one();
      return fu;
    }
    

  private:

    std::mutex mtx;
    std::vector<std::thread> threads;
    std::condition_variable cv;
    
    bool stop {false};
    std::queue< std::function<void()> > queue;

};


// ----------------------------------------------------------------------------
// Class definition for Threadpool with decentralized queue
// Every thread has its own local queue
// Main thread push tasks into a queue in a round robin manner
// ----------------------------------------------------------------------------

class Threadpool_D {

  public:
    
    // constructor tasks a unsigned integer representing the number of
    // workers you need
    Threadpool_D(size_t N): number_threads{N}, mtxs(N), cvs(N), queues(N) {

      for (size_t i = 0; i < N; i++) {
        threads.emplace_back([this, i](){
          // keep doing my job until the main thread sends a stop signal
          while(!stop) {
            std::function<void()> task;
            // my job is to iteratively grab a task from the queue
            {
              // Best practice: anything that happens inside the while continuation check
              // should always be protected by lock
              std::unique_lock lock(mtxs[i]);
              while(queues[i].empty() && !stop) {
                cvs[i].wait(lock);
              }
              if(!queues[i].empty()) {
                task = queues[i].front();
                queues[i].pop();
              }
            }
            // and run the task...
            if(task) {
              task();
            }
          }
        });
      }
    }

    // destructor will release all threading resources by joining all of them
    ~Threadpool_D() {
      // I need to join the threads to release their resources
      for(auto& t : threads) {
        t.join();
      }
    }

    // shutdown the threadpool
    void shutdown() {
      //std::scoped_lock lock(mtx);
      stop = true;

      for (size_t i = 0; i < number_threads; ++i) {
        cvs[i].notify_one();
      }
    }

    // insert a task "callable object" into the threadpool
    template <typename C>
    auto insert(C&& task) {
      
      std::promise<void> promise;
      auto fu = promise.get_future();
      
      {
        std::scoped_lock lock(mtxs[turn]);
        queues[turn].push(
          [moc=MoC{std::move(promise)}, task=std::forward<C>(task)] () mutable {
            task();
            moc.object.set_value();
          }
        );
      }

      cvs[turn].notify_one();

      turn = (turn+1)%number_threads;
      
      return fu;
    }
  
    
  private:

    size_t number_threads; 
    size_t turn{0};
    std::vector<std::mutex> mtxs;
    std::vector<std::thread> threads;
    std::vector<std::condition_variable> cvs;
    bool stop {false};
    std::vector<std::queue<std::function<void()>>> queues;

};
