//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#pragma once

#include <unistd.h>

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

//#define MPMC
#ifdef MPMC
#include "mpmc.hpp"
#else
#include "spmc.hpp"
#endif

#define DISABLE_THREAD_POOL

template <class Func, class... Args>
inline auto forward_as_lambda(Func &&func, Args &&...args) {
  return [f   = std::forward<decltype(func)>(func),
          tup = std::tuple<std::conditional_t<std::is_lvalue_reference_v<Args>, Args, std::remove_reference_t<Args>>...>(
              std::forward<decltype(args)>(args)...)]() mutable { return std::apply(std::move(f), std::move(tup)); };
}

template <class Func, class T, class... Args>
inline auto forward_as_lambda2(Func &&func, T &&first, Args &&...args) {
  return [f   = std::forward<decltype(func)>(func),
          tt  = std::forward<decltype(first)>(first),
          tup = std::tuple<std::conditional_t<std::is_lvalue_reference_v<Args>, Args, std::remove_reference_t<Args>>...>(
              std::forward<decltype(args)>(args)...)]() mutable {
    return std::apply(std::move(f), std::tuple_cat(std::forward_as_tuple(tt), std::move(tup)));
  };
}

class Thread_pool {
  std::vector<std::thread> threads;
  std::atomic_flag booting_lock = ATOMIC_FLAG_INIT;
  std::atomic<bool> started_lock;

#ifdef MPMC
  mpmc<std::function<void(void)>> queue;
#else
  spmc256<std::function<void(void)>> queue;
#endif

  std::atomic<int>  jobs_left;
  std::atomic<bool> finishing;

  size_t thread_count;

  std::condition_variable job_available_var;
  std::mutex              queue_mutex;

  void task() {
    if (!booting_lock.test_and_set(std::memory_order_acquire)) {
      for (unsigned i = 1; i < thread_count; ++i) {
        threads.push_back(std::thread([this] { this->task(); }));
      }
      started_lock = true;
    }

    while (!finishing) {
      next_job()();
      jobs_left.fetch_sub(1, std::memory_order_relaxed);
    }
  }

  std::function<void(void)> next_job() {
    std::function<void(void)>    res;
    std::unique_lock<std::mutex> job_lock(queue_mutex);

    // Wait for a job if we don't have any.
    job_available_var.wait(job_lock, [this]() -> bool { return !queue.empty() || finishing; });

    bool has_work = queue.dequeue(res);
    if (has_work) {
      return res;
    }

    jobs_left.fetch_add(1, std::memory_order_relaxed);

    return [] {};  // Nothing to do
  }

  void add_(std::function<void(void)> job) {
#ifdef DISABLE_THREAD_POOL
    job();
    return;
#else
    if (jobs_left > 48) {  // FIXME->sh: what if not so much core?
      job();
      return;
    }
    jobs_left.fetch_add(1, std::memory_order_relaxed);
    queue.enqueue(job);
    job_available_var.notify_one();
#endif
  }

public:
  Thread_pool(int _thread_count = 0)
      :
#ifdef MPMC
      queue(256)
      ,
#endif
      jobs_left(0)
      , finishing(false) {

    started_lock = false;

    thread_count = _thread_count;
    size_t lim   = (std::thread::hardware_concurrency() - 1);  // -1 for calling thread

    if (thread_count > lim || thread_count == 0)
      thread_count = lim;
    else if (thread_count < 1)
      thread_count = 1;

    assert(thread_count);

    threads.push_back(std::thread([this] { this->task(); }));  // Just one thread in critical path
  }

  virtual ~Thread_pool() {
    while(!started_lock)
      ; // The exit could be before we even booting the threads (uff)

    wait_all();

    finishing = true;

    job_available_var.notify_all();

    for (auto &x : threads)
      if (x.joinable())
        x.join();
  }

  inline unsigned size() const { return thread_count; }

  template <class Func, class... Args>
  void add(Func &&func, Args &&...args) {
    return add_(forward_as_lambda(std::forward<decltype(func)>(func), std::forward<decltype(args)>(args)...));
  }
#if 1
  template <class Func, class T, class... Args>
  void add(Func &&func, T *first, Args &&...args) {
    return add_(forward_as_lambda2(std::forward<decltype(func)>(func),
                                   std::forward<decltype(first)>(first),
                                   std::forward<decltype(args)>(args)...));
  }
#endif

  void wait_all() {
    while (jobs_left > 0) {
      std::function<void(void)> res;
      bool                      has_work = queue.dequeue(res);
      if (has_work) {
        res();
        jobs_left.fetch_sub(1, std::memory_order_relaxed);
      }
    }
  }
};

extern Thread_pool thread_pool;
