
#include <gtest/gtest.h>
#include <esl/map_utils.hpp>
#include <string>
#include <map>
#include <unordered_map>

struct MapValue {
	int v;
	MapValue(): v(1) {}

	MapValue(int v): v(v) {}
};


template <class Map>
void test_map_get(Map&& m) {
	{
		auto& v1 = esl::map_get(m, "k1");
		ASSERT_EQ(v1.v, 11);

		ASSERT_THROW((esl::map_get(m, "k0")), esl::key_not_found);

		MapValue def(5);
		auto& v3 = esl::map_get(m, "k0", def);
		ASSERT_EQ(&v3, &def);
		ASSERT_EQ(v3.v, 5);
	}

	{
		auto v1 = esl::map_get_if(&m, "k1");
		ASSERT_EQ(v1->v, 11);

		auto v2 = esl::map_get_if(&m, "k0");
		ASSERT_EQ(v2, nullptr);
	}
}

template <class Map>
void test_map_get_non_const(Map&& m) {
	{
		auto v1 = esl::map_get_if(&m, "k1");
		ASSERT_EQ(v1->v, 11);

		v1->v = 111;
		ASSERT_EQ(v1->v, 111);

		auto v3 = esl::map_get_if(&m, "k0");
		ASSERT_EQ(v3, nullptr);
	}

	{
		auto v1 = esl::map_get(m, "k1");
		ASSERT_EQ(v1.v, 111);

		v1.v = 1111;
		ASSERT_EQ(v1.v, 1111);

		MapValue def(5);
		auto v3 = esl::map_get(m, "k0", def);
		ASSERT_EQ(v3.v, 5);

		v3.v = 333;
		ASSERT_EQ(v3.v, 333);
	}
}

TEST(MapUtilsTest, map_getv) {
	std::map<std::string, MapValue> m;
	m.emplace("k1", 11);
	m.emplace("k2", 12);
	m.emplace("k3", 13);

	test_map_get(m);
	test_map_get(std::as_const(m));
	test_map_get_non_const(m);
}

TEST(MapUtilsTest, map_contains) {
	std::map<std::string, MapValue> m;
	m.emplace("k1", 11);

	ASSERT_TRUE(esl::map_contains(std::as_const(m), "k1"));
	ASSERT_FALSE(esl::map_contains(std::as_const(m), "k2"));
}
