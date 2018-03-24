/*
 * This file contains code from boost under Boost Software License - Version 1.0,
 * http://www.boost.org/LICENSE_1_0.txt
 *
 */
#ifndef ESL_FUNCTIONAL_HPP
#define ESL_FUNCTIONAL_HPP

#include "intrin.hpp"

#include <cstdint>
#include <functional>
#include <string_view>
#include <utility>
#include <tuple>

namespace esl {

// hash_value
template <class T>
inline std::size_t hash_value(const T& value) noexcept {
	return std::hash<T>{}(value);
}
inline std::size_t hash_value(const void* p, std::size_t size) noexcept {
	return std::hash<std::string_view>{}(std::string_view(static_cast<const char*>(p), size));
}

#ifdef ESL_SIZE64

// Modified from boost/container_hash/hash.hpp
inline constexpr void hash_value_combine_(std::uint64_t& h, std::uint64_t k) noexcept {
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

#else

inline constexpr void hash_value_combine_(std::uint32_t& h, std::uint32_t k) noexcept {
	const std::uint32_t c1 = 0xcc9e2d51U;
	const std::uint32_t c2 = 0x1b873593U;

	k *= c1;
	k = rotl(k, 15);
	k *= c2;

	h ^= k;
	h = rotl(h, 13);
	h = h * 5 + 0xe6546b64U;
}

#endif

// hash_combiner
// operator|: combine two hash value
// operator+: combine hash value with a object
class hash_combiner {
private:
	std::size_t h_;

public:
	constexpr hash_combiner() noexcept: h_(0) {}

	constexpr hash_combiner(std::size_t h) noexcept: h_(h) {}

	constexpr std::size_t value() const noexcept { return h_; }

	constexpr operator std::size_t() const noexcept { return h_; }

	constexpr hash_combiner operator|(std::size_t k) const noexcept {
		std::size_t h = h_;
		hash_value_combine_(h, k);
		return h;
	}

	constexpr hash_combiner& operator|=(std::size_t k) noexcept {
		hash_value_combine_(h_, k);
		return *this;
	}

	template <class T>
	constexpr hash_combiner operator+(const T& value) const noexcept {
		std::size_t h = h_;
		hash_value_combine_(h, hash_value(value));
		return h;
	}

	template <class T>
	constexpr hash_combiner& operator+=(const T& value) noexcept {
		hash_value_combine_(h_, hash_value(value));
		return *this;
	}
};

// hash_value_combine
inline std::size_t hash_value_combine(std::size_t h, std::size_t k) noexcept {
	hash_value_combine_(h, k);
	return h;
}

// hash_combine
template <class T>
inline std::size_t hash_combine(std::size_t h, const T& value) noexcept {
	return hash_value_combine(h, hash_value(value));
}

} // namespace esl

namespace std {

// hash<std::pair<T1, T2>>
template <class T1, class T2>
struct hash<std::pair<T1, T2>> {
	std::size_t operator()(const std::pair<T1, T2>& p) const noexcept {
		return ::esl::hash_combine(::esl::hash_value(p.first), p.second);
	}
};

// hash<std::tuple<Ts..>>
template <class... Ts>
struct hash<std::tuple<Ts...>> {
	std::size_t operator()(const std::tuple<Ts...>& t) const noexcept {
		return hash_(t, std::make_index_sequence<sizeof...(Ts)>{});
	}
private:
	template <std::size_t... I>
	std::size_t hash_(const std::tuple<Ts...>& t, std::index_sequence<I...>) const noexcept {
		::esl::hash_combiner hc;
		return (hc += ... += std::get<I>(t));
	}
};

} // namespace std

#endif //ESL_FUNCTIONAL_HPP

