#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include "parallel_sort.h"

int main() {
    std::vector<int> v = { 3, 1, 8, 5, 10, 7, 21, 14, 37, 29, 2, 43, 7, 67 };

    parallel_sort(v.begin(), v.end());

    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    return 0;
}
