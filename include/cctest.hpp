#ifndef CCTEST_HPP_INCLUDED
#define CCTEST_HPP_INCLUDED

#include <exception>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#ifndef __FUNCTION_NAME__
#ifdef WIN32
#define __FUNCTION_NAME__ __FUNCTION__
#else
#define __FUNCTION_NAME__ __func__
#endif
#endif

#define TEST_CASE(name)                                       \
  class test_case_##name : public cctest::test_case {         \
   public:                                                    \
    static test_case_##name _unused_test_case_var;            \
    test_case_##name() : cctest::test_case(#name) {}          \
    void run() const override;                                \
  };                                                          \
  test_case_##name test_case_##name::_unused_test_case_var{}; \
  void test_case_##name::run() const

#define RUN_TESTS() cctest::test_collection::run_tests()

#define ASSERT__(expression, is_fatal)                                 \
  cctest::assertions::assert(#expression, __FUNCTION_NAME__, __FILE__, \
                             __LINE__, expression, is_fatal)
#define ASSERT(expression) ASSERT__(expression, false)

#define ASSERT_EQ(expr1, expr2)                                              \
  if constexpr (cctest::has_streamable_traits<decltype(expr1)>() &&          \
                cctest::has_streamable_traits<decltype(expr2)>()) {          \
    cctest::assertions::assert_eq(expr1, expr2, __FUNCTION_NAME__, __FILE__, \
                                  __LINE__, expr1 == expr2, false);          \
  } else {                                                                   \
    ASSERT(expr1 == expr2);                                                  \
  }

#define ASSERT_NEQ(expr1, expr2) ASSERT(expr1 != expr2)

#define FATAL_ASSERT(expression) ASSERT__(expression, true)

namespace cctest {

template <typename T, typename = void>
struct has_streamable_traits : public std::false_type {};

template <typename T>
struct has_streamable_traits<
    T,
    std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T&>())>>
    : public std::true_type {};

enum assertion_type { assert, assert_equality_check };

class assertion_failure : public std::exception {
 private:
  std::string expression, func_name, fname;
  bool is_fatal;
  assertion_type type;
  unsigned int line_count;

 public:
  assertion_failure(const std::string&& expression_,
                    const std::string&& func_name_, const std::string&& fname_,
                    unsigned int lc_, bool is_fatal_,
                    assertion_type atype = assertion_type::assert)
      : expression(expression_),
        func_name(func_name_),
        fname(fname_),
        is_fatal(is_fatal_),
        line_count(lc_),
        type(atype) {}

  std::string to_string() const {
    std::stringstream error_stream;
    if (is_fatal) error_stream << "\033[31mFATAL ERROR\033[39m\n";
    error_stream << "-----" << func_name << "----\n";
    error_stream << func_name << " panicked at " << fname << ":" << line_count
                 << "\n";
    if (type == assertion_type::assert) {
      error_stream << "assertion " << expression << " failed\n";
    } else {
      error_stream << expression << "\n";
    }
    return error_stream.str();
  }

  bool get_is_fatal() const { return is_fatal; }
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

    for (size_t i = 0; i < tests.size(); ++i) {
      const test_case* unit_test = tests.at(i);
      auto result = run_individual_test(unit_test);
      out_stream << "\ntest " << unit_test->get_name() << " ... "
                 << (result.has_value() ? failed_str : passed_str);
      if (result.has_value()) {
        const auto failure = result.value();
        failures.push_back(std::move(failure.to_string()));

        if (failure.get_is_fatal()) {
          const auto remaining_tcount = (tests.size() - i - 1);
          out_stream << "\nEncountered a fatal error, aborted the remaining "
                     << remaining_tcount
                     << (remaining_tcount == 1 ? " test\n" : " tests\n");
          break;
        }
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
                     unsigned int lc, bool expression, bool is_fatal) {
    if (!expression) {
      throw assertion_failure(std::move(expression_str), std::move(func_name),
                              std::move(fname), lc, is_fatal);
    }
  }

  template <typename T, typename K>
  static void assert_eq(const T& expr1, const K& expr2,
                        const std::string&& func_name,
                        const std::string&& fname, unsigned int lc,
                        bool expression, bool is_fatal) {
    if (!expression) {
      std::stringstream expression_str;
      expression_str << "Assertion failed\nExpected: " << expr2
                     << "\nActual: " << expr1;
      throw assertion_failure(std::move(expression_str.str()),
                              std::move(func_name), std::move(fname), lc,
                              is_fatal, assertion_type::assert_equality_check);
    }
  }
};

}  // namespace cctest

#endif
