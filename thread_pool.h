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
    std::packaged_task<void()> task(f);
    std::future<void> res = task.get_future();

    m_work_queue.push(std::move(task));

    return res;
  }

  void run_pending_tasks();

private:
  void do_work();

  std::atomic<bool> m_done;
  WorkQueue<std::packaged_task<void ()> > m_work_queue;
  std::vector<std::thread> m_workers;
  ThreadJoiner m_thread_joiner;
};

#endif
