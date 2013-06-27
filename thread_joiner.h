#ifndef THREAD_JOINER_H
#define THREAD_JOINER_H

#include <thread>
#include <vector>

class ThreadJoiner {
public:
    ThreadJoiner() = delete;
    ThreadJoiner(const ThreadJoiner & other) = delete;
    ThreadJoiner & operator=(const ThreadJoiner & other) = delete;

    ThreadJoiner(std::vector<std::thread> & threads) : m_threads(threads) { }

    ~ThreadJoiner() {
        for (auto & th : m_threads) {
            if (th.joinable()) {
                th.join();
            }
        }
    }

private:
    std::vector<std::thread> & m_threads;
};

#endif
