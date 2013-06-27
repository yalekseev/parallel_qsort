#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <type_traits>
#include <functional>
#include <memory>
#include <atomic>
#include <future>
#include <vector>
#include <deque>
#include <mutex>

#include "thread_joiner.h"
#include "work_queue.h"

namespace internal {

template <typename R>
bool future_is_ready(std::future<R> & future) {
  return (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
}

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

        try {
            ++m_num_tasks;
            m_main_queue.push(std::move(task));
        } catch (...) {
            --m_num_tasks;
        }

        return res;
    }

    /*! Return true if there are no pending or running tasks. */
    bool empty() const;

    /*! Get next pending task and run it. */
    bool run_pending_tasks();

private:
    typedef std::packaged_task<void ()> task_type;
    typedef WorkQueue<task_type> work_queue_type;

    void do_work(std::size_t thread_index);
    /*! Get next pending task from local queue. */
    bool pop_task_from_my_queue(task_type & task);
    /*! Get next pending task from main queue. */
    bool pop_task_from_main_queue(task_type & task);
    /*! Get next pending task from neigbouhrs queue. */
    bool pop_task_from_other_queue(task_type & task);

    std::atomic<bool> m_done;
    // Number of pending or running tasks
    std::atomic<int> m_num_tasks;
    work_queue_type m_main_queue;
    std::vector<std::shared_ptr<work_queue_type> > m_thread_queues;
    std::vector<std::thread> m_workers;
    ThreadJoiner m_thread_joiner;

    // thread local members
    static thread_local std::size_t m_my_index;
    static thread_local work_queue_type * m_my_queue;
};

} // namespace internal

#endif
