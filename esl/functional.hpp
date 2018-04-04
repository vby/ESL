/*
 * This file contains code from boost under Boost Software License - Version 1.0,
 * http://www.boost.org/LICENSE_1_0.txt
 *
 */
#ifndef ESL_FUNCTIONAL_HPP
#define ESL_FUNCTIONAL_HPP

#include "limits.hpp"
#include "intrin.hpp"

#include <cstdint>
#include <functional>
#include <string_view>
#include <utility>
#include <tuple>

#include <vector>
#include <unordered_map>

namespace esl {

// hash_value
template <class T>
inline std::size_t hash_value(const T& value) noexcept(noexcept(std::declval<std::hash<T>>()(std::declval<const T&>()))) {
	return std::hash<T>{}(value);
}
inline std::size_t hash_value(const void* p, std::size_t size) noexcept {
	return std::hash<std::string_view>{}(std::string_view(static_cast<const char*>(p), size));
}

#if (ESL_SIZE_WIDTH == 64)

// Modified from boost/container_hash/hash.hpp
inline constexpr void hash_value_combine_(std::size_t& h, std::size_t k) noexcept {
	const std::size_t m = 0xc6a4a7935bd1e995U;

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

inline constexpr void hash_value_combine_(std::size_t& h, std::size_t k) noexcept {
	const std::size_t c1 = 0xcc9e2d51U;
	const std::size_t c2 = 0x1b873593U;

	k *= c1;
	k = rotl(k, 15);
	k *= c2;

	h ^= k;
	h = rotl(h, 13);
	h = h * 5 + 0xe6546b64U;
}

#endif

// hashval
// operator|: combine two hash value
// operator+: combine hash value with a object
class hashval {
private:
	std::size_t h_;

public:
	constexpr hashval(): h_(0) {}

	constexpr hashval(std::size_t h): h_(h) {}

	constexpr std::size_t value() const { return h_; }

	constexpr operator std::size_t() const { return h_; }

	constexpr hashval operator|(std::size_t k) const {
		std::size_t h = h_;
		hash_value_combine_(h, k);
		return h;
	}

	constexpr hashval& operator|=(std::size_t k) {
		hash_value_combine_(h_, k);
		return *this;
	}

	template <class T>
	constexpr hashval operator+(const T& value) const {
		std::size_t h = h_;
		hash_value_combine_(h, ::esl::hash_value(value));
		return h;
	}

	template <class T>
	constexpr hashval& operator+=(const T& value) {
		hash_value_combine_(h_, ::esl::hash_value(value));
		return *this;
	}
};

// hash_value_combine
inline constexpr std::size_t hash_value_combine(std::size_t h, std::size_t k) {
	hash_value_combine_(h, k);
	return h;
}

// hash_combine
template <class T>
inline std::size_t hash_combine(std::size_t h, const T& value) {
	return hash_value_combine(h, ::esl::hash_value(value));
}

} // namespace esl

namespace std {

// hash<std::pair<T1, T2>>
template <class T1, class T2>
struct hash<std::pair<T1, T2>> {
	std::size_t operator()(const std::pair<T1, T2>& p) const {
		return ::esl::hash_combine(::esl::hash_value(p.first), p.second);
	}
};

// hash<std::tuple<Ts..>>
template <class... Ts>
struct hash<std::tuple<Ts...>> {
	std::size_t operator()(const std::tuple<Ts...>& t) const {
		return hash_(t, std::make_index_sequence<sizeof...(Ts)>{});
	}
private:
	template <std::size_t... I>
	std::size_t hash_(const std::tuple<Ts...>& t, std::index_sequence<I...>) const {
		::esl::hashval h;
		return (h += ... += std::get<I>(t));
	}
};

// hash<std::vector>
template <class T, class Alloc>
struct hash<std::vector<T, Alloc>> {
	std::size_t operator()(const std::vector<T, Alloc>& vec) const {
		::esl::hashval h;
		for (auto& v: vec) {
			h += v;
		}
		return h;
	}
};
template <class Key, class T, class Hash, class KeyEqual, class Alloc>
struct hash<std::unordered_map<Key, T, Hash, KeyEqual, Alloc>> {
	std::size_t operator()(const std::unordered_map<Key, T, Hash, KeyEqual, Alloc>& m) const {
		::esl::hashval h;
		for (auto& v: m) {
			h += v;
		}
		return h;
	}
};

// TODO more container

} // namespace std

#endif //ESL_FUNCTIONAL_HPP

