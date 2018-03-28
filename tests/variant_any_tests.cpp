
#include <gtest/gtest.h>
#include <esl/variant_any.hpp>

TEST(VariantAnyTest, comp) {
	esl::variant_any<int, bool> va0;
	esl::variant_any<int, bool> va00;
	esl::variant_any<int, bool> va1(123);
	esl::variant_any<int, bool> va2(123);
	esl::variant_any<int, bool> va3(456);
	esl::variant_any<int, bool> va4(true);
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

