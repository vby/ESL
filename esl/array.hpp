#ifndef ESL_ARRAY_HPP
#define ESL_ARRAY_HPP

#include "type_traits.hpp"

#include <array>
#include <utility>

namespace esl {

// earray
template <class T, std::size_t N>
class earray: public std::array<T, N> {
public:
    using std::array<T, N>::array;

    template <std::size_t M>
    constexpr earray(std::integral_constant<std::size_t, M>, const T* arr): earray(arr, std::make_index_sequence<N>{}) {
        static_assert(M >= N);
    }

    template <std::size_t M>
    constexpr earray(const T (&arr)[M]): earray(std::integral_constant<std::size_t, N>{}, arr) {}

    template <std::size_t M>
	constexpr earray(const std::array<T, M>& arr): earray(std::integral_constant<std::size_t, N>{}, arr.data()) {}

	//constexpr earray& operator=(const earray&) = default;
	//constexpr earray& operator=(earray&&) = default;

private:
    template <std::size_t... Is>
    constexpr earray(const T* arr, std::index_sequence<Is...>): std::array<T, N>{{arr[Is]...}} {}
};

// array_size, array_size_v
template <class T>
struct array_size;
template <class T, std::size_t N>
struct array_size<T[N]>: std::integral_constant<std::size_t, N> {};
template <class T, std::size_t N>
struct array_size<const T[N]>: std::integral_constant<std::size_t, N> {};
template <class T, std::size_t N>
struct array_size<volatile T[N]>: std::integral_constant<std::size_t, N> {};
template <class T, std::size_t N>
struct array_size<const volatile T[N]>: std::integral_constant<std::size_t, N> {};
template <class T, std::size_t N>
struct array_size<std::array<T, N>>: std::integral_constant<std::size_t, N> {};
template <class T, std::size_t N>
struct array_size<earray<T, N>>: std::integral_constant<std::size_t, N> {};
template <class T>
struct array_size<const T>: array_size<T> {};
template <class T>
struct array_size<volatile T>: array_size<T> {};
template <class T>
struct array_size<const volatile T>: array_size<T> {};
template <class T>
inline constexpr std::size_t array_size_v = array_size<T>::value;

// array_rank, array_rank_v
template <class T>
struct array_rank: std::integral_constant<std::size_t, 0> {};
template <class T, std::size_t N>
struct array_rank<std::array<T, N>>: std::integral_constant<std::size_t, array_rank<T>::value + 1> {};
template <class T, std::size_t N>
struct array_rank<earray<T, N>>: array_rank<std::array<T, N>> {};
template <class T>
struct array_rank<const T>: array_rank<T> {};
template <class T>
struct array_rank<volatile T>: array_rank<T> {};
template <class T>
struct array_rank<const volatile T>: array_rank<T> {};
template <class T>
inline constexpr std::size_t array_rank_v = array_rank<T>::value;

// array_element, array_element_t
template <std::size_t Rank, class T>
struct array_element { using type = T; };
template <std::size_t Rank, class T, std::size_t N>
struct array_element<Rank, std::array<T, N>>: array_element<Rank - 1, typename std::array<T, N>::value_type> {};
template <std::size_t Rank, class T, std::size_t N>
struct array_element<Rank, earray<T, N>>: array_element<Rank, std::array<T, N>> {};
template <std::size_t Rank, class T>
struct array_element<Rank, const T>: array_element<Rank, T> {};
template <std::size_t Rank, class T>
struct array_element<Rank, volatile T>: array_element<Rank, T> {};
template <std::size_t Rank, class T>
struct array_element<Rank, const volatile T>: array_element<Rank, T> {};
template <std::size_t Rank, class T>
using array_element_t = typename array_element<Rank, T>::type;

// array_element_size, array_element_size_v
template <std::size_t Rank, class T>
struct array_element_size: array_size<array_element_t<Rank, T>> {};
template <std::size_t Rank, class T>
inline constexpr std::size_t array_element_size_v = array_element_size<Rank, T>::value;


namespace details {

template <class T>
inline constexpr decltype(auto) array_access(T& a) noexcept { return a; }
template <class T, std::size_t N, class... Rest>
inline constexpr decltype(auto) array_access(std::array<T, N>& a, std::size_t first_pos, Rest... rest_pos) noexcept {
	return array_access(a[first_pos], rest_pos...);
}
template <class T, std::size_t N, class... Rest>
inline constexpr decltype(auto) array_access(const std::array<T, N>& a, std::size_t first_pos, Rest... rest_pos) noexcept {
	return array_access(a[first_pos], rest_pos...);
}

template <class T>
inline constexpr decltype(auto) array_at(T& a) noexcept { return a; }
template <class T, std::size_t N, class... Rest>
inline constexpr decltype(auto) array_at(std::array<T, N>& a, std::size_t first_pos, Rest... rest_pos) noexcept {
	return array_at(a.at(first_pos), rest_pos...);
}
template <class T, std::size_t N, class... Rest>
inline constexpr decltype(auto) array_at(const std::array<T, N>& a, std::size_t first_pos, Rest... rest_pos) noexcept {
	return array_at(a.at(first_pos), rest_pos...);
}

// array_get
template <std::size_t... Is>
struct array_get {
	template <class T>
	static constexpr T& get(T& a) noexcept { return a; }
};
template <std::size_t First, std::size_t... Rest>
struct array_get<First, Rest...> {
	template <class T, std::size_t N>
	static constexpr decltype(auto) get(std::array<T, N>& a) noexcept {
		return array_get<Rest...>::template get(std::get<First>(a));
	}
	template <class T, std::size_t N>
	static constexpr decltype(auto) get(const std::array<T, N>& a) noexcept {
		return array_get<Rest...>::template get(std::get<First>(a));
	}
};

} // namespace details

// multi_array, multi_array_t
template <class T, std::size_t... Dimensions>
struct multi_array {
	using type = T;
};
template <class T, std::size_t First, std::size_t... Rest>
struct multi_array<T, First, Rest...> {
	using type = std::array<typename multi_array<T, Rest...>::type, First>;
};
template <class T, std::size_t... Dimensions>
using multi_array_t = typename multi_array<T, Dimensions...>::type;

// access
template <class T, std::size_t N, class... Poss>
inline constexpr array_element_t<sizeof...(Poss), std::array<T, N>>& access(std::array<T, N>& a, Poss... poss) noexcept {
	return details::array_access(a, poss...);
}
template <class T, std::size_t N, class... Poss>
inline constexpr const array_element_t<sizeof...(Poss), std::array<T, N>>& access(const std::array<T, N>& a, Poss... poss) noexcept {
	return details::array_access(a, poss...);
}
template <class T, std::size_t N, class... Poss>
inline constexpr array_element_t<sizeof...(Poss), std::array<T, N>>&& access(std::array<T, N>&& a, Poss... poss) noexcept {
	return std::move(access(a, poss...));
}
template <class T, std::size_t N, class... Poss>
inline constexpr const array_element_t<sizeof...(Poss), std::array<T, N>>&& access(const std::array<T, N>&& a, Poss... poss) noexcept {
	return std::move(access(a, poss...));
}

// at
template <class T, std::size_t N, class... Poss>
inline constexpr array_element_t<sizeof...(Poss), std::array<T, N>>& at(std::array<T, N>& a, Poss... poss) noexcept {
	return details::array_at(a, poss...);
}
template <class T, std::size_t N, class... Poss>
inline constexpr const array_element_t<sizeof...(Poss), std::array<T, N>>& at(const std::array<T, N>& a, Poss... poss) noexcept {
	return details::array_at(a, poss...);
}
template <class T, std::size_t N, class... Poss>
inline constexpr array_element_t<sizeof...(Poss), std::array<T, N>>&& at(std::array<T, N>&& a, Poss... poss) noexcept {
	return std::move(at(a, poss...));
}
template <class T, std::size_t N, class... Poss>
inline constexpr const array_element_t<sizeof...(Poss), std::array<T, N>>&& at(const std::array<T, N>&& a, Poss... poss) noexcept {
	return std::move(at(a, poss...));
}

} // namespace esl

