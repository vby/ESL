
#ifndef ESL_EXCEPTION_HPP
#define ESL_EXCEPTION_HPP

#include <stdexcept>
#include <cinttypes>
#include <sstream>

namespace esl {

class key_not_found: public std::logic_error {
public:
	using std::logic_error::logic_error;
};

template <class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
class basic_exceptionstream: public std::basic_ostringstream<CharT, Traits, Allocator> {
private:
	using stream_type = std::basic_ostringstream<CharT, Traits, Allocator>;

public:
	explicit basic_exceptionstream() = default;

	explicit basic_exceptionstream(const stream_type& str): stream_type(str) {}

	template <class T>
	basic_exceptionstream& operator<<(T&& value) {
		this->stream_type::operator<<(std::forward<T>(value));
		return *this;
	}

	void operator<<(void (*f)(stream_type&)) { f(*this); }
};

using exceptionstream = basic_exceptionstream<char>;
using wexceptionstream = basic_exceptionstream<wchar_t>;

template <class Excetpion, class CharT, class Traits, class Allocator>
void throw_exception(std::basic_ostringstream<CharT, Traits, Allocator>& oss) {
	throw Excetpion(oss.str().c_str());
}

#define ESL_TRHOW_OUT_OF_RANGE_IF(lhs, op, rhs, location) \
	if ((lhs) op (rhs)) { \
		exceptionstream{} << (location) << ": " #lhs " (which is " << (lhs) \
			<< ") " #op " " #rhs " (which is " << (rhs) << ")" << throw_exception<std::out_of_range>; \
	}

} // namespace esl

#endif //ESL_EXCEPTION_HPP

