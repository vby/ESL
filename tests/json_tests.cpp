
#include <gtest/gtest.h>
#include <esl/json.hpp>

namespace json = esl::json;

TEST(JsonTest, value) {
	json::object robj = { {"k1", 123456.0}, {"k2", 123.456}, {"k3", json::string("v3")}, {"k4", true}, {"k5", json::null} };
	json::value jv(std::move(robj));

	ASSERT_TRUE(!jv.valueless_by_exception());
	ASSERT_EQ(jv.index(), json::object_index);
	ASSERT_EQ(std::get<json::number>(std::get<json::object>(jv)["k1"]), 123456);
	ASSERT_DOUBLE_EQ(std::get<json::number>(std::get<json::object>(jv)["k2"]), 123.456);
	ASSERT_EQ(std::get<json::string>(std::get<json::object>(jv)["k3"]), "v3");
	ASSERT_EQ(std::get<json::boolean>(std::get<json::object>(jv)["k4"]), true);
	ASSERT_EQ(std::get<json::object>(jv)["k5"].index(), json::null_index);

	auto& root_obj = std::get<json::object>(jv);
	root_obj.emplace("arr", json::value(std::in_place_type<json::array>, {
				json::value(123456.0),
				json::value(123.456),
				json::value(json::string("v3")),
				json::value(true),
				json::value(json::null),
			}));
	root_obj.emplace("obj", json::value(std::in_place_type<json::object>));

	auto& arr_value = root_obj["arr"];
	ASSERT_EQ(arr_value.index(), json::array_index);

	auto& arr = std::get<json::array>(arr_value);
	ASSERT_EQ(arr.size(), 5);
	ASSERT_EQ(arr[0].index(), json::number_index);
	ASSERT_EQ(arr[3].index(), json::boolean_index);
	ASSERT_EQ(arr[4].index(), json::null_index);

	auto& obj_value = root_obj["obj"];
	ASSERT_EQ(obj_value.index(), json::object_index);

	auto& obj = std::get<json::object>(obj_value);
	ASSERT_EQ(obj.size(), 0);
	obj["obj_k1"]  = json::string("obj_v1");
	ASSERT_EQ(obj.size(), 1);

	ASSERT_EQ(std::get<json::string>(std::get<json::object>(std::get<json::object>(jv)["obj"])["obj_k1"]), "obj_v1");
}

