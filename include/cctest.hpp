#ifndef CCTEST_HPP_INCLUDED
#define CCTEST_HPP_INCLUDED

#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#ifndef __FUNCTION_NAME__
#ifdef WIN32
#define __FUNCTION_NAME__ __FUNCTION__
#else
#define __FUNCTION_NAME__ __func__
#endif
#endif

#define TEST_CASE(name)                                       \
  class test_case_##name : public test_case {                 \
   public:                                                    \
    static test_case_##name _unused_test_case_var;            \
    test_case_##name() : test_case(#name) {}                  \
    void run() const override;                                \
  };                                                          \
  test_case_##name test_case_##name::_unused_test_case_var{}; \
  void test_case_##name::run() const

#define RUN_TESTS() test_collection::run_tests()
#define ASSERT(expression) \
  assertions::assert(#expression, __FUNCTION_NAME__, __FILE__, expression)

#define ASSERT_EQ(expr1, expr2) ASSERT(expr1 == expr2)
#define ASSERT_NEQ(expr1, expr2) ASSERT(expr1 != expr2)

class assertion_failure : public std::exception {
 private:
  std::string expression, func_name, fname;

 public:
  assertion_failure(const std::string&& expression_,
                    const std::string&& func_name_, const std::string&& fname_)
      : expression(expression_), func_name(func_name_), fname(fname_) {}

  std::string to_string() const {
    std::stringstream error_stream;
    error_stream << "-----" << func_name << "----\n";
    error_stream << func_name << " panicked at " << fname << "\n";
    error_stream << "assertion " << expression << " failed\n";
    return error_stream.str();
  }
};

constexpr const char* failed_str = "\033[31mFAILED\033[39m";
constexpr const char* passed_str = "\033[32mPASSED\033[39m";

class test_case {
 private:
  std::string test_name;

 public:
  test_case(const std::string&& test);
  virtual void run() const {}
  const std::string& get_name() const { return test_name; }
};

class test_collection {
 private:
  test_collection() = delete;
  inline static std::vector<test_case*> tests{};

 public:
  static char append(test_case* test_case) {
    tests.push_back(test_case);
    return 0;
  }

  static std::optional<assertion_failure> run_individual_test(
      const test_case* test) {
    try {
      test->run();
    } catch (const assertion_failure& exception) {
      return exception;
    };
    return {};
  }

  static void run_tests() {
    std::ios_base::sync_with_stdio(false);
    std::stringstream out_stream;
    std::vector<std::string> failures;
    out_stream << "Running " << tests.size()
               << (tests.size() == 1 ? " test" : " tests");

    for (const auto& unit_test : tests) {
      auto result = run_individual_test(unit_test);
      out_stream << "\ntest " << unit_test->get_name() << " ... "
                 << (result.has_value() ? failed_str : passed_str);
      if (result.has_value()) {
        const auto failure = result.value();
        failures.push_back(std::move(failure.to_string()));
      }
    }
    if (failures.size() > 0) {
      out_stream << "\nfailures:\n";
      for (const std::string& failure : failures) {
        out_stream << failure << "\n";
      }
    } else {
      out_stream << "\n";
    }

    const std::size_t passed_count = tests.size() - failures.size();
    out_stream << "test result:"
               << (failures.size() > 0 ? failed_str : passed_str) << ". "
               << passed_count << " passed; " << failures.size()
               << " failed;\n";
    std::cout << out_stream.rdbuf();
  }
};

test_case::test_case(const std::string&& test) {
  test_name = std::move(test);
  test_collection::append(this);
}

class assertions {
 public:
  static void assert(const std::string&& expression_str,
                     const std::string&& func_name, const std::string&& fname,
                     bool expression) {
    if (!expression) {
      throw assertion_failure(std::move(expression_str), std::move(func_name),
                              std::move(fname));
    }
  }
};

#endif
