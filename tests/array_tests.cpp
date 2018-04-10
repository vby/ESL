
#include <gtest/gtest.h>
#include <esl/array.hpp>
#include <string>
#include <numeric>

TEST(ArrayTest, md_array) {
	static_assert(std::is_same_v<esl::md_array_t<int>, int>);
	static_assert(std::is_same_v<esl::md_array_t<int, 2>, std::array<int, 2>>);
	static_assert(std::is_same_v<esl::md_array_t<int, 2, 3>, std::array<std::array<int, 3>, 2>>);

	static_assert(std::is_same_v<esl::md_carray_t<int>, int>);
	static_assert(std::is_same_v<esl::md_carray_t<int, 2>, int[2]>);
	static_assert(std::is_same_v<esl::md_carray_t<int, 2, 3>, int[2][3]>);
}

TEST(ArrayTest, array_size) {
	static_assert(esl::array_size_v<int[2]> == 2);
	static_assert(esl::array_size_v<int[2][3]> == 2);
	static_assert(esl::array_size_v<std::array<int, 5>> == 5);
	static_assert(esl::array_size_v<std::array<std::array<int, 2>, 5>> == 5);
}

TEST(ArrayTest, array_rank) {
	static_assert(esl::array_rank_v<int> == 0);
	static_assert(esl::array_rank_v<int[]> == 1);
	static_assert(esl::array_rank_v<int[10]> == 1);
	static_assert(esl::array_rank_v<std::array<int, 5>> == 1);
	static_assert(esl::array_rank_v<std::array<int[5], 10>> == 2);
	static_assert(esl::array_rank_v<esl::md_array_t<int, 2, 3>> == 2);
}

TEST(ArrayTest, array_element) {
	static_assert(std::is_same_v<esl::array_element_t<0, int>, int>);
	static_assert(std::is_same_v<esl::array_element_t<0, int[2]>, int[2]>);
	static_assert(std::is_same_v<esl::array_element_t<1, const int[2]>, int>);
	static_assert(std::is_same_v<esl::array_element_t<0, int[2][3]>, int[2][3]>);
	static_assert(std::is_same_v<esl::array_element_t<1, int[2][3]>, int[3]>);
	static_assert(std::is_same_v<esl::array_element_t<2, int[2][3]>, int>);
	static_assert(std::is_same_v<esl::array_element_t<1, std::array<std::array<int, 3>, 2>>, std::array<int, 3>>);
	static_assert(std::is_same_v<esl::array_element_t<2, std::array<std::array<int, 3>, 2>>, int>);
}

TEST(ArrayTest, make_sized_array) {
	int a[100];
	std::iota(a, a + 100, 0);
	std::array<int, 20> arr = esl::make_sized_array<20>(a + 10);
	ASSERT_EQ(arr[0], 10);
}

TEST(ArrayTest, make_array) {
	int a[100];
	std::iota(a, a + 100, 0);
	std::array<int, 100> arr = esl::make_array(a);
	ASSERT_EQ(arr[10], 10);
}

TEST(ArrayTest, sub_array) {
	int a[100];
	std::iota(a, a + 100, 0);
	{
		std::array<int, 10> arr = esl::sub_array<20, 10>(a);
		ASSERT_EQ(arr[0], 20);
	}
	{
		std::array<int, 80> arr = esl::sub_array<20>(a);
		ASSERT_EQ(arr[0], 20);

		std::array<int, 30> arr2 = esl::sub_array<10, 30>(arr);
		ASSERT_EQ(arr2[0], 30);

		std::array<int, 10> arr3 = esl::sub_array<0, 10>(std::move(arr2));
		ASSERT_EQ(arr3[0], 30);
	}
}

TEST(ArrayTest, get) {
	esl::md_array_t<std::string, 2, 5, 3> arr;
	std::get<0, 1, 2>(arr) = "0,1,2";
	ASSERT_EQ(arr[0][1][2], "0,1,2");
}

