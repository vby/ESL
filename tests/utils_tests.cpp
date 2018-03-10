
#include <gtest/gtest.h>
#include <esl/utils.hpp>

using namespace esl::casts;

TEST(UtilsTest, casts) {
	struct A {
		int a;
	};
	struct B: public A {
		B(int a, char b): A{a}, b(b) {}
		char b;
	};
	struct C {
		int a;
	};
	struct D {
		int a;
		char b;
	};

	{
		B b{1, 2};
		A& a = implicit_cast<A&>(b);
		ASSERT_EQ(a.a, 1);
	}

	{
		int a = 5;
		char c = narrow_cast<char>(a);
		ASSERT_EQ(a, c);
	}

	{
		A a{1};
		C c = bit_cast<C>(a);
		ASSERT_EQ(c.a, a.a);
	}
}

