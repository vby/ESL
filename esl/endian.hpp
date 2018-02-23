
#ifndef ESL_ENDIAN_HPP
#define ESL_ENDIAN_HPP

#if __has_include(<endian.h>)
	#define ESL_HAS_BSD_ENDIAN
	#include <endian.h>
#elif __has_include(<sys/endian.h>)
	#define ESL_HAS_BSD_ENDIAN
	#include <sys/endian.h>
#endif

namespace esl {
#ifdef ESL_HAS_BSD_ENDIAN
	// C++20 endian in <type_traits>
	enum class endian {
		little = __LITTLE_ENDIAN,
		big    = __BIG_ENDIAN,
		native = __BYTE_ORDER,
	};
#else // FIXME: Assume little-endian
	enum class endian {
		little = 1234,
		big    = 4321,
		native = 1234,
	};
#endif

} // namespace esl

#endif // ESL_ENDIAN_HPP

