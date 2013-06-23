#include <functional>
#include <type_traits>
#include <memory>
#include <atomic>
#include <future>
#include <vector>
#include <deque>
#include <mutex>

#include "thread_pool.h"

ThreadPool::ThreadPool() : m_done(false) , m_thread_joiner(m_workers) {
  std::size_t num_threads = std::max(std::thread::hardware_concurrency(), 2U);

  try {
    for (size_t i = 0; i < num_threads; ++i) {
      m_workers.push_back(std::thread(&ThreadPool::do_work, this));
    }
  } catch (...) {
    m_done = true;
    throw;
  }
}

ThreadPool::~ThreadPool() {
  m_done = true;
}

void ThreadPool::run_pending_tasks() {
  std::packaged_task<void ()> task;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_work_queue.empty()) {
      task = std::move(m_work_queue.front());
      m_work_queue.pop_front();
    }
  }

  if (task.valid()) {
    task();
  }
}

void ThreadPool::do_work() {
  while (!m_done) {
    run_pending_tasks();
  }
}
