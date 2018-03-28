
#include <gtest/gtest.h>
#include <esl/json.hpp>

TEST(JsonTest, value) {
	esl::json::object robj = { {"k1", 123456.0}, {"k2", 123.456}, {"k3", esl::json::string("v3")}, {"k4", true}, {"k5", esl::json::null} };
	esl::json::value jv(std::move(robj));

	ASSERT_TRUE(jv.has_value());
	ASSERT_EQ(jv.index(), esl::json::object_index);
	ASSERT_EQ(std::get<esl::json::number>(std::get<esl::json::object>(jv)["k1"]), 123456);
	ASSERT_DOUBLE_EQ(std::get<esl::json::number>(std::get<esl::json::object>(jv)["k2"]), 123.456);
	ASSERT_EQ(std::get<esl::json::string>(std::get<esl::json::object>(jv)["k3"]), "v3");
	ASSERT_EQ(std::get<esl::json::boolean>(std::get<esl::json::object>(jv)["k4"]), true);
	ASSERT_EQ(std::get<esl::json::object>(jv)["k5"].index(), esl::json::null_index);

	auto& root_obj = std::get<esl::json::object>(jv);
	root_obj.emplace("arr", esl::json::value(std::in_place_type<esl::json::array>, {
				esl::json::value(123456.0),
				esl::json::value(123.456),
				esl::json::value(esl::json::string("v3")),
				esl::json::value(true),
				esl::json::value(esl::json::null),
			}));
	root_obj.emplace("obj", esl::json::value(std::in_place_type<esl::json::object>));

	auto& arr_value = root_obj["arr"];
	ASSERT_TRUE(arr_value.has_value());
	ASSERT_EQ(arr_value.index(), esl::json::array_index);

	auto& arr = std::get<esl::json::array>(arr_value);
	ASSERT_EQ(arr.size(), 5);
	ASSERT_EQ(arr[0].index(), esl::json::number_index);
	ASSERT_EQ(arr[3].index(), esl::json::boolean_index);
	ASSERT_EQ(arr[4].index(), esl::json::null_index);

	auto& obj_value = root_obj["obj"];
	ASSERT_TRUE(obj_value.has_value());
	ASSERT_EQ(obj_value.index(), esl::json::object_index);

	auto& obj = std::get<esl::json::object>(obj_value);
	ASSERT_EQ(obj.size(), 0);
	obj["obj_k1"]  = esl::json::string("obj_v1");
	ASSERT_EQ(obj.size(), 1);

	ASSERT_EQ(std::get<esl::json::string>(std::get<esl::json::object>(std::get<esl::json::object>(jv)["obj"])["obj_k1"]), "obj_v1");
}

