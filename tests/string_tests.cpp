
#include <gtest/gtest.h>
#include <esl/string.hpp>
#include <vector>
#include <iterator>

TEST(StringViewTest, find) {
	{
		std::string str("adbcdef");
		std::string_view sv(str.data(), str.size());
		const auto pos = sv.find('d', 2);
		ASSERT_EQ(pos, 4);
	}
}

TEST(StringTest, string_split_by_char) {
	{
		std::string str("s1,s23,s345");
		std::vector<std::string_view> ss;
		auto it = esl::string_split(str, ',', std::back_inserter(ss));
		ASSERT_TRUE(ss.size() == 3);
		ASSERT_TRUE((ss[0] == "s1" && ss[1] == "s23" && ss[2] == "s345"));
		esl::string_split(str, ',', it);
		ASSERT_TRUE(ss.size() == 6);
	}
}

TEST(StringTest, string_split_by_substr) {
	{
		std::string str("s1==s23==s345=");
		std::vector<std::string> ss;
		auto it = esl::string_split(str, "==", std::back_inserter(ss));
		ASSERT_TRUE(ss.size() == 3);
		ASSERT_TRUE((ss[0] == "s1" && ss[1] == "s23" && ss[2] == "s345="));
	}
}

