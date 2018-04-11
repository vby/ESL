
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

TEST(FuntionalTest, crc32) {
	ASSERT_EQ(esl::crc32("", 0), 0U);
	ASSERT_EQ(esl::crc32("0", 1), 4108050209U);
	ASSERT_EQ(esl::crc32("0123456789", 10), 2793719750U);
	ASSERT_EQ(esl::crc32("ABCDEFGH", 8), 1759295004U);
	ASSERT_EQ(esl::crc32("0123456789ABCDEFGH", 18), esl::crc32("ABCDEFGH", 8, 2793719750U));
}

TEST(FuntionalTest, md5) {
	ASSERT_EQ(esl::md5val(esl::md5("", 0)).to_hex_string(), "d41d8cd98f00b204e9800998ecf8427e");
	ASSERT_EQ(esl::md5val(esl::md5("0123456789", 10)).to_hex_string(), "781e5e245d69b566979b86e28d23f2c7");
	auto s = esl::md5("0123", 4);
	ASSERT_EQ(esl::md5val(esl::md5("456789", 6, s)).to_hex_string(), "781e5e245d69b566979b86e28d23f2c7");
}

