
#include <gtest/gtest.h>
#include <esl/codecs.hpp>

TEST(CodecsBase64Test, encode) {
	// no pads
	{
		auto sp = esl::base64::encode("012345678", 9);
		ASSERT_EQ(sp, "MDEyMzQ1Njc4");

		auto s = esl::base64::encode("012345678", 9, false);
		ASSERT_EQ(s, "MDEyMzQ1Njc4");
	}

	// one pad
	{
		auto sp = esl::base64::encode("01234567", 8);
		ASSERT_EQ(sp, "MDEyMzQ1Njc=");

		auto s = esl::base64::encode("01234567", 8, false);
		ASSERT_EQ(s, "MDEyMzQ1Njc");
	}

	// two pads
	{
		auto sp = esl::base64::encode("0123456789", 10);
		ASSERT_EQ(sp, "MDEyMzQ1Njc4OQ==");

		auto s = esl::base64::encode("0123456789", 10, false);
		ASSERT_EQ(s, "MDEyMzQ1Njc4OQ");
	}
}

TEST(CodecsBase64Test, decode) {
	// no pads
	{
		std::string_view sv("MDEyMzQ1Njc4");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "012345678");
		ASSERT_EQ(pos, sv.size());
	}

	// one pad
	{
		std::string_view svp("MDEyMzQ1Njc=");
		const auto& [dp, posp] = esl::base64::decode(svp);
		ASSERT_EQ(dp, "01234567");
		ASSERT_EQ(posp, svp.size());

		std::string_view sv("MDEyMzQ1Njc");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "01234567");
		ASSERT_EQ(pos, sv.size());
	}

	// two pads
	{
		std::string_view svp("MDEyMzQ1Njc4OQ==");
		const auto& [dp, posp] = esl::base64::decode(svp);
		ASSERT_EQ(dp, "0123456789");
		ASSERT_EQ(posp, svp.size());

		std::string_view sv("MDEyMzQ1Njc4OQ");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size());
	}

	// concated
	{
		std::string_view sv("MDEyMzQ1Njc4OQ==MDEyMzQ1Njc=MDEyMzQ1Njc4OQ");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "0123456789012345670123456789");
		ASSERT_EQ(pos, sv.size());
	}

	// error padding
	{
		std::string_view sv("MDEyMzQ1Njc4=");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "012345678");
		ASSERT_EQ(pos, sv.size() - 1);
	}
	{
		std::string_view sv("MDEyMzQ1Njc4OQ=");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size());
	}
	{
		std::string_view sv("MDEyMzQ1Njc4OQ===");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size() - 1);
	}

	// error chars - X---
	{
		std::string_view sv("MDEyMzQ1Njc4?aa");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "012345678");
		ASSERT_EQ(pos, sv.size() - 3);
	}

	// error chars - aX--
	{
		std::string_view sv("MDEyMzQ1Njc4b<cc");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "012345678");
		ASSERT_EQ(pos, sv.size() - 3);
	}

	// error chars - abX-
	{
		std::string_view sv("MDEyMzQ1Njc4OQ>ddd");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size() - 4);
	}

	// error chars - abcX
	{
		std::string_view sv("MDEyMzQ1Njc>e");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "01234567");
		ASSERT_EQ(pos, sv.size() - 2);
	}

	// error chars - ab=X
	{
		std::string_view sv("MDEyMzQ1Njc4OQ=ff");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size() - 2);
	}

	// error chars sequence
	{
		std::string_view sv("MDEyMzQ1Njc4OR");
		const auto& [d, pos] = esl::base64::decode(sv);
		ASSERT_EQ(d, "0123456789");
		ASSERT_EQ(pos, sv.size() - 1);
	}
}

