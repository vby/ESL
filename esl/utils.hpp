
#ifndef ESL_UTILS_HPP
#define ESL_UTILS_HPP

#include <cstdint>
#include <limits>
#include <array>
#include <utility>

namespace esl {

template <class T, std::size_t Size, std::size_t N, class U>
constexpr std::array<T, Size> transpose_integer_array(U* arr) {
	std::array<T, Size> t_arr{};
	for (std::size_t i = 0; i < Size; ++i) {
		t_arr[i] = std::numeric_limits<T>::max();
	}
	using UU = std::make_unsigned_t<U>;
	for (std::size_t i = 0; i < N; ++i) {
		t_arr[static_cast<UU>(arr[i])] = static_cast<T>(i);
	}
	return t_arr;
}

template <class T, std::size_t Size, class U, std::size_t N>
constexpr std::array<T, Size> transpose_integer_array(U (&arr)[N]) {
	return transpose_integer_array<T, Size, N>(arr);
}

template <class T, std::size_t Size, class U, std::size_t N>
constexpr std::array<T, Size> transpose_integer_array(const std::array<T, N>& arr) {
	return transpose_integer_array<T, Size, N>(arr);
}

} // namespace esl

#endif //ESL_UTILS_HPP

