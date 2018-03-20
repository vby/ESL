
#include <gtest/gtest.h>

#include <esl/string.hpp>
#include <esl/iterator.hpp>

#include <vector>
#include <set>
#include <iterator>

TEST(StringTest, constexpr_str_functions) {
	static_assert(esl::constexpr_strlen("") == 0);
	static_assert(esl::constexpr_strlen("abcdefgh") == 8);

	static_assert(esl::constexpr_strcmp("", "") == 0);
	static_assert(esl::constexpr_strcmp("", "a") < 0);
	static_assert(esl::constexpr_strcmp("a", "") > 0);
	static_assert(esl::constexpr_strcmp("abc", "abc") == 0);
	static_assert(esl::constexpr_strcmp("abcd", "abc") > 0);
	static_assert(esl::constexpr_strcmp("abc", "abcd") < 0);

	static_assert(esl::constexpr_strncmp("", "", 0) == 0);
	static_assert(esl::constexpr_strncmp("", "", 2) == 0);
	static_assert(esl::constexpr_strncmp("abc", "abcd", 3) == 0);
	static_assert(esl::constexpr_strncmp("abc", "abcd", 4) < 0);
	static_assert(esl::constexpr_strncmp("abc", "abcd", 5) < 0);
	static_assert(esl::constexpr_strncmp("abcd", "abc", 3) == 0);
	static_assert(esl::constexpr_strncmp("abcd", "abc", 4) > 0);
	static_assert(esl::constexpr_strncmp("abcd", "abc", 5) > 0);
}

TEST(StringTest, make_string) {
	ASSERT_EQ(esl::make_string("hello"), "hello");
	std::string_view s("shello");
	ASSERT_EQ(esl::make_string(s), "shello");
}

TEST(StringTest, make_string_view) {
	ASSERT_EQ(esl::make_string_view("hello"), "hello");
	ASSERT_EQ(esl::make_string_view("hello", 3), "hel");
	std::string s("shello");
	ASSERT_EQ(esl::make_string_view(s), "shello");
	ASSERT_EQ(esl::make_string_view(s.data() + 2, 2), "el");
}

TEST(StringTest, split) {
	{
		std::string str("s1,s23,s345");
		std::vector<std::string_view> ss;
		auto it = esl::split(str, ',', std::back_inserter(ss));
		ASSERT_TRUE(ss.size() == 3);
		ASSERT_TRUE((ss[0] == "s1" && ss[1] == "s23" && ss[2] == "s345"));
		esl::split(str, ',', it);
		ASSERT_TRUE(ss.size() == 6);
	}
	{
		std::string str("a;bc;def;gh");
		std::vector<std::string_view> svec;
		esl::split(str, ';', esl::make_add_iterator(svec));
		ASSERT_EQ(svec.size(), 4);

		std::set<std::string_view> sset;
		esl::split(str, ';', esl::make_add_iterator(sset));
		ASSERT_EQ(sset.size(), 4);
	}
	// split_by_substr
	{
		std::string str("s1==s23==s345=");
		std::vector<std::string> ss;
		esl::split(str, "==", esl::make_cast_iterator(std::back_inserter(ss)));
		ASSERT_TRUE(ss.size() == 3);
		ASSERT_TRUE((ss[0] == "s1" && ss[1] == "s23" && ss[2] == "s345="));
	}
}

TEST(StringTest, join) {
	{
		std::vector<std::string> ss {"s1", "s22", "s333"};
		auto str = esl::join(';', ss.begin(), ss.end());
		ASSERT_EQ(str, "s1;s22;s333");
	}
	{
		std::vector<std::string> ss {"s1", "s22", "s333"};
		auto str = esl::join("::", ss.begin(), ss.end());
		ASSERT_EQ(str, "s1::s22::s333");
	}
}

