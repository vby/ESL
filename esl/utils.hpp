
#ifndef ESL_UTILS_HPP
#define ESL_UTILS_HPP

#include <cstdint>
#include <limits>
#include <array>
#include <utility>
#include <initializer_list>

namespace esl {

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

} // namespace esl

#endif //ESL_UTILS_HPP

