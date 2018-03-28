#ifndef ESL_JSON_HPP
#define ESL_JSON_HPP

#include "variant_any.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace esl {

namespace json {

class value;

// types
struct null_t {
	explicit constexpr null_t() noexcept = default;
	constexpr bool operator==(const null_t&) const noexcept { return true; }
	constexpr bool operator!=(const null_t&) const noexcept { return false; }
	constexpr bool operator<(const null_t&) const noexcept { return false; }
	constexpr bool operator>(const null_t&) const noexcept { return false; }
	constexpr bool operator<=(const null_t&) const noexcept { return true; }
	constexpr bool operator>=(const null_t&) const noexcept { return true; }
	constexpr operator bool() const noexcept { return false; }
};
using boolean = bool;
using number = double;
using string = std::string;
using array = std::vector<value>;
using object = std::unordered_map<std::string, value>;

inline constexpr null_t null{};

enum index {
    null_index = 0,
    boolean_index,
    number_index,
    string_index,
    array_index,
    object_index,
};

class value: public variant_any<null_t, boolean, number, string, array, object> {
public:
    using variant_any::variant_any;

    using variant_any::operator=;
};

} // namespace json

} // namespace esl

#endif // ESL_JSON_HPP

