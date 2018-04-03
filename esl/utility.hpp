
#ifndef ESL_UTILITY_HPP
#define ESL_UTILITY_HPP

#include "macros.hpp"
#include "type_traits.hpp"
#include "array.hpp"

#include <cstdint>
#include <limits>
#include <array>
#include <utility>
#include <initializer_list>
#include <type_traits>
#include <cstring>

namespace esl {

inline namespace casts {

template <class To, class From>
inline constexpr To implicit_cast(From&& v) {
	return std::forward<From>(v);
}

template <class To, class From>
inline constexpr To narrow_cast(From&& v) {
	return static_cast<To>(std::forward<From>(v));
}

template <class To, class From>
inline constexpr To bit_cast(const From& v) noexcept {
	static_assert(std::is_pod_v<From>, "bit_cast source type should be POD");
	static_assert(std::is_pod_v<To>, "bit_cast destination type should be POD");
	static_assert(sizeof(From) == sizeof(To), "bit_cast types should have same size");
	To to{};
	std::memcpy(&to, &v, sizeof(To));
	return to;
}

} // inline namespace casts

template <template <class...> class F, class... Tups>
struct make_tuple_vtable {
	using type = multi_array_t<decltype(&F<std::tuple_element_t<0, Tups>...>::value), std::tuple_size_v<Tups>...>;
private:
	template <std::size_t... Is>
	static constexpr void apply_alt(type& vt, std::index_sequence<Is...>) {
		std::get<Is...>(vt) = F<std::tuple_element_t<Is, Tups>...>::value;
	}
	template <class... IntSeq>
	static constexpr type gen_vtable(std::tuple<IntSeq...>) {
		type vt{};
		(..., apply_alt(vt, IntSeq{}));
		return vt;
	}
public:
	static constexpr type value = gen_vtable(index_sequence_combination_for<Tups...>{});
};
template <template <class...> class F, class... Tups>
using make_tuple_vtable_t = typename make_tuple_vtable<F, Tups...>::type;
template <template <class...> class F, class... Tups>
inline constexpr auto make_tuple_vtable_v = make_tuple_vtable<F, Tups...>::value;

// make_index_sequence_vtable, make_index_sequence_vtable_t, make_index_sequence_vtable_v
template <template <std::size_t...> class F, std::size_t... Dimensions>
struct make_index_sequence_vtable {
private:
	template <std::size_t>
	struct AlwaysZero { static constexpr std::size_t value = 0; };
public:
	using type = multi_array_t<decltype(&F<AlwaysZero<Dimensions>::value...>::value), Dimensions...>;
private:
	template <std::size_t... Is>
	static constexpr void apply_alt(type& vt, std::index_sequence<Is...>) {
		std::get<Is...>(vt) = F<Is...>::value;
	}
	template <class... IntSeq>
	static constexpr type gen_vtable(std::tuple<IntSeq...>) {
		type vt{};
		(..., apply_alt(vt, IntSeq{}));
		return vt;
	}
public:
	static constexpr type value = gen_vtable(make_index_sequence_combination<Dimensions...>{});
};
template <template <std::size_t...> class F, std::size_t... Dimensions>
using make_index_sequence_vtable_t = typename make_index_sequence_vtable<F, Dimensions...>::type;
template <template <std::size_t...> class F, std::size_t... Dimensions>
inline constexpr auto make_index_sequence_vtable_v = make_index_sequence_vtable<F, Dimensions...>::value;


// transpose_integer_array
template <class T, std::size_t Size, std::size_t N, class U>
constexpr std::array<T, Size> transpose_integer_array(U* arr, std::initializer_list<std::pair<U, T>> kvs) {
	static_assert(N <= std::numeric_limits<T>::max(), "target type is too small");
	std::array<T, Size> t_arr{};
	for (std::size_t i = 0; i < Size; ++i) {
		t_arr[i] = std::numeric_limits<T>::max();
	}
	using UU = std::make_unsigned_t<U>;
	for (std::size_t i = 0; i < N; ++i) {
		t_arr[static_cast<UU>(arr[i])] = static_cast<T>(i);
	}
	// Fail compile on MSVC - 19.13.26128
	// for (auto& kv: kvs) {
	//		t_arr[kv.first] = kv.second;
	// }
	auto it = kvs.begin();
	while (it != kvs.end()) {
		t_arr[it->first] = it->second;
		++it;
	}
	return t_arr;
}
template <class T, std::size_t Size, class U, std::size_t N>
constexpr std::array<T, Size> transpose_integer_array(U (&arr)[N], std::initializer_list<std::pair<U, T>> kvs) {
	return transpose_integer_array<T, Size, N>(arr, kvs);
}
template <class T, std::size_t Size, class U, std::size_t N>
constexpr std::array<T, Size> transpose_integer_array(const std::array<T, N>& arr, std::initializer_list<std::pair<U, T>> kvs) {
	return transpose_integer_array<T, Size, N>(arr, kvs);
}

// add_emplace
template <class C, class... Args>
inline decltype(auto) add_emplace(C& c, Args&&... args) {
	if constexpr (is_emplace_backable_v<C, Args&&...>) {
		return c.emplace_back(std::forward<Args>(args)...);
	} else if constexpr (is_emplaceable_v<C, Args&&...>) {
		return c.emplace(std::forward<Args>(args)...);
	} else {
		return c.emplace_front(std::forward<Args>(args)...);
	}
}

} // namespace esl

#endif //ESL_UTILITY_HPP

