
#ifndef ESL_EXCEPTION_HPP
#define ESL_EXCEPTION_HPP

#include "std_ext.hpp"

#include <cstdarg>
#include <cinttypes>

extern int vsnprintf(char* buffer, size_t buf_size, const char* format, va_list vlist); 

namespace esl {

template <class T, std::size_t Size = 1024>
[[noreturn]] void throw_exception_format(const char* fmt, ...) {
	char* const s = static_cast<char*>(ESL_ALLOCA(Size));
	va_list ap; 
	va_start(ap, fmt);
	::vsnprintf(s, Size, fmt, ap);
	throw T(s);
	va_end(ap);  // Not reached.
}

#define ESL_TRHOW_OUT_OF_RANGE_IF(lhs, op, rhs, location) \
	if ((lhs) op (rhs)) { \
		throw_exception_format<std::out_of_range>("%s: " #lhs " (which is %" PRIdMAX ") " \
			#op " " #rhs " (which is %" PRIdMAX ")", (location), (lhs), (rhs)); \
	}

} // namespace esl

#endif //ESL_EXCEPTION_HPP

