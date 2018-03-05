
#include <gtest/gtest.h>
#include <esl/linked_list.hpp>

struct SomeValue {
	int a;
	SomeValue(int a): a(a) {}
};

TEST(LinkedListTest, extract) {
	esl::linked_list<SomeValue> val_list;
	ASSERT_TRUE(val_list.empty());

	val_list.push_back(SomeValue(1));
	val_list.push_back(SomeValue(2));
	val_list.push_back(SomeValue(3));
	val_list.push_back(SomeValue(4));
	val_list.push_back(SomeValue(5));

	ASSERT_EQ(val_list.size(), 5);

	// 1 2 3 4 5
	{
		auto it = val_list.begin();
		++it;
		ASSERT_EQ(it->a, 2);

		auto nh = val_list.extract(it);

		ASSERT_TRUE(nh);
		ASSERT_EQ(nh.value().a, 2);
		ASSERT_EQ(val_list.size(), 4);
	}

	ASSERT_EQ(val_list.size(), 4);

	// 1 3 4 5
	{
		auto it = val_list.end();
		std::advance(it, -2);
		ASSERT_EQ(it->a, 4);

		auto nh = val_list.extract(it);

		ASSERT_TRUE(nh);
		ASSERT_EQ(nh.value().a, 4);
		ASSERT_EQ(val_list.size(), 3);

		val_list.push_back(std::move(nh));

		ASSERT_EQ(val_list.front().a, 1);
		ASSERT_EQ(val_list.back().a, 4);
		ASSERT_EQ(val_list.size(), 4);
	}

	ASSERT_EQ(val_list.size(), 4);

	// 1 3 5 4
	{
		auto it = val_list.begin();
		std::advance(it, 2);
		ASSERT_EQ(it->a, 5);

		auto nh = val_list.extract(it);

		ASSERT_TRUE(nh);
		ASSERT_EQ(nh.value().a, 5);
		ASSERT_EQ(val_list.size(), 3);

		auto it_insert = val_list.insert(std::next(val_list.begin(), 1), std::move(nh));

		ASSERT_EQ(it_insert->a, 5);
		it_insert->a = 6;
		ASSERT_EQ(it_insert->a, 6);

		ASSERT_EQ(val_list.front().a, 1);
		ASSERT_EQ(val_list.back().a, 4);
		ASSERT_EQ(val_list.size(), 4);
	}

	ASSERT_EQ(val_list.size(), 4);

	// 1 6 3 4
}


