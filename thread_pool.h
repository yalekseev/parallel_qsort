#ifndef THREAD_POOL
#define THREAD_POOL

#include <functional>
#include <type_traits>
#include <memory>
#include <atomic>
#include <future>
#include <vector>
#include <deque>
#include <mutex>

#include "thread_joiner.h"
#include "work_queue.h"

class ThreadPool {
public:
  ThreadPool();
  ThreadPool(const ThreadPool & other) = delete;
  ThreadPool & operator=(const ThreadPool & other) = delete;

  ~ThreadPool();

  template <typename F>
  std::future<void> submit(F f) {
    task_type task(f);
    std::future<void> res = task.get_future();

    m_main_queue.push(std::move(task));

    return res;
  }

  void run_pending_tasks();

private:
  typedef std::packaged_task<void ()> task_type;
  typedef WorkQueue<task_type> work_queue_type;

  void do_work(std::size_t thread_index);
  bool pop_task_from_my_queue(task_type & task);
  bool pop_task_from_main_queue(task_type & task);
  bool pop_task_from_other_queue(task_type & task);

  std::atomic<bool> m_done;
  work_queue_type m_main_queue;
  std::vector<std::shared_ptr<work_queue_type> > m_thread_queues;
  std::vector<std::thread> m_workers;
  ThreadJoiner m_thread_joiner;

  // thread local members
  static thread_local std::size_t m_my_index;
  static thread_local work_queue_type * m_my_queue;
};

#endif
