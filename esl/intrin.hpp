#ifndef ESL_INTRIN_HPP
#define ESL_INTRIN_HPP

#include "macros.hpp"

namespace esl {

// constexpr _rot*

template <class T>
ESL_ATTR_FORCEINLINE constexpr T rotl(T x, int s) noexcept {
	return ((x << s) | (x >> (sizeof(T) * 8 - s)));
}

template <class T>
ESL_ATTR_FORCEINLINE constexpr T rotr(T x, int s) noexcept {
	return ((x >> s) | (x << (sizeof(T) * 8 - s)));
}

} // namespace esl

#endif // ESL_INTRIN_HPP

