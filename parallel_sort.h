#include <algorithm>
#include <iterator>
#include <thread>

#include "thread_pool.h"

namespace internal {

template <typename R>
bool future_is_ready(std::future<R> & future) {
  return (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready);
}

template <typename Iterator>
class ParallelSorter {
public:
    ParallelSorter() = default;
    ParallelSorter(const ParallelSorter & other) = delete;
    ParallelSorter & operator=(const ParallelSorter & other) = delete;

    void sort(Iterator begin, Iterator end) {
      std::size_t size = std::distance(begin, end);
      if (size < 2) {
        return;
      }

      Iterator mid = begin + size / 2;

      std::nth_element(begin, mid, end);

      auto f1 = m_thread_pool.submit(std::bind(&ParallelSorter::sort, this, begin, mid));
      auto f2 = m_thread_pool.submit(std::bind(&ParallelSorter::sort, this, mid + 1, end));

      while (!future_is_ready(f1) || !future_is_ready(f2)) {
        m_thread_pool.run_pending_tasks();
        std::this_thread::yield();
      }
    }

private:
    ThreadPool m_thread_pool;
};

} // namespace internal

template <typename Iterator>
void parallel_sort(Iterator begin, Iterator end) {
    internal::ParallelSorter<Iterator> sorter;
    sorter.sort(begin, end);
}
