#ifndef THREAD_JOINER
#define THREAD_JOINER

#include <thread>
#include <vector>

class ThreadJoiner {
public:
  ThreadJoiner(std::vector<std::thread> & threads) : m_threads(threads) {

  }

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
