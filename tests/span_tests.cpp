
#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>
#include <esl/span.hpp>

//#define ESL_COMPILATION_ERROR

TEST_CASE("default_constructor") {
	{
		esl::span<int> s;
		CHECK((s.size() == 0 && s.empty()));

		esl::span<const int> cs;
		CHECK((cs.size() == 0 && cs.empty()));
	}
	{
		esl::span<int, 0> s;
		CHECK((s.size() == 0 && s.empty()));

		esl::span<const int, 0> cs;
		CHECK((cs.size() == 0 && cs.empty()));
	}
#ifdef ESL_COMPILATION_ERROR
	{
		esl::span<int, 1> s;
		esl::span<const int, 1> cs;
	}
#endif
}

TEST_CASE("constructor") {
	int arr[100];
#ifdef ESL_COMPILATION_ERROR
	{
		esl::span<int, 5> s(arr);
	}
#endif
	{
		esl::span<int> s(arr, 0);
		CHECK((s.size() == 0 && s.data() == arr && s.empty()));
	}
	{
		esl::span<int> s(arr, 5);
		CHECK((s.size() == 5 && s.data() == arr && !s.empty()));
	}
}

TEST_CASE("fixed_span_constructor") {
	int arr[100];
	{
		esl::span<int, 0> s(arr);
		CHECK((s.size() == 0 && s.data() == arr && s.empty()));
	}
	{
		esl::span<int, 5> s(arr);
		CHECK((s.size() == 5 && s.data() == arr && !s.empty()));
	}
#ifdef ESL_COMPILATION_ERROR
	{
		esl::span<int, 5> s(arr, 10);
	}
#endif
}

TEST_CASE("size_optimization") {
	CHECK(sizeof(esl::span<int>) == sizeof(int*) + sizeof(std::size_t));
	CHECK(sizeof(esl::span<int, 0>) == sizeof(int*));
	CHECK(sizeof(esl::span<int, 10>) == sizeof(int*));
}

template <class Span>
void test_span_subspan(const Span& s) {
	{
		auto ss = s.subspan(0);
		CHECK(ss.size() == 100);
	}
	{
		auto ss = s.subspan(10);
		CHECK(ss.size() == 90);
	}
	{
		auto ss = s.subspan(100);
		CHECK(ss.size() == 0);
	}
	{
		CHECK_THROWS_AS(s.subspan(101), std::out_of_range);
	}
	{
		auto ss = s.subspan(0, 10);
		CHECK(ss.size() == 10);
	}
	{
		auto ss = s.subspan(0, 110);
		CHECK(ss.size() == 100);
	}
	{
		auto ss = s.subspan(90, 5);
		CHECK(ss.size() == 5);
	}
	{
		auto ss = s.subspan(90, 10);
		CHECK(ss.size() == 10);
	}
	{
		auto ss = s.subspan(90, 15);
		CHECK(ss.size() == 10);
	}
	{
		auto ss = s.subspan(100, 5);
		CHECK(ss.size() == 0);
	}
	{
		CHECK_THROWS_AS(s.subspan(101, 0), std::out_of_range);
	}
	{
		CHECK_THROWS_AS(s.subspan(101, 10), std::out_of_range);
	}
}

TEST_CASE("subspan") {
	std::array<int, 100> arr;
	esl::span<int> s(arr.data(), arr.size());
	test_span_subspan(s);
}

TEST_CASE("fixed_span_subspan") {
	std::array<int, 100> arr;
	esl::span<int, 100> s(arr.data());
	test_span_subspan(s);
}

