
#include <gtest/gtest.h>
#include <esl/unicode.hpp>
#include <type_traits>

TEST(UnicodeTest, plane_at) {
	// at code point
	ASSERT_EQ(esl::plane_at(0), esl::unicode_plane::basic_multilingual);
	ASSERT_EQ(esl::plane_at(0xFFFF), esl::unicode_plane::basic_multilingual);
	ASSERT_EQ(esl::plane_at(0x10000), esl::unicode_plane::supplementary_multilingual);
	ASSERT_EQ(esl::plane_at(0x1FFFF), esl::unicode_plane::supplementary_multilingual);
	ASSERT_EQ(esl::plane_at(0x110000), esl::unicode_plane::no_plane);

	// at block
	ASSERT_EQ(esl::plane_at(esl::unicode_block::basic_latin), esl::unicode_plane::basic_multilingual);
	ASSERT_EQ(esl::plane_at(esl::unicode_block::tags), esl::unicode_plane::supplementary_special_purpose);
}

TEST(UnicodeTest, block_at) {
	ASSERT_EQ(esl::block_at(0), esl::unicode_block::basic_latin);
	ASSERT_EQ(esl::block_at(0x7F), esl::unicode_block::basic_latin);
	ASSERT_EQ(esl::block_at(0x80), esl::unicode_block::latin_1_supplement);
	ASSERT_EQ(esl::block_at(0x110000), esl::unicode_block::no_block);
	ASSERT_EQ(esl::block_at(0xE007F), esl::unicode_block::tags);
	ASSERT_EQ(esl::block_at(0xE008F), esl::unicode_block::no_block);
}

TEST(UnicodeTest, max_min_code_point) {
	// plane
	ASSERT_EQ(esl::min_code_point(esl::unicode_plane::no_plane), esl::no_code_point);
	ASSERT_EQ(esl::max_code_point(esl::unicode_plane::no_plane), esl::no_code_point);

	ASSERT_EQ(esl::min_code_point(esl::unicode_plane::basic_multilingual), 0);
	ASSERT_EQ(esl::max_code_point(esl::unicode_plane::basic_multilingual), 0xFFFF);

	// block
	ASSERT_EQ(esl::min_code_point(esl::unicode_block::no_block), esl::no_code_point);
	ASSERT_EQ(esl::max_code_point(esl::unicode_block::no_block), esl::no_code_point);

	ASSERT_EQ(esl::min_code_point(esl::unicode_block::basic_latin), 0);
	ASSERT_EQ(esl::max_code_point(esl::unicode_block::basic_latin), 0x7F);
}

