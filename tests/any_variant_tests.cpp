
#include <gtest/gtest.h>
#include <esl/any_variant.hpp>
#include <string>

TEST(AnyVariantTest, default_construct) {
	esl::any_variant<std::string, bool> v;
	ASSERT_EQ(v.index(), 0);
	ASSERT_FALSE(v.valueless_by_exception());
}

TEST(AnyVariantTest, construct) {
	{
		esl::any_variant<int, bool, std::string> va;
		ASSERT_FALSE(va.valueless_by_exception());
		ASSERT_EQ(va.index(), 0);
	}
	{
		esl::any_variant<int, bool, std::string> va(123);
		ASSERT_EQ(va.index(), 0);
		va = false;
		ASSERT_EQ(va.index(), 1);
		va = std::string("12345");
		ASSERT_EQ(va.index(), 2);
		va = "12345";
		ASSERT_EQ(va.index(), 1);
		//va = 1.23;
		va = true;
		ASSERT_EQ(va.index(), 1);
	}
}

TEST(AnyVariantTest, get) {
	esl::any_variant<int, bool, std::string> v(false);
	ASSERT_THROW(std::get<int>(v), std::bad_variant_access);
	bool&& rb = std::get<bool>(std::move(v));
	ASSERT_EQ(rb, false);
}

TEST(AnyVariantTest, get_if) {
	esl::any_variant<int, bool, std::string> v(std::string("hello"));
	int* ip = std::get_if<int>(&v);
	ASSERT_EQ(ip, nullptr);
	std::string* sp = std::get_if<std::string>(&v);
	ASSERT_NE(sp, nullptr);
}

TEST(AnyVariantTest, visit) {
	esl::any_variant<int, bool, std::string> v1(false);
	esl::any_variant<float, std::string> v2(std::string("hello"));
	auto f = [](auto&& arg, auto&& arg2) -> std::size_t {
			return esl::index_of_v<std::remove_reference_t<decltype(arg)>, int, bool, std::string>
				+ esl::index_of_v<std::remove_reference_t<decltype(arg2)>, float, std::string>;
		};

	auto index = esl::visit(f, v1, v2);
	ASSERT_EQ(index, 2);

	v1.emplace<0>(123);
	index = esl::visit(f, v1, v2);
	ASSERT_EQ(index, 1);

	v2.emplace<float>(1.0);
	index = esl::visit(f, v1, v2);
	ASSERT_EQ(index, 0);
}

TEST(AnyVariantTest, hash) {
	using va_type = esl::any_variant<int, bool, int>;

	va_type va(std::in_place_index<0>, 100);
	auto h1 = esl::hash_value(va);
	ASSERT_NE(h1, 100);

	va.emplace<2>(100);
	auto h2 = esl::hash_value(va);
	ASSERT_NE(h2, 100);
	ASSERT_NE(h2, h1);

	va.emplace<1>(101);
	auto h3 = esl::hash_value(va);
	ASSERT_NE(h3, h1);
	ASSERT_NE(h3, h2);

	va.emplace<2>(100);
	auto h4 = esl::hash_value(va);
	ASSERT_EQ(h4, h2);
}

TEST(AnyVariantTest, comp) {
	esl::any_variant<int, bool> va0;
	esl::any_variant<int, bool> va00;
	esl::any_variant<int, bool> va1(123);
	esl::any_variant<int, bool> va2(123);
	esl::any_variant<int, bool> va3(456);
	esl::any_variant<int, bool> va4(true);
	ASSERT_EQ(va0, va0);
	ASSERT_EQ(va0, va00);
	ASSERT_FALSE(va0 != va00);
	ASSERT_NE(va0, va1);
	ASSERT_NE(va0, va2);
	ASSERT_NE(va0, va3);
	ASSERT_NE(va0, va4);
	ASSERT_LT(va0, va1);
	ASSERT_LE(va0, va1);
	ASSERT_GT(va1, va0);
	ASSERT_GE(va1, va0);

	ASSERT_EQ(va1, va1);
	ASSERT_EQ(va1, va2);
	ASSERT_GE(va1, va2);
	ASSERT_LE(va1, va2);
	ASSERT_GE(va2, va1);
	ASSERT_LE(va2, va1);

	ASSERT_FALSE(va1 == va3);
	ASSERT_NE(va1, va3);
	ASSERT_LT(va1, va3);
	ASSERT_LE(va1, va3);
	ASSERT_GT(va3, va1);
	ASSERT_GE(va3, va1);

	ASSERT_FALSE(va1 == va4);
	ASSERT_NE(va1, va4);
	ASSERT_LT(va1, va4);
	ASSERT_LE(va1, va4);
	ASSERT_GT(va4, va1);
	ASSERT_GE(va4, va1);
}

