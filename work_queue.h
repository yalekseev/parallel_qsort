#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <deque>
#include <mutex>

template <typename T>
class WorkQueue {
public:
    void push(const T & val);
    void push(T && val);

    bool try_pop(T & val);
    bool try_steal(T & val);

private:
    std::deque<T> m_queue;
    mutable std::mutex m_mutex;
};

template <typename T>
void WorkQueue<T>::push(const T & val) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(val);
}

template <typename T>
void WorkQueue<T>::push(T && val) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(std::move(val));
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

#endif
