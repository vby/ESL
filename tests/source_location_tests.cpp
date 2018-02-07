
#include <gtest/gtest.h>
#include <esl/source_location.hpp>

#ifdef ESL_HAS_SOURCE_LOCATION
std::string source_location_test_func1(const esl::source_location& sl = esl::source_location::current()) {
	return sl.function_name();
}

std::string source_location_test_func1_caller() {
	return source_location_test_func1();
}

TEST(SourceLocationTest, current) {
	{
		auto func_name = source_location_test_func1_caller();
		ASSERT_EQ(func_name, "source_location_test_func1_caller");
	}
}

#endif

std::string source_location_test_func2(const esl::source_location& sl) {
	return sl.function_name();
}

std::string source_location_test_func2_caller() {
	return source_location_test_func2(ESL_SOURCE_LOCATION_CURRENT());
}

TEST(SourceLocationTest, current_by_macros) {
	{
		auto func_name = source_location_test_func2_caller();
		ASSERT_EQ(func_name, "source_location_test_func2_caller");
	}
}
