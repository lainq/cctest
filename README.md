### cctest
cctest is a very simple library made for testing in c++

The library is a header-only library which contains a single header file which can be copied into your projects includes.

#### Usage
Here's an example
```cc
#include "../include/cctest.hpp"
#include ...

// ...
TEST_CASE(test_case_check_eq) {
  std::string initial = "Hello World";
  initial.append("world2");
  ASSERT_EQ(initial, "Hello Worldworld2"); // PASSED
  auto count = std::count_if(initial.begin(), initial.end(), [](char pred) {
    return std::isdigit(pred);
  });  
  ASSERT_EQ(count, 1); // PASSED
}

TEST_CASE(another_test) {
  std::vector<int> vec(10);
  std::iota(vec.begin(), vec.end(), 1);
  ASSERT_EQ(std::accumulate(vec.begin(), vec.end(), 0), 55); // PASSED
  ASSERT_NEQ(2 * 2, 4); // FAILED
}

int main() {
  REGISTER_TEST(another_test);
  REGISTER_TEST(test_case_check_eq);
  RUN_TESTS();
}
```

The following will be the output
```sh
Running 2 tests
test another_test ... FAILED
test test_case_check_eq ... PASSED
failures:
-----test_another_test----
test_another_test panicked at example.cc
assertion 2 * 2 != 4 failed

test result:FAILED. 1 passed; 1 failed;
```