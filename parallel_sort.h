#ifndef PARALLEL_SORT_H
#define PARALLEL_SORT_H

#include <algorithm>
#include <iterator>
#include <thread>

#include "thread_pool.h"

namespace internal {

template <typename Iterator>
class ParallelSorter {
public:
    ParallelSorter() = default;
    ParallelSorter(const ParallelSorter & other) = delete;
    ParallelSorter & operator=(const ParallelSorter & other) = delete;

    void sort(Iterator begin, Iterator end) {
        m_thread_pool.submit(std::bind(&ParallelSorter::do_sort, this, begin, end));

        while (!m_thread_pool.empty()) {
            m_thread_pool.run_pending_tasks();
        }
    }

private:
    void do_sort(Iterator begin, Iterator end) {
      std::size_t size = std::distance(begin, end);
      if (size < 2) {
        return;
      }

      Iterator mid = begin + size / 2;

      std::nth_element(begin, mid, end);

      m_thread_pool.submit(std::bind(&ParallelSorter::do_sort, this, begin, mid));
      m_thread_pool.submit(std::bind(&ParallelSorter::do_sort, this, mid + 1, end));
    }

    ThreadPool m_thread_pool;
};

} // namespace internal

template <typename Iterator>
void parallel_sort(Iterator begin, Iterator end) {
    internal::ParallelSorter<Iterator> sorter;
    sorter.sort(begin, end);
}

#endif
