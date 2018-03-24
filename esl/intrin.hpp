#ifndef ESL_INTRIN_HPP
#define ESL_INTRIN_HPP

#include "macros.hpp"

namespace esl {

// constexpr _rot*
// rotl
template <class T>
ESL_ATTR_FORCEINLINE constexpr T rotl(T x, unsigned char s) noexcept {
	return ((x << s) | (x >> (sizeof(T) * 8 - s)));
}
// rotr
template <class T>
ESL_ATTR_FORCEINLINE constexpr T rotr(T x, unsigned char s) noexcept {
	return ((x >> s) | (x << (sizeof(T) * 8 - s)));
}
// clz
template <class T>
ESL_ATTR_FORCEINLINE unsigned char clz(T x) noexcept {
#ifdef ESL_COMPILER_MSVC
	if constexpr (sizeof(T) == sizeof(unsigned int)) {
		return __lzcnt(x);
	} else if constexpr (sizeof(T) == 8) {
		return __lzcnt64(x);
	}
#else
	if constexpr (sizeof(T) == sizeof(unsigned int)) {
		return static_cast<unsigned char>(__builtin_clz(x));
	} else if constexpr (sizeof(T) == sizeof(unsigned long)) {
		return static_cast<unsigned char>(__builtin_clzl(x));
	} else if constexpr (sizeof(T) == sizeof(unsigned long long)) {
		return static_cast<unsigned char>(__builtin_clzll(x));
	}
#endif
}

} // namespace esl

#endif // ESL_INTRIN_HPP

