#ifndef ESL_JSON_HPP
#define ESL_JSON_HPP

#include "flex_variant.hpp"

#include <string>
#include <unordered_map>
#include <vector>

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

using value_base = flex_variant<null_t, boolean, number, string, array, object>;

class value : public value_base {
public:
    using value_base::value_base;
};

} // namespace json

} // namespace esl

namespace std {

// std::variant_size
template <>
struct variant_size<::esl::json::value> : variant_size<::esl::json::value_base> {};

// std::variant_alternative
template <std::size_t I>
struct variant_alternative<I, ::esl::json::value> : variant_alternative<I, ::esl::json::value_base> {};

// std::hash
template <>
struct hash<::esl::json::value> {
    size_t operator()(const ::esl::json::value& n) const {
        return hash<::esl::json::value_base>{}(n);
    }
};

} // namespace std

#endif // ESL_JSON_HPP
