#include "../include/cctest.hpp"
#include <string>
#include <algorithm>
#include <numeric>
#include <vector>

TEST_CASE(test_case_check_eq) {
  std::string initial = "Hello World";
  initial.append("world2");
  std::cout << initial << "\n";
  ASSERT_EQ(initial, "Hello Worldworld2");
  ASSERT_EQ(initial.at(0), 'H');
  // std::cout << count << "\n";
  auto count = std::count_if(initial.begin(), initial.end(), [](char pred) {
    return std::isdigit(pred);
  });  ASSERT_EQ(count, 1);
}

TEST_CASE(another_test) {
  std::vector<int> vec(10);
  std::iota(vec.begin(), vec.end(), 1);
  ASSERT_EQ(std::accumulate(vec.begin(), vec.end(), 0), 55);
  ASSERT_NEQ(2 * 2, 4);
}

int main() {
  REGISTER_TEST(another_test);
  REGISTER_TEST(test_case_check_eq);
  RUN_TESTS();
}