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


class ThreadPool {
public:
  ThreadPool();

  ~ThreadPool();

  template <typename F>
  std::future<void> submit(F f) {
    std::packaged_task<void()> task(f);
    std::future<void> res = task.get_future();
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_work_queue.push_back(std::move(task));
    }

    return res;
  }

  void run_pending_tasks();

private:
  void do_work();

  std::atomic<bool> m_done;
  std::deque<std::packaged_task<void ()> > m_work_queue;
  std::mutex m_mutex;
  std::vector<std::thread> m_workers;
  ThreadJoiner m_thread_joiner;
};

#endif
