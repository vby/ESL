
#ifndef ESL_TYPE_TRAITS_HPP
#define ESL_TYPE_TRAITS_HPP

#include "macros.hpp"

// C++20 endian

#if __has_include(<endian.h>) || __has_include(<sys/endian.h>)

#if __has_include(<endian.h>)
	#include <endian.h>
#else
	#include <sys/endian.h>
#endif

namespace esl {

enum class endian {
	little = __LITTLE_ENDIAN,
	big    = __BIG_ENDIAN, 
	native = __BYTE_ORDER,
};

} // namespace esl

#else // FIXME: Assume always little-endian 

namespace esl {

enum class endian {
	little = 1234,
	big    = 4321, 
	native = 1234,
};

} // namespace esl

#endif 

namespace esl {

// member_allocator_type: T::allocator_type
template <class T, class D = void, class = std::void_t<>>
struct member_allocator_type: std::false_type {
	using fallback_type = D;
};
template <class T, class D>
struct member_allocator_type<T, D, std::void_t<typename T::allocator_type>>: std::true_type {
	using type = typename T::allocator_type; 
	using fallback_type = type;
};
template <class T>
inline constexpr bool member_allocator_type_v = member_allocator_type<T>::value; 
template <class T>
using member_allocator_type_t = typename member_allocator_type<T>::type;
template <class T, class D>
using member_allocator_type_fallback_t = typename member_allocator_type<T, D>::fallback_type;

} // namespace esl

#endif // ESL_TYPE_TRAITS_HPP

