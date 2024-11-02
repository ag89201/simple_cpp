#pragma once

#include <cassert>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

constexpr auto RED_TEXT_START = "\033[1;31m";
constexpr auto BLUE_TEXT_START = "\033[1;36m";
constexpr auto TEXT_COLOR_RESET = "\033[0m";

#define SIZEOF(T)             \
	(std::cout << #T << ":\t" \
	           << "sizeof = " << sizeof(T) << '\t' << "alignof = " << alignof(T) << endl)

namespace simple_unit_test {

	enum class ASSERT_TYPE { _ASSERT_ = 243, _ASSERT_EQUAL_ };

	template <typename T, typename U>
	void AssertEqualImpl(const T& t,
	                     const U& u,
	                     const std::string& t_str,
	                     const std::string& u_str,
	                     const std::string& file,
	                     const std::string& func,
	                     unsigned line,
	                     const std::string& hint,
	                     ASSERT_TYPE assert_type) {
		if (t != u) {
			std::stringstream out;
			out << std::boolalpha;
			if (assert_type == ASSERT_TYPE::_ASSERT_EQUAL_) {
				out << file << "(" << line << "): " << func << ": ";
				out << "ASSERT_EQUAL"
				    << "(" << t_str << ", " << u_str << ") failed: ";
				out << t << " != " << u << ".";
			} else if (assert_type == ASSERT_TYPE::_ASSERT_) {
				out << file << "(" << line << "): " << func << ": ";
				out << "ASSERT(" << t_str << ") failed.";
			}
			if (!hint.empty()) {
				out << " Hint: " << hint;
			}
			throw std::runtime_error(out.str());
		}
	}

#define TEST_FUNC void

#define ASSERT_EQUAL(a, b)                                                  \
	AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, "", \
	                simple_unit_test::ASSERT_TYPE::_ASSERT_EQUAL_)

#define ASSERT_EQUAL_HINT(a, b, hint)                                           \
	AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint), \
	                simple_unit_test::ASSERT_TYPE::_ASSERT_EQUAL_)

#define ASSERT(expr)                                                                     \
	AssertEqualImpl((expr), (true), #expr, "true", __FILE__, __FUNCTION__, __LINE__, "", \
	                simple_unit_test::ASSERT_TYPE::_ASSERT_)

#define ASSERT_HINT(expr, hint)                                                              \
	AssertEqualImpl((expr), (true), #expr, "true", __FILE__, __FUNCTION__, __LINE__, (hint), \
	                simple_unit_test::ASSERT_TYPE::_ASSERT_)

	template <class TestFunction>
	void RunTestImpl(TestFunction func, const std::string& test_name) {
		try {
			func();
			std::cerr << BLUE_TEXT_START << test_name << " OK" << TEXT_COLOR_RESET << std::endl;
		} catch (const std::exception& e) {
			std::cerr << RED_TEXT_START << test_name << " fail: " << e.what() << TEXT_COLOR_RESET
			          << std::endl;
		} catch (...) {
			std::cerr << RED_TEXT_START << "Unknown exception caught" << TEXT_COLOR_RESET << std::endl;
		}
	}

#define RUN_TEST(func) RunTestImpl(func, #func)

	class TestRunner {
	   public:
		template <class TestFunc>
		void RunTest(TestFunc func, const std::string& test_name) {
			try {
				func();
				++ok_count;
				std::cerr << BLUE_TEXT_START << test_name << " OK" << TEXT_COLOR_RESET << std::endl;
			} catch (const std::exception& e) {
				++fail_count;
				std::cerr << RED_TEXT_START << test_name << " fail: " << e.what()
				          << TEXT_COLOR_RESET << std::endl;

			} catch (...) {
				++fail_count;
				std::cerr << RED_TEXT_START << "Unknown exception caught" << TEXT_COLOR_RESET
				          << std::endl;
			}
		}
		~TestRunner() {
			if (fail_count > 0) {
				std::cerr << BLUE_TEXT_START << ok_count << " unit tests successfully completed"
				          << TEXT_COLOR_RESET << std::endl;
				std::cerr << RED_TEXT_START << fail_count << " unit tests terminate failed"
				          << TEXT_COLOR_RESET << std::endl;
				// exit(1);
			}
		}

	   private:
		int fail_count = 0;
		int ok_count = 0;
	};
}  // namespace simple_unit_test
namespace sut = simple_unit_test;
/*example*/
//    simple_unit_test::TestRunner runner;

//    runner.RunTest(ExampleTest, "ExampleTest");
//    runner.RunTest(ExampleTest2, "ExampleTest2");
//    runner.RunTest(ExampleTest3, "ExampleTest3");

// simple_unit_test::RUN_TEST(ExampleTest);
// simple_unit_test::RUN_TEST(ExampleTest2);
// simple_unit_test::RUN_TEST(ExampleTest3);