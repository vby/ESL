#ifndef ESL_FUNCTIONAL_HPP
#define ESL_FUNCTIONAL_HPP

#include "macros.hpp"

#include <cstdint>
#include <functional>
#include <utility>
#include <tuple>

namespace esl {

// hash_value
template <class T>
inline std::size_t hash_value(const T& value) noexcept {
	return std::hash<T>{}(value);
}

// hash_value_combine_
// Modified from boost/container_hash/hash.hpp
inline void hash_value_combine_(std::uint64_t& h, std::uint64_t k) noexcept {
	const std::uint64_t m = 0xc6a4a7935bd1e995U;

	k *= m;
	k ^= k >> 47;
	k *= m;

	h ^= k;
	h *= m;

	// Completely arbitrary number, to prevent 0's
	// from hashing to 0.
	h += 0xe6546b64U;
}

inline void hash_value_combine_(std::uint32_t& h, std::uint32_t k) noexcept {
	const std::uint32_t c1 = 0xcc9e2d51U;
	const std::uint32_t c2 = 0x1b873593U;

	k *= c1;
	k = ESL_ROTL32(k, 15);
	k *= c2;

	h ^= k;
	h = ESL_ROTL32(h, 13);
	h = h * 5 + 0xe6546b64U;
}

template <class Int, class Int2>
inline void hash_value_combine_(Int& h, Int2 k) noexcept {
	h ^= k + 0x9e3779b9 + (h << 6) + (h >> 2);
}

// hash_value_combine
template <class Int, class... Ints>
inline Int hash_value_combine(Int h, Ints... ks) noexcept {
	(..., hash_value_combine_(h, ks));
	return h;
}

// hash_combine
template <class... Ts>
inline std::size_t hash_combine(std::size_t h, const Ts&... ts) noexcept {
	return hash_value_combine(h, hash_value(ts)...);
}

} // namespace esl

namespace std {

// hash<std::pair<T1, T2>>
template <class T1, class T2>
struct hash<std::pair<T1, T2>> {
	std::size_t operator()(const std::pair<T1, T2>& p) const noexcept {
		std::size_t h = ::esl::hash_value(p.first);
		return ::esl::hash_combine(h, p.second);
	}
};

template <class... Ts>
struct hash<std::tuple<Ts...>> {
	std::size_t operator()(const std::tuple<Ts...>& t) const noexcept {
		return hash_(t, std::make_index_sequence<sizeof...(Ts)>{});
	}
private:
	template <std::size_t... I>
	std::size_t hash_(const std::tuple<Ts...>& t, std::index_sequence<I...>) const noexcept {
		return ::esl::hash_combine(0, std::get<I>(t)...);
	}
};

} // namespace std

#endif //ESL_FUNCTIONAL_HPP

