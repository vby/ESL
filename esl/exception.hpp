
#ifndef ESL_EXCEPTION_HPP
#define ESL_EXCEPTION_HPP

#include <cinttypes>
#include <sstream>

namespace esl {

template <class Excetpion, class OStream = std::ostringstream>
class throw_stream {
public:
	using stream_type = OStream;
	using exception_type = Excetpion;

private:
	OStream stream_;

public:
	explicit throw_stream() = default;

	explicit throw_stream(const std::string& str): stream_(str) {}

	throw_stream(throw_stream&&) = default;

	~throw_stream() noexcept(false) { throw Excetpion(stream_.str()); }

	OStream& stream() noexcept { return stream_; }

	template <class T>
	throw_stream& operator<<(T&& t) {
		stream_ << std::forward<T>(t);
		return *this;
	}
};

#define ESL_TRHOW_OUT_OF_RANGE_IF(lhs, op, rhs, location) \
	if ((lhs) op (rhs)) { \
		throw_stream<std::out_of_range>{} << (location) << ": " #lhs " (which is " << (lhs) \
			<< ") " #op " " #rhs " (which is " << (rhs) << ")"; \
	}

} // namespace esl

#endif //ESL_EXCEPTION_HPP