namespace std {

// get
template <size_t... Is, class T, std::size_t N, class = std::enable_if_t<sizeof...(Is) != 1>>
inline constexpr ::esl::array_element_t<sizeof...(Is), std::array<T, N>>& get(std::array<T, N>& a) noexcept {
	return ::esl::details::array_get<Is...>::get(a);
}
template <size_t... Is, class T, std::size_t N, class = std::enable_if_t<sizeof...(Is) != 1>>
inline constexpr const ::esl::array_element_t<sizeof...(Is), std::array<T, N>>& get(const std::array<T, N>& a) noexcept {
	return ::esl::details::array_get<Is...>::get(a);
}
template <size_t... Is, class T, std::size_t N, class = std::enable_if_t<sizeof...(Is) != 1>>
inline constexpr ::esl::array_element_t<sizeof...(Is), std::array<T, N>>&& get(std::array<T, N>&& a) noexcept {
	return move(::esl::details::array_get<Is...>::get(a));
}
template <size_t... Is, class T, std::size_t N, class = std::enable_if_t<sizeof...(Is) != 1>>
inline constexpr const ::esl::array_element_t<sizeof...(Is), std::array<T, N>>&& get(const std::array<T, N>&& a) noexcept {
	return move(::esl::details::array_get<Is...>::get(a));
}

} // namespace std

namespace esl {

using std::get;

} // namespace esl

#endif // ESL_ARRAY_HPP

