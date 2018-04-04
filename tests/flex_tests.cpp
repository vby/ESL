
#include <gtest/gtest.h>
#include <esl/flex.hpp>
#include <cstdint>

struct TypeA;

using FlexTypeA = esl::flex<TypeA>;
using TypeAPair = std::pair<FlexTypeA, bool>;

TypeAPair a_pair;

struct TypeA {
	int a;
	bool b;
	TypeA(): a(0), b(false) {}
	TypeA(int a, bool b): a(a), b(b) {}

	TypeA& operator=(int a) {
		this->a = a;
		return *this;
	}
};

TEST(FlexTypeTest, small) {
	ASSERT_EQ(a_pair.first.value().a, 0);
	a_pair.first.value().a = 100;
	ASSERT_EQ(a_pair.first.value().a, 100);

	a_pair = TypeAPair{FlexTypeA{10, true}, false};
	ASSERT_EQ(a_pair.first.value().a, 10);
	ASSERT_EQ(a_pair.first.value().b, true);

	a_pair.first.value() = 50;
	ASSERT_EQ(a_pair.first.value().a, 50);
}

