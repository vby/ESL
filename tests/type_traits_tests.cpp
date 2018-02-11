
#include <gtest/gtest.h>
#include <vector>
#include <esl/type_traits.hpp>

TEST(TypeTraitsTest, member_type) {
	EXPECT_FALSE(esl::member_type_type_v<int>);
	EXPECT_TRUE(esl::member_type_type_v<std::true_type>);
	EXPECT_FALSE(esl::member_type_size_type_v<std::true_type>);

	EXPECT_FALSE(esl::member_type_type_v<esl::member_type_type<int>>);
	EXPECT_TRUE((std::is_same_v<esl::member_type_type_t<int, char>, char>));

	EXPECT_FALSE(esl::member_type_size_type_v<std::true_type>);
	EXPECT_TRUE((std::is_same_v<esl::member_type_type_t<std::true_type, int>, std::true_type>));
	EXPECT_TRUE((std::is_same_v<esl::member_type_size_type_t<std::true_type, int>, int>));
}

void type_traits_test_function_a(int, char) {}

class TypeTraitsTestMemberFunctionA {
public:
};
class TypeTraitsTestMemberFunctionB {
public:
	void operator()(int) {}
};
class TypeTraitsTestMemberFunctionC {
public:
	void operator()(int) {}
	int operator()(int, bool) { return 0; }
};

TEST(TypeTraitsTest, member_function) {
	EXPECT_FALSE(esl::has_non_overloaded_operator_parentheses_v<TypeTraitsTestMemberFunctionA>);
	EXPECT_TRUE(esl::has_non_overloaded_operator_parentheses_v<TypeTraitsTestMemberFunctionB>);
	EXPECT_FALSE(esl::has_non_overloaded_operator_parentheses_v<TypeTraitsTestMemberFunctionC>);
}