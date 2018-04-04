
#include <gtest/gtest.h>
#include <esl/any_type.hpp>
#include <cstdint>

struct TypeA;

using AnyTypeA = esl::any_type<TypeA>;
using TypeAPair = std::pair<AnyTypeA, bool>;

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

TEST(AnyTypeTest, small) {
	ASSERT_EQ(a_pair.first.value().a, 0);
	a_pair.first.value().a = 100;
	ASSERT_EQ(a_pair.first.value().a, 100);

	a_pair = TypeAPair{AnyTypeA{10, true}, false};
	ASSERT_EQ(a_pair.first.value().a, 10);
	ASSERT_EQ(a_pair.first.value().b, true);

	a_pair.first.value() = 50;
	ASSERT_EQ(a_pair.first.value().a, 50);
}

