
#ifndef ESL_TYPE_TRAITS_HPP
#define ESL_TYPE_TRAITS_HPP

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

