#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

#include "../include/cctest.hpp"

TEST_CASE(test_case_check_eq) {
  std::string initial = "Hello World";
  initial.append("world2");
  ASSERT_EQ(initial, "Hello Worldworld2");
  ASSERT_EQ(initial.at(0), 'H');
  auto count = std::count_if(initial.begin(), initial.end(),
                             [](char pred) { return std::isdigit(pred); });
  ASSERT_EQ(count, 1);
}

TEST_CASE(yet_another_test) {
  std::vector<int> v{1, 2, 3, 4, 5};
  ASSERT(std::binary_search(v.begin(), v.end(), 3));
  FATAL_ASSERT(1 == 1);
  ASSERT_EQ(1, 4);
}

TEST_CASE(another_test) {
  std::vector<int> vec(10);
  std::iota(vec.begin(), vec.end(), 1);
  ASSERT_EQ(std::accumulate(vec.begin(), vec.end(), 0), 55);

  FATAL_ASSERT(1 == 3);
  ASSERT_NEQ(2 * 2, 4);
}

int main() { RUN_TESTS(); }