TEST(StringTest, from_string) {
	auto s0 = esl::make_string_view("");
	auto s00 = esl::make_string_view("-");
	auto s1 = esl::make_string_view("123456789");
	auto s2 = esl::make_string_view("-123456789");
	auto s3 = esl::make_string_view("x123456789");
	auto s4 = esl::make_string_view("12345x6789");
	auto s5 = esl::make_string_view("a1fe0F8B");
	auto s6 = esl::make_string_view("a1fexF8B");
	auto s7 = esl::make_string_view("4123456789");
	auto s8 = esl::make_string_view("5123456789");
	{
		int a = 100;
		auto [errc, ptr] = esl::from_string(s0, a);
		ASSERT_EQ(errc, esl::from_string_errc::invalid_argument);
		ASSERT_EQ(a, 100);
		ASSERT_EQ(ptr, s0.data());
	}
	{
		int a = 100;
		auto [errc, ptr] = esl::from_string(s00, a);
		ASSERT_EQ(errc, esl::from_string_errc::invalid_argument);
		ASSERT_EQ(a, 100);
		ASSERT_EQ(ptr, s00.data() + 1);
	}
	{
		unsigned int a = 0;
		auto [errc, ptr] = esl::from_string(s1, a);
		ASSERT_EQ(errc, esl::from_string_errc::success);
		ASSERT_EQ(a, 123456789);
		ASSERT_EQ(ptr, s1.data() + s1.size());
	}
	{
		int a = 0;
		auto [errc, ptr] = esl::from_string(s2, a);
		ASSERT_EQ(errc, esl::from_string_errc::success);
		ASSERT_EQ(a, -123456789);
		ASSERT_EQ(ptr, s2.data() + s2.size());
	}
	{
		unsigned int a = 100;
		auto [errc, ptr] = esl::from_string(s2, a);
		ASSERT_EQ(errc, esl::from_string_errc::invalid_argument);
		ASSERT_EQ(a, 100);
		ASSERT_EQ(ptr, s2.data());
	}
	{
		int a = 100;
		auto [errc, ptr] = esl::from_string(s3, a);
		ASSERT_EQ(errc, esl::from_string_errc::invalid_argument);
		ASSERT_EQ(a, 100);
		ASSERT_EQ(ptr, s3.data());
	}
	{
		int a = 100;
		auto [errc, ptr] = esl::from_string(s4, a);
		ASSERT_EQ(errc, esl::from_string_errc::success);
		ASSERT_EQ(a, 12345);
		ASSERT_EQ(ptr, s4.data() + 5);
	}
	{
		unsigned int a = 0;
		auto [errc, ptr] = esl::from_string(s5, a, 16);
		ASSERT_EQ(errc, esl::from_string_errc::success);
		ASSERT_EQ(a, 0xa1fe0F8B);
		ASSERT_EQ(ptr, s5.data() + s5.size());
	}
	{
		unsigned int a = 0;
		auto [errc, ptr] = esl::from_string(s6, a, 16);
		ASSERT_EQ(errc, esl::from_string_errc::success);
		ASSERT_EQ(a, 0xa1fe);
		ASSERT_EQ(ptr, s6.data() + 4);
	}
	{
		std::uint32_t a = 100;
		auto [errc, ptr] = esl::from_string(s7, a);
		ASSERT_EQ(errc, esl::from_string_errc::success);
		ASSERT_EQ(a, 4123456789);
		ASSERT_EQ(ptr, s7.data() + s7.size());
	}
	{
		std::uint32_t a = 100;
		auto [errc, ptr] = esl::from_string(s8, a);
		ASSERT_EQ(errc, esl::from_string_errc::result_out_of_range);
		ASSERT_EQ(a, 100);
		ASSERT_EQ(ptr, s8.data() + 9);
	}
}

TEST(StringTest, format) {
	auto s = esl::format("{{hi{}}}", 123);
	ASSERT_EQ(s, "{hi123}");

	s = esl::format("{:{>5}aaa{:4}", 12, 34);
	ASSERT_EQ(s, "{{{12aaa  34");

	s = esl::format("{:{>5}bbb{:}", 12, 34);
	ASSERT_EQ(s, "{{{12bbb34");

	s = esl::format("{:-<5}bbb{2:>#8X}ccc{1:+#}", "hi", 366, 255);
	ASSERT_EQ(s, "hi---bbb    0XFFccc+366");

	s = esl::format("aaa{2}b{1}cc{0}", 111, 2222, 33333);
	ASSERT_EQ(s, "aaa33333b2222cc111");

	s = esl::format("{:08.5f}x{0:.4}x{0:.2}x", 1.23);
	ASSERT_EQ(s, "01.23000x1.23x1.2x");

	// xflags
	s = esl::format("aa{:c}bb", 65);
	ASSERT_EQ(s, "aaAbb");
}

