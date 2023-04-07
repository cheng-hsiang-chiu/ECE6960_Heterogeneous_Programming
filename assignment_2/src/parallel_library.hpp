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
#include <numeric>

template <typename T>
struct MoC {

  MoC(T&& rhs) : object(std::move(rhs)) {}
  MoC(const MoC& other) : object(std::move(other.object)) {}

  T& get() { return object; }

  mutable T object;
};

// ----------------------------------------------------------------------------
// Class definition for Threadpool
// ----------------------------------------------------------------------------

class Threadpool {

  public:
    
    // constructor tasks a unsigned integer representing the number of
    // workers you need
    Threadpool(size_t N) {

      for(size_t i=0; i<N; i++) {
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
    ~Threadpool() {
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

    // reduce with static scheduling
    template <typename Input, typename T, typename F>
    T reduce_static(Input beg, Input end, T init, F bop, size_t chunk_size = 2) {
    
      // the total number of elements in the range [beg, end)
      size_t N = std::distance(beg, end);

      std::vector<std::future<void>> futures;
    
      std::atomic<size_t> takens{0};

      std::mutex mutex;

      for (size_t i = 0; i < threads.size(); ++i) {
        futures.emplace_back(insert([N, beg, end, bop, &init, &mutex, chunk_size, &takens](){
          
          // pre-reduce
          size_t curr_b = takens.fetch_add(2, std::memory_order_relaxed);

          // corner case #1: no more elements to reduce
          if(curr_b >= N) {
            return;
          }

          // corner case #2: only one element left
          if(N - curr_b == 1) {
            std::scoped_lock lock(mutex);
            init = bop(init, *(beg + curr_b));
            return;
          }

          // perform a reduction on these two elements
          T temp = bop(*(beg+curr_b), *(beg+curr_b+1));
          
          curr_b = takens.fetch_add(chunk_size, std::memory_order_relaxed);
          
          while (curr_b < N) {
            size_t curr_e = std::min(N, curr_b + chunk_size);
            // run a sequential reduction to the range specified by beg + [curr_b, curr_e)
            temp = std::accumulate(beg + curr_b, beg + curr_e, temp, bop);
            
            // get the next chunk
            curr_b = takens.fetch_add(chunk_size, std::memory_order_relaxed);
          }

          // perform a final reduction on temp with init
          {
            std::scoped_lock lock(mutex);
            init = bop(init, temp);
          }
        }));
      }

      // caller thread to wait for all W tasks finish (futures)
      for(auto & fu : futures) {
        fu.get();
      }

      return init;
    }
    
    // reduce with guided scheduling
    template <typename Input, typename T, typename F>
    T reduce_guided(Input beg, Input end, T init, F bop, size_t chunk_size = 2) {
    
      // the total number of elements in the range [beg, end)
      size_t N = std::distance(beg, end);

      std::vector<std::future<void>> futures;
    
      std::atomic<size_t> takens{0};

      std::mutex mutex;

      size_t workers = threads.size();

      for (size_t i = 0; i < threads.size(); ++i) {
        futures.emplace_back(insert([N, beg, end, bop, &init, &mutex, chunk_size, &takens, workers](){
          
          size_t threshold = 2*workers*(chunk_size+1);  // threshold to perform fine-grained scheduling
          float  p = 1.0/(2*workers);

          T temp{0};
          size_t curr_b = takens.load(std::memory_order_relaxed);

          while(curr_b < N) {
            size_t remaining = N - curr_b;
           
            // fine grained 
            if (remaining <= threshold) {
              curr_b = takens.fetch_add(chunk_size, std::memory_order_relaxed);
              if(curr_b >= N) {
                break;
              }
              size_t curr_e = std::min(N, curr_b + chunk_size);
              temp = std::accumulate(beg + curr_b, beg + curr_e, temp, bop);
              curr_b = takens.load(std::memory_order_relaxed);
            }

            // coarse grained
            else {
              size_t q = remaining * p;
              if (q < chunk_size) {
                q = chunk_size;
              }

              size_t curr_e = std::min(N, curr_b + q);

              if (takens.compare_exchange_strong(curr_b, curr_e, std::memory_order_relaxed,
                                                                 std::memory_order_relaxed)) {
                
                temp = std::accumulate(beg + curr_b, beg + curr_e, temp, bop);
                curr_b = takens.load(std::memory_order_relaxed);
              }
            }
          }

          // perform a final reduction on temp with init
          {
            std::scoped_lock lock(mutex);
            init = bop(init, temp);
          }
        }));
      }

      // caller thread to wait for all W tasks finish (futures)
      for(auto & fu : futures) {
        fu.get();
      }

      return init;
    }


  private:

    std::mutex mtx;
    std::vector<std::thread> threads;
    std::condition_variable cv;
    bool stop {false};
    std::queue< std::function<void()> > queue;

};

auto seq_reduce(std::vector<int>& vec, int initial) {
  return 
  std::accumulate(
    vec.begin(),
    vec.end(), 
    initial, 
    [](int a, int b){
      return a + b; 
    }
  );
}

auto par_reduce_static(std::vector<int>& vec, int initial, size_t chunk_size, Threadpool& threadpool) {
  return 
  threadpool.reduce_static(
    vec.begin(), 
    vec.end(), 
    initial, 
    [](int a, int b){
      return a + b;
    },
    chunk_size
  );

}

auto par_reduce_guided(std::vector<int>& vec, int initial, size_t chunk_size, Threadpool& threadpool) {
  return
  threadpool.reduce_guided(
    vec.begin(), 
    vec.end(), 
    initial, 
    [](int a, int b){
      return a + b;
    },
    chunk_size
  );
}
