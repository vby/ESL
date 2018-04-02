
#ifndef ESL_UTILITY_HPP
#define ESL_UTILITY_HPP

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

// make_vtable, make_vtable_t, make_vtable_v
template <template <class> class F, class... Ts>
struct make_vtable {
	using type = std::array<decltype(&F<nth_type_t<0, Ts...>>::value), sizeof...(Ts)>;
	static constexpr type value{{F<Ts>::value...}};
};
template <template <class> class F>
struct make_vtable<F> {
	using type = std::array<decltype(&F<void>::value), 0>;
	static constexpr type value{};
};
template <template <class> class F, class... Ts>
using make_vtable_t = typename make_vtable<F, Ts...>::type;
template <template <class> class F, class... Ts>
inline constexpr auto make_vtable_v = make_vtable<F, Ts...>::value;

// make_vtable2, make_vtable2_t, make_vtable2_v
template <template <class, class> class F2, class T1, class T2>
struct make_vtable2;
template <template <class, class> class F2, class... T1s, class... T2s>
struct make_vtable2<F2, std::tuple<T1s...>, std::tuple<T2s...>> {
	template <class T1>
	struct make_vtable2_1 {
		using type = std::array<decltype(&F2<T1, nth_type_t<0, T2s...>>::value), sizeof...(T2s)>;
		static constexpr type value{{F2<T1, T2s>::value...}};
	};
	using type = std::array<typename make_vtable2_1<nth_type_t<0, T1s...>>::type, sizeof...(T1s)>;
	static constexpr type value{{make_vtable2_1<T1s>::value...}};
};
template <template <class, class> class F2>
struct make_vtable2<F2, std::tuple<>, std::tuple<>> {
	using type = std::array<std::array<decltype(&F2<void, void>::value), 0>, 0>;
	static constexpr type value{};
};
template <template <class, class> class F2, class T1, class T2>
using make_vtable2_t = typename make_vtable2<F2, T1, T2>::type;
template <template <class, class> class F2, class T1, class T2>
inline constexpr auto make_vtable2_v = make_vtable2<F2, T1, T2>::value;

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

