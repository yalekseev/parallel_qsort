#include <algorithm>
#include <iostream>
#include <iterator>
#include <cassert>
#include <cstdlib>
#include <vector>

#include "parallel_sort.h"

void generate_vector(std::vector<int> & v) {
  v.clear();

  std::size_t size = std::rand() % 5000000 + 1;

  v.reserve(size);
  for (std::size_t i = 0; i < size; ++i) {
    v.push_back(std::rand());
  }
}

int main() {
  std::srand(123);

  for (size_t i = 0; i < 10; ++i) {
    std::vector<int> v1;
    generate_vector(v1);

    std::vector<int> v2(v1);

    std::sort(v1.begin(), v1.end());
    parallel_sort(v2.begin(), v2.end());

    assert(v1 == v2);
  }

  return 0;
}
