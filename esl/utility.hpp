
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

namespace details {

template <class Se1, class Seq2>
struct IntegerSequenceConcat;
template <class T, std::size_t... Ints1, std::size_t... Ints2>
struct IntegerSequenceConcat<std::integer_sequence<T, Ints1...>, std::integer_sequence<T, Ints2...>> {
	using type = std::integer_sequence<T, Ints1..., (Ints2 + sizeof...(Ints1))...>;
};
template <class T, std::size_t N>
struct BuildIntegerSequence: IntegerSequenceConcat<typename BuildIntegerSequence<T, N / 2>::type, typename BuildIntegerSequence<T, N - N / 2>::type> {};
template <class T>
struct BuildIntegerSequence<T, 1> { using type = std::integer_sequence<T, 0>; };
template <class T>
struct BuildIntegerSequence<T, 0> { using type = std::integer_sequence<T>; };
template <class T, T N>
struct MakeIntegerSequence {
	static_assert(N >= 0, "std::make_integer_sequence input shall not be negative");
	using type = typename BuildIntegerSequence<T, static_cast<std::size_t>(N)>::type;
};

} // namespace details

// make_integer_sequence
template <class T, T N>
using make_integer_sequence = typename details::MakeIntegerSequence<T, N>::type;
// index_sequence
template <size_t... Ints>
using index_sequence = std::integer_sequence<std::size_t, Ints...>;
// make_index_sequence
template <size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;
// index_sequence_for
template <class... Ts>
using index_sequence_for = make_index_sequence<sizeof...(Ts)>;

// make_tuple_vtable, make_tuple_vtable_t, make_tuple_vtable_v
template <template <class...> class F, class... Tups>
struct make_tuple_vtable {
	using type = multi_array_t<decltype(&F<std::tuple_element_t<0, Tups>...>::value), std::tuple_size_v<Tups>...>;
private:
	template <std::size_t... Is>
	static constexpr void apply_alt(type& vtbl, std::index_sequence<Is...>) {
		std::get<Is...>(vtbl) = &F<std::tuple_element_t<Is, Tups>...>::value;
	}
	template <class TupIntSeq>
	struct VTable {};
	template <class... IntSeq>
	struct VTable<std::tuple<IntSeq...>> {
		static constexpr type gen_vtable() {
			type vtbl{};
			(..., apply_alt(vtbl, IntSeq{}));
			return vtbl;
		}
	};
public:
#ifdef ESL_COMPILER_MSVC
	static constexpr type value = VTable<integer_sequence_combination_t<make_index_sequence<std::tuple_size_v<Tups>>...>>::gen_vtable();
#else
	static constexpr type value = VTable<integer_sequence_combination_t<std::make_index_sequence<std::tuple_size_v<Tups>>...>>::gen_vtable();
#endif
};
template <template <class...> class F, class... Tups>
using make_tuple_vtable_t = typename make_tuple_vtable<F, Tups...>::type;
template <template <class...> class F, class... Tups>
inline constexpr auto make_tuple_vtable_v = make_tuple_vtable<F, Tups...>::value;

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

