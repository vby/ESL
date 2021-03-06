
#include <gtest/gtest.h>
#include <vector>
#include <esl/type_traits.hpp>

TEST(TypeTraitsTest, has_member_type) {
	EXPECT_FALSE(esl::has_member_type_type_v<int>);
	EXPECT_TRUE(esl::has_member_type_type_v<std::true_type>);
	EXPECT_FALSE(esl::has_member_type_size_type_v<std::true_type>);
	EXPECT_TRUE(esl::has_member_type_type_v<esl::has_member_type_type<int>>);
	EXPECT_FALSE(esl::has_member_type_size_type_v<std::true_type>);
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

int function_test_fun0(int, std::string&);
int function_test_fun1(bool) noexcept;

class FuncTestClass {
public:
	bool func0(int) const;
	bool func1(int) &;
	bool func2(int) && noexcept;
};

TEST(TypeTraitsTest, function_traits) {
	{
		using traits = esl::function_traits<int(bool, char)>;
		EXPECT_TRUE((std::is_same_v<traits::function_type, int(bool, char)>));
		EXPECT_TRUE((std::is_same_v<traits::result_type, int>));
		EXPECT_TRUE((std::is_same_v<traits::args_tuple, std::tuple<bool, char>>));
		EXPECT_FALSE(traits::is_noexcept::value);
	}
	{
		using traits = esl::function_traits<void(bool, char) noexcept>;
		EXPECT_TRUE((std::is_same_v<traits::function_type, void(bool, char) noexcept>));
		EXPECT_TRUE((std::is_same_v<traits::result_type, void>));
		EXPECT_TRUE((std::is_same_v<traits::args_tuple, std::tuple<bool, char>>));
		EXPECT_TRUE(traits::is_noexcept::value);
	}
	{
		using traits = esl::function_traits<decltype(function_test_fun0)>;
		static_assert(!std::is_const_v<traits::specifier>);
		static_assert(!esl::is_noexcept_v<decltype(function_test_fun0)>);
		static_assert(esl::is_noexcept_v<decltype(function_test_fun1)>);
	}
	{
		using traits = esl::function_traits<decltype(&FuncTestClass::func0)>;
		static_assert(std::is_const_v<traits::specifier>);
		static_assert(!std::is_const_v<esl::function_traits<decltype(&FuncTestClass::func1)>::specifier>);
		static_assert(std::is_lvalue_reference_v<esl::function_traits<decltype(&FuncTestClass::func1)>::specifier>);
		static_assert(!esl::is_noexcept_v<decltype(&FuncTestClass::func0)>);
		static_assert(!esl::is_noexcept_v<decltype(&FuncTestClass::func1)>);
		static_assert(esl::is_noexcept_v<decltype(&FuncTestClass::func2)>);
	}
}
