
#include <gtest/gtest.h>
#include <esl/endian.hpp>
#include <cstdint>

void test_little_endian() {
	std::uint16_t a = 0xFF;
	ASSERT_EQ(*reinterpret_cast<std::uint8_t*>(&a), 0xFF);
	ASSERT_EQ(*(reinterpret_cast<std::uint8_t*>(&a) + 1), 0);
}

void test_big_endian() {
	std::uint16_t a = 0xFF;
	ASSERT_EQ(*reinterpret_cast<std::uint8_t*>(&a), 0);
	ASSERT_EQ(*(reinterpret_cast<std::uint8_t*>(&a) + 1), 0xFF);
}

TEST(EndianTest, endian) {
	ASSERT_NE(esl::endian::little, esl::endian::big);
	if (esl::endian::native == esl::endian::little) {
		test_little_endian();
	} else if (esl::endian::native == esl::endian::big) {
		test_big_endian();
	} else {
		// mixed endian
		test_little_endian();
		test_big_endian();
	}
}

