parallel_qsort
==============

Parallel Quick Sort based on thread pool with support for work stealing.

Usage
=====

```cpp

#include <iostream>
#include <iterator>
#include <vector>
#include "parallel_sort.h"

int main() {
  std::vector<int> v = { 5, 4, 3, 2, 1, 6 };
  parallel_sort(v.begin(), v.end());

  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;

  return 0;
}
```
