
#include <gtest/gtest.h>
#include <esl/utility.hpp>

using namespace esl::casts;

TEST(UtilityTest, casts) {
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

TEST(UtilityTest, make_overloaded) {
	auto f = esl::make_overloaded(
		[](int) { return 1; },
		[](bool) { return 2; },
		[](const std::string&) { return 3; }
	);
	ASSERT_EQ(f(0), 1);
	ASSERT_EQ(f(true), 2);
	ASSERT_EQ(f(std::string("hello")), 3);
}