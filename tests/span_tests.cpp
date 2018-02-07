
#include <gtest/gtest.h>
#include <esl/span.hpp>

//#define ESL_COMPILATION_ERROR

TEST(SpanTest, default_constructor) {
	{
		esl::span<int> s;
		ASSERT_TRUE((s.size() == 0 && s.empty()));

		esl::span<const int> cs;
		ASSERT_TRUE((cs.size() == 0 && cs.empty()));
	}
	{
		esl::span<int, 0> s;
		ASSERT_TRUE((s.size() == 0 && s.empty()));

		esl::span<const int, 0> cs;
		ASSERT_TRUE((cs.size() == 0 && cs.empty()));
	}
#ifdef ESL_COMPILATION_ERROR
	{
		esl::span<int, 1> s;
		esl::span<const int, 1> cs;
	}
#endif
}

TEST(SpanTest, constructor) {
	int arr[100];
#ifdef ESL_COMPILATION_ERROR
	{
		esl::span<int, 5> s(arr);
	}
#endif
	{
		esl::span<int> s(arr, 0);
		ASSERT_TRUE((s.size() == 0 && s.data() == arr && s.empty()));
	}
	{
		esl::span<int> s(arr, 5);
		ASSERT_TRUE((s.size() == 5 && s.data() == arr && !s.empty()));
	}
}

TEST(SpanTest, fixed_span_constructor) {
	int arr[100];
	{
		esl::span<int, 0> s(arr);
		ASSERT_TRUE((s.size() == 0 && s.data() == arr && s.empty()));
	}
	{
		esl::span<int, 5> s(arr);
		ASSERT_TRUE((s.size() == 5 && s.data() == arr && !s.empty()));
	}
#ifdef ESL_COMPILATION_ERROR
	{
		esl::span<int, 5> s(arr, 10);
	}
#endif
}

TEST(SpanTest, span_cast) { 
	int arr[100];
	{
		esl::span<int, 10> sf(arr);

		// fix -> fix 
		esl::span<int, 10> sf2(sf);
		ASSERT_TRUE(sf2.size() == 10);

		// fix -> fix (change type)
		esl::span<const int, 10> sf3(sf);
		ASSERT_TRUE(sf3.size() == 10);

#ifdef ESL_COMPILATION_ERROR
		// fix -> fix (change size)
		esl::span<int, 5> sf4(sf);
#endif

		// fix -> dyn
		esl::span<int> s(sf);
		ASSERT_TRUE(s.size() == 10);

		// fix -> dyn (change type)
		esl::span<const int> s2(s);
		ASSERT_TRUE(s2.size() == 10);

		// dyn -> dyn
		esl::span<int> s3(s);
		ASSERT_TRUE(s3.size() == 10);

		// dyn -> dyn (change type)
		esl::span<const int> s4(s);
		ASSERT_TRUE(s4.size() == 10);

#ifdef ESL_COMPILATION_ERROR
		// dyn -> fix
		esl::span<int, 5> s5(s);
#endif

	}
}

TEST(SpanTest, size_optimization) {
	ASSERT_TRUE(sizeof(esl::span<int>) == sizeof(int*) + sizeof(std::size_t));
	ASSERT_TRUE(sizeof(esl::span<int, 0>) == sizeof(int*));
	ASSERT_TRUE(sizeof(esl::span<int, 10>) == sizeof(int*));
}

template <class Span>
void test_span_subspan(const Span& s) {
	{
		auto ss = s.subspan(0);
		ASSERT_TRUE(ss.size() == 100);
	}
	{
		auto ss = s.subspan(10);
		ASSERT_TRUE(ss.size() == 90);
	}
	{
		auto ss = s.subspan(100);
		ASSERT_TRUE(ss.size() == 0);
	}
	{
		ASSERT_THROW(s.subspan(101), std::out_of_range);
	}
	{
		auto ss = s.subspan(0, 10);
		ASSERT_TRUE(ss.size() == 10);
	}
	{
		auto ss = s.subspan(0, 110);
		ASSERT_TRUE(ss.size() == 100);
	}
	{
		auto ss = s.subspan(90, 5);
		ASSERT_TRUE(ss.size() == 5);
	}
	{
		auto ss = s.subspan(90, 10);
		ASSERT_TRUE(ss.size() == 10);
	}
	{
		auto ss = s.subspan(90, 15);
		ASSERT_TRUE(ss.size() == 10);
	}
	{
		auto ss = s.subspan(100, 5);
		ASSERT_TRUE(ss.size() == 0);
	}
	{
		ASSERT_THROW(s.subspan(101, 0), std::out_of_range);
	}
	{
		ASSERT_THROW(s.subspan(101, 10), std::out_of_range);
	}
}

TEST(SpanTest, subspan) {
	std::array<int, 100> arr;
	esl::span<int> s(arr.data(), arr.size());
	test_span_subspan(s);
}

TEST(SpanTest, fixed_span_subspan) {
	std::array<int, 100> arr;
	esl::span<int, 100> s(arr.data());
	test_span_subspan(s);
}

