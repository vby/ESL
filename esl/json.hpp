#ifndef ESL_JSON_HPP
#define ESL_JSON_HPP

#include "any_variant.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace esl {

namespace json {

class value;

// types
using null_t = std::monostate;
using boolean = bool;
using number = double;
using string = std::string;
using array = std::vector<value>;
using object = std::unordered_map<std::string, value>;

// null
inline constexpr null_t null{};

// zero-base type index
enum index {
    null_index = 0,
    boolean_index,
    number_index,
    string_index,
    array_index,
    object_index,
};

using value_base = any_variant<null_t, boolean, number, string, array, object>;

class value: public value_base {
public:
    using any_variant::any_variant;

    using any_variant::operator=;
};

} // namespace json

} // namespace esl

namespace std {

// std::variant_size
template <>
struct variant_size<::esl::json::value>: variant_size<::esl::json::value_base> {};

// std::variant_alternative
template <std::size_t I>
struct variant_alternative<I, ::esl::json::value>: variant_alternative<I, ::esl::json::value_base> {};

// std::hash
template <>
struct hash<::esl::json::value> {
	size_t operator()(const ::esl::json::value& n) const {
		return hash<::esl::json::value_base>{}(n);
	}
};

} // namespace std

#endif // ESL_JSON_HPP

