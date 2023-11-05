#ifndef CCTEST_HPP_INCLUDED
#define CCTEST_HPP_INCLUDED

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <exception>
#include <sstream>
#include <iostream>
#include <optional>

#ifndef __FUNCTION_NAME__
  #ifdef WIN32 
    #define __FUNCTION_NAME__ __FUNCTION__
  #else
    #define __FUNCTION_NAME__ __func__
  #endif
#endif

#define TEST_CASE(name) static void test_##name()

#define RUN_TESTS() test_collection::run_tests()
#define REGISTER_TEST(name) \
test_collection::append(#name, \
std::function<std::remove_pointer_t<decltype(test_##name)\
>>(test_##name))

#define ASSERT(expression) assertions::assert(#expression, \
  __FUNCTION_NAME__, __FILE__, expression)

#define ASSERT_EQ(expr1, expr2) ASSERT(expr1 == expr2)
#define ASSERT_NEQ(expr1, expr2) ASSERT(expr1 != expr2)

class assertion_failure: public std::exception {
private:
  std::string expression, func_name, fname;
public:
  assertion_failure(const std::string&& expression_,
    const std::string&& func_name_,
    const std::string&& fname_):
    expression(expression_),
  func_name(func_name_),
  fname(fname_) {}

  std::string to_string() const {
    std::stringstream error_stream;
    error_stream << "-----" << func_name << "----\n";
    error_stream << func_name << " panicked at " << fname << "\n";
    error_stream << "assertion " << expression << " failed\n";
    return error_stream.str();
  }
};

class unit_test {public:
  using test_function_t = std::function<void()>;
  unit_test(const std::string&& tname, test_function_t&& func) :
    name(std::move(tname)), test_func(std::move(func)) {}
  std::optional<assertion_failure> run() const {
    try {
      test_func();    } catch(assertion_failure& exception) {
           return exception;
    }
    return {};
  }
  const std::string& get_name() const { return name ;}

private:  const std::string name;
  unit_test::test_function_t test_func;
};

constexpr const char* failed_str = "\033[31mFAILED\033[39m";
constexpr const char* passed_str = "\033[32mPASSED\033[39m";

class test_collection {
private:
  test_collection() = delete;
  inline static std::vector<std::unique_ptr<unit_test>> tests {};
public:
  template<typename... Args>
  static void append(const std::string&& expression, unit_test::test_function_t&& func) {
    std::unique_ptr<unit_test> test { new unit_test(std::move(expression), std::move(func)) };
    tests.push_back(std::move(test));
  }

  static void run_tests() {
    std::ios_base::sync_with_stdio(false);
    std::stringstream out_stream;
    std::vector<std::string> failures;
    out_stream << "Running " << tests.size() << (tests.size() == 1 ? " test" : " tests");

    for(const auto& unit_test: tests) {
      auto result = unit_test->run();
      out_stream << "\ntest " << unit_test->get_name() << " ... " <<
        (result.has_value() ?  failed_str : passed_str);
      if(result.has_value()) {
        const auto failure = result.value();
        failures.push_back(std::move(failure.to_string()));
      }
    }
    if(failures.size() > 0) {
    out_stream << "\nfailures:\n";
    for(const std::string& failure: failures) {
      out_stream << failure << "\n";
    }} else { out_stream << "\n";}

    const std::size_t passed_count = tests.size() - failures.size();
    out_stream << "test result:" << (failures.size() > 0 ? 
      failed_str: passed_str) << ". " << passed_count << " passed; " <<
      failures.size() << " failed;\n";
    std::cout << out_stream.rdbuf();
  }
};

class assertions {
public:
  static void assert(const std::string&& expression_str,
    const std::string&& func_name,
    const std::string&& fname, bool expression) {
    if(!expression) {      throw assertion_failure(std::move(expression_str),
       std::move(func_name), std::move(fname));
    }
  }
};

#endif
