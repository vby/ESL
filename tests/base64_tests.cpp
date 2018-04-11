
#include <gtest/gtest.h>
#include <esl/base64.hpp>

TEST(Base64Test, encode) {
	// no pads
	{
		auto sp = esl::base64_encode("012345678", 9);
		ASSERT_EQ(sp, "MDEyMzQ1Njc4");

		auto s = esl::base64_encode("012345678", 9, esl::base64_std_npad);
		ASSERT_EQ(s, "MDEyMzQ1Njc4");
	}

	// one pad
	{
		auto sp = esl::base64_encode("01234567", 8);
		ASSERT_EQ(sp, "MDEyMzQ1Njc=");

		auto s = esl::base64_encode("01234567", 8, esl::base64_std_npad);
		ASSERT_EQ(s, "MDEyMzQ1Njc");
	}

	// two pads
	{
		auto sp = esl::base64_encode("0123456789", 10);
		ASSERT_EQ(sp, "MDEyMzQ1Njc4OQ==");

		auto s = esl::base64_encode("0123456789", 10, esl::base64_std_npad);
		ASSERT_EQ(s, "MDEyMzQ1Njc4OQ");
	}
}

TEST(Base64Test, decode) {
	// no pads
	{
		std::string_view sv("MDEyMzQ1Njc4");
		auto d = esl::base64_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "012345678");
	}

	// one pad
	{
		std::string_view svp("MDEyMzQ1Njc=");
		auto dp = esl::base64_decode(svp.data(), svp.size());
		ASSERT_EQ(dp, "01234567");

		std::string_view sv("MDEyMzQ1Njc");
		auto d = esl::base64_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "01234567");
	}

	// two pads
	{
		std::string_view svp("MDEyMzQ1Njc4OQ==");
		auto dp = esl::base64_decode(svp.data(), svp.size());
		ASSERT_EQ(dp, "0123456789");

		std::string_view sv("MDEyMzQ1Njc4OQ");
		auto d = esl::base64_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "0123456789");
	}

	// concated
	{
		std::string_view sv("MDEyMzQ1Njc4OQ==MDEyMzQ1Njc=MDEyMzQ1Njc4OQ");
		auto d = esl::base64_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "0123456789012345670123456789");
	}

	// error padding
	{
		std::string_view sv("MDEyMzQ1Njc4=");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "012345678");
		ASSERT_EQ(pos, sv.size() - 1);
	}
	{
		std::string_view sv("MDEyMzQ1Njc4OQ=");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size());
	}
	{
		std::string_view sv("MDEyMzQ1Njc4OQ===");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size() - 1);
	}

	// error chars - X---
	{
		std::string_view sv("MDEyMzQ1Njc4?aa");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "012345678");
		ASSERT_EQ(pos, sv.size() - 3);
	}

	// error chars - aX--
	{
		std::string_view sv("MDEyMzQ1Njc4b<cc");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "012345678");
		ASSERT_EQ(pos, sv.size() - 3);
	}

	// error chars - abX-
	{
		std::string_view sv("MDEyMzQ1Njc4OQ>ddd");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size() - 4);
	}

	// error chars - abcX
	{
		std::string_view sv("MDEyMzQ1Njc>e");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "01234567");
		ASSERT_EQ(pos, sv.size() - 2);
	}

	// error chars - ab=X
	{
		std::string_view sv("MDEyMzQ1Njc4OQ=ff");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size() - 2);
	}

	// incomplete (2 to 1)
	{
		std::string_view sv("MDEyMzQ1Njc4O");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "012345678");
		ASSERT_EQ(pos, sv.size() + 1);
	}

	// incomplete (3 to 2)
	{
		std::string_view sv("MDEyMzQ1Njc4OR");
		const auto& [d, pos] = esl::base64_try_decode(sv.data(), sv.size());
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size() + 1);
	}

}

