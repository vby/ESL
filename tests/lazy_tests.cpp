
#include <gtest/gtest.h>
#include <esl/lazy.hpp>
#include <type_traits>

int class_a_count = 0;
struct ClassA {
	int x;
	std::string s;
	ClassA() {
		x = ++class_a_count;
	}
	ClassA(int x, const std::string& xs): x(x + 1), s(xs) {
		++class_a_count;
	}
	ClassA(int x, std::string&& xs): x(x + 2), s(std::move(xs)) {
		++class_a_count;
	}
	~ClassA() {
		--class_a_count;
	}
	ClassA(const ClassA&) = delete;
	const ClassA& operator=(const ClassA&) = delete;
};


TEST(LazyTest, lazy_without_args) {
	using LazyClassA = esl::lazy<ClassA>;

	ASSERT_FALSE(std::is_copy_constructible_v<LazyClassA> || std::is_copy_assignable_v<LazyClassA>);
	ASSERT_FALSE(std::is_move_constructible_v<LazyClassA> || std::is_move_assignable_v<LazyClassA>);

	ASSERT_EQ(class_a_count, 0);
	{
		LazyClassA lazy_a1;
		ASSERT_EQ(class_a_count, 0);

		// operator&
		ASSERT_EQ((&lazy_a1)->x, 1);
		ASSERT_EQ(class_a_count, 1);

		// address
		ASSERT_EQ(lazy_a1.address()->x, 1);
		ASSERT_EQ(class_a_count, 1);

		// operator reference
		ASSERT_EQ(static_cast<ClassA&>(lazy_a1).x, 1);
		ASSERT_EQ(class_a_count, 1);

		(&lazy_a1)->x = 10;
		ASSERT_EQ((&lazy_a1)->x, 10);
		ASSERT_EQ(class_a_count, 1);

		LazyClassA lazy_a2;
		ASSERT_EQ((&lazy_a2)->x, 2);
		ASSERT_EQ(class_a_count, 2);
	}

	ASSERT_EQ(class_a_count, 0);
}

TEST(LazyTest, lazy_construct_with_args) {
	// exact type of args
	{
		esl::lazy<ClassA, int, const std::string&> lazy_a(10, "a1");
		ASSERT_EQ(class_a_count, 0);
		ASSERT_EQ((&lazy_a)->x, 11);
		ASSERT_EQ((&lazy_a)->s, "a1");
		ASSERT_EQ(class_a_count, 1);
	}
	{
		esl::lazy<ClassA, int, std::string&&> lazy_a(10, "a1");
		ASSERT_EQ(class_a_count, 0);
		ASSERT_EQ((&lazy_a)->x, 12);
		ASSERT_EQ((&lazy_a)->s, "a1");
		ASSERT_EQ(class_a_count, 1);
	}
	// decay type of args
	{
		esl::lazy<ClassA, int, std::string> lazy_a(10, "a1");
		ASSERT_EQ(class_a_count, 0);
		ASSERT_EQ((&lazy_a)->x, 12);
		ASSERT_EQ((&lazy_a)->s, "a1");
		ASSERT_EQ(class_a_count, 1);
	}
}

