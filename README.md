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
  EXPECT_THAT(9, cctest::utils::is_divisible_by(3));
  EXPECT_THAT(std::string("Hello World"), cctest::utils::starts_with("Not Hello"));

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
```

The following will be the output
```sh
Running 3 tests
test test_case_check_eq ... PASSED
test yet_another_test ... FAILED
test another_test ... FAILED
Encountered a fatal error, aborted the remaining 0 tests

failures:
-----run----
run panicked at ./examples/example.cc:10
Expected "Hello World" to start with "Not Hello"

run panicked at ./examples/example.cc:22
Assertion failed
Expected: 4
Actual: 1

FATAL ERROR
-----run----
run panicked at ./examples/example.cc:30
assertion 1 == 3 failed

test result:FAILED. 1 passed; 2 failed;
```