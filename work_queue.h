#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <limits>
#include <deque>
#include <mutex>

namespace internal {

template <typename T>
class WorkQueue {
private:
    enum { MAX_CAPACITY = std::numeric_limits<std::size_t>::max() };

public:
    explicit WorkQueue(std::size_t capacity = MAX_CAPACITY);
    WorkQueue(const WorkQueue & other) = delete;
    WorkQueue & operator=(const WorkQueue & other) = delete;

    bool empty() const;

    void push(const T & val);
    void push(T && val);
    bool try_push(const T & val);
    bool try_push(T && val);

    void pop(T & val);
    bool try_pop(T & val);
    bool try_steal(T & val);

    void set_capacity(std::size_t capacity);

private:
    std::size_t m_capacity;
    std::deque<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_non_empty_cond;
    std::condition_variable m_non_full_cond;
};

template <typename T>
WorkQueue<T>::WorkQueue(std::size_t capacity) : m_capacity(capacity) { }

template <typename T>
bool WorkQueue<T>::empty() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
}

template <typename T>
void WorkQueue<T>::push(const T & val) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_non_full_cond.wait(lock, [&](){ return m_queue.size() < m_capacity; });
    m_queue.push_back(val);
    m_non_empty_cond.notify_one();
}

template <typename T>
void WorkQueue<T>::push(T && val) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_non_full_cond.wait(lock, [&](){ return m_queue.size() < m_capacity; });
    m_queue.push_back(std::move(val));
    m_non_empty_cond.notify_one();
}

template <typename T>
bool WorkQueue<T>::try_push(const T & val) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.size() < m_capacity) {
        m_queue.push_back(val);
        return true;
    }

    return false;
}

template <typename T>
bool WorkQueue<T>::try_push(T && val) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.size() < m_capacity) {
        m_queue.push_back(std::move(val));
        return true;
    }

    return false;
}

template <typename T>
void WorkQueue<T>::pop(T & val) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_non_empty_cond.wait(lock, [&](){ return !m_queue.empty(); });
    val = std::move(m_queue.back());
    m_queue.pop_back();
    m_non_full_cond.notify_one();
}

template <typename T>
bool WorkQueue<T>::try_pop(T & val) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_queue.empty()) {
        val = std::move(m_queue.back());
        m_queue.pop_back();
        return true;
    }

    return false;
}

template <typename T>
bool WorkQueue<T>::try_steal(T & val) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_queue.empty()) {
        val = std::move(m_queue.front());
        m_queue.pop_front();
        return true;
    }

    return false;
}

template <typename T>
void WorkQueue<T>::set_capacity(std::size_t capacity) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_capacity = capacity;
}

} // namespace internal

#endif
