#ifndef ESL_INTRIN_HPP
#define ESL_INTRIN_HPP

#include "macros.hpp"
#include "limits.hpp"

#include <type_traits>

namespace esl {

// constexpr _rot*
// rotl
template <class T>
ESL_ATTR_FORCEINLINE constexpr T rotl(T x, unsigned char s) noexcept {
	return ((x << s) | (x >> (sizeof(T) * 8 - s)));
}
// rotr
template <class T, class = std::enable_if_t<std::is_unsigned_v<T>>>
ESL_ATTR_FORCEINLINE constexpr T rotr(T x, unsigned char s) noexcept {
	return ((x >> s) | (x << (sizeof(T) * 8 - s)));
}
// clz
template <class T, class = std::enable_if_t<std::is_unsigned_v<T>>>
ESL_ATTR_FORCEINLINE unsigned char clz(T x) noexcept {
	if constexpr (sizeof(T) == 4) {
	#ifdef ESL_COMPILER_MSVC
		return __lzcnt(x);
	#else
		return static_cast<unsigned char>(__builtin_clz(x));
	#endif
	} else if constexpr (sizeof(T) == 8) {
	#ifdef ESL_COMPILER_MSVC
		return __lzcnt64(x);
	#else
		return static_cast<unsigned char>(__builtin_clzll(x));
	#endif
	}
}

} // namespace esl

#endif // ESL_INTRIN_HPP

