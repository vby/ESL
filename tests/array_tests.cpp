
#include <gtest/gtest.h>
#include <esl/array.hpp>
#include <string>

TEST(ArrayTest, get) {
	esl::multi_array_t<std::string, 2, 5, 3> arr;
	std::get<0, 1, 2>(arr) = "0,1,2";
	ASSERT_EQ(arr[0][1][2], "0,1,2");
}

