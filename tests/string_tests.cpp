
#include <gtest/gtest.h>

#include <esl/string.hpp>
#include <esl/iterator.hpp>

#include <vector>
#include <iterator>

TEST(StringViewTest, substr) {
	{
		std::string str("adbcdef");
		std::string_view sv(str.data(), str.size());
		auto ss = esl::substr(str, 1, 3);
		ASSERT_EQ(ss.size(), 3);
		ASSERT_EQ(ss[0], 'd');
	}
}

TEST(StringTest, split_by_char) {
	{
		std::string str("s1,s23,s345");
		std::vector<std::string_view> ss;
		auto it = esl::split(str, ',', std::back_inserter(ss));
		ASSERT_TRUE(ss.size() == 3);
		ASSERT_TRUE((ss[0] == "s1" && ss[1] == "s23" && ss[2] == "s345"));
		esl::split(str, ',', it);
		ASSERT_TRUE(ss.size() == 6);
	}
}

TEST(StringTest, split_by_substr) {
	{
		std::string str("s1==s23==s345=");
		std::vector<std::string> ss;
		auto it = esl::split(str, "==", esl::make_cast_iterator(std::back_inserter(ss)));
		ASSERT_TRUE(ss.size() == 3);
		ASSERT_TRUE((ss[0] == "s1" && ss[1] == "s23" && ss[2] == "s345="));
	}
}

TEST(StringTest, join) {
	{
		std::vector<std::string> ss {"s1", "s22", "s333"};
		auto str = esl::join("::", ss.begin(), ss.end());
		ASSERT_EQ(str, "s1::s22::s333");
	}
}

