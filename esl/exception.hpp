
#ifndef ESL_EXCEPTION_HPP
#define ESL_EXCEPTION_HPP

#include "string.hpp"

#include <stdexcept>
#include <cinttypes>
#include <sstream>

namespace esl {

class key_not_found: public std::logic_error {
public:
	using std::logic_error::logic_error;
};

#define ESL_TRHOW_OUT_OF_RANGE_IF(lhs, op, rhs, location) \
	if ((lhs) op (rhs)) { \
		throw std::out_of_range(format("{}: " #lhs " (which is \"{}\") " #op " " #rhs " (which is \"{}\")", location, lhs, rhs)); \
	}

} // namespace esl

#endif //ESL_EXCEPTION_HPP

