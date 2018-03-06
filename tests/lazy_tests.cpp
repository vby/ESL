
#include <gtest/gtest.h>
#include <esl/lazy.hpp>
#include <type_traits>

int class_a_count = 0;
struct ClassA {
	int x;
	ClassA() {
		x = ++class_a_count;
	}
	~ClassA() {
		--class_a_count;
	}
};

TEST(LazyTest, lazy) {

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

