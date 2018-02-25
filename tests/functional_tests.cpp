
#include <gtest/gtest.h>
#include <esl/functional.hpp>

TEST(FuntionalTest, hash_value) {
	{
		int a = 100;
		ASSERT_EQ(esl::hash_value(a), std::hash<int>{}(a));
	}
	{
		int a[100];
		ASSERT_NE(esl::hash_value(a, sizeof(a)), 0);
	}
}

TEST(FuntionalTest, hash_combine) {
	{
		std::size_t h = 0;
		std::size_t k = esl::hash_value(100);
		h = esl::hash_value_combine(h, k);
		ASSERT_NE(h, 0);
		ASSERT_NE(h, k);

		auto h1 = esl::hash_combine(h, std::string("hello"));
		ASSERT_NE(h1, h);

		auto h2 = esl::hash_combine(h, std::string("hello2"));
		ASSERT_NE(h2, h);
		ASSERT_NE(h2, h1);
	}
}

TEST(FuntionalTest, hash_pair) {
	{
		std::pair<int, std::string> p { 10, "hello"};
		auto h = esl::hash_value(p);
		ASSERT_NE(h, 0);

		std::pair<int, std::string> p2 { 10, "hello2"};
		auto h2 = esl::hash_value(p2);
		ASSERT_NE(h2, 0);
		ASSERT_NE(h2, h);
	}
}

TEST(FuntionalTest, hash_tuple) {
	{
		std::tuple<> tup;
		auto h = esl::hash_value(tup);
		ASSERT_EQ(h, 0);

		std::tuple<int, std::string, bool> tup1 { 10, "hello", false };
		auto h1 = esl::hash_value(tup1);
		ASSERT_NE(h1, 0);
		ASSERT_NE(h1, h);

		std::tuple<int, std::string, bool> tup2 { 10, "hello2", false };
		auto h2 = esl::hash_value(tup2);
		ASSERT_NE(h2, 0);
		ASSERT_NE(h2, h);
		ASSERT_NE(h2, h1);
	}
}

