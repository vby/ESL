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

class value: public any_variant<null_t, boolean, number, string, array, object> {
public:
    using any_variant::any_variant;

    using any_variant::operator=;
};

} // namespace json

} // namespace esl

#endif // ESL_JSON_HPP

