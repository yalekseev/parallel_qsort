#include <type_traits>
#include <functional>
#include <algorithm>
#include <memory>
#include <atomic>
#include <future>
#include <vector>
#include <deque>
#include <mutex>

#include "thread_pool.h"

thread_local std::size_t ThreadPool::m_my_index;
thread_local ThreadPool::work_queue_type * ThreadPool::m_my_queue;

ThreadPool::ThreadPool() : m_done(false) , m_thread_joiner(m_workers) {
  std::size_t num_threads = std::max(std::thread::hardware_concurrency(), 4U);

  try {
    // Create per thread queues prior to creating threads
    for (std::size_t i = 0; i < num_threads; ++i) {
      m_thread_queues.push_back(std::shared_ptr<work_queue_type>(new work_queue_type));
    }

    // Create working thresds
    for (std::size_t i = 0; i < num_threads; ++i) {
      m_workers.push_back(std::thread(&ThreadPool::do_work, this, i));
    }
  } catch (...) {
    m_done = true;
    throw;
  }
}

ThreadPool::~ThreadPool() {
  m_done = true;
}

bool ThreadPool::pop_task_from_my_queue(task_type & task) {
  return m_my_queue && m_my_queue->try_pop(task);
}

bool ThreadPool::pop_task_from_main_queue(task_type & task) {
  return m_main_queue.try_pop(task);
}

bool ThreadPool::pop_task_from_other_queue(task_type & task) {
  for (std::size_t i = (m_my_index + 1) % m_thread_queues.size(); i != m_my_index; i = (i + 1) % m_thread_queues.size()) {
    if (m_thread_queues[i]->try_steal(task)) {
      return true;
    }
  }

  return false;
}

void ThreadPool::run_pending_tasks() {
  task_type task;

  if (pop_task_from_my_queue(task) || pop_task_from_main_queue(task) || pop_task_from_other_queue(task)) {
    task();
  }
}

void ThreadPool::do_work(std::size_t thread_index) {
  m_my_index = thread_index;
  m_my_queue = m_thread_queues[m_my_index].get();

  while (!m_done) {
    run_pending_tasks();
  }
}
