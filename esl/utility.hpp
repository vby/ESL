
#ifndef ESL_UTILITY_HPP
#define ESL_UTILITY_HPP

#include "macros.hpp"
#include "type_traits.hpp"
#include "array.hpp"

#include <cstdint>
#include <cstring>
#include <limits>
#include <array>
#include <utility>
#include <initializer_list>
#include <type_traits>
#include <string>

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

// make_overloaded
template <class... Ts>
inline overloaded<std::decay_t<Ts>...> make_overloaded(Ts&&... f) {
	return overloaded<std::decay_t<Ts>...>(std::forward<Ts>(f)...);
}

// make_tuple_vtable, make_tuple_vtable_t, make_tuple_vtable_v
template <template <class...> class F, class... Tups>
struct make_tuple_vtable {
	using type = md_array_t<decltype(&F<std::tuple_element_t<0, Tups>...>::value), std::tuple_size_v<Tups>...>;
private:
	template <std::size_t... Is>
	static constexpr void apply_alt(type& vt, std::index_sequence<Is...>) {
		std::get<Is...>(vt) = F<std::tuple_element_t<Is, Tups>...>::value;
	}
	template <class... IntSeq>
	struct Alts {
		static constexpr void apply(type&) {}
	};
	template <class IntSeq, class... Rest>
	struct Alts<IntSeq, Rest...> {
		static constexpr void apply(type& vt) {
			apply_alt(vt, IntSeq{});
			Alts<Rest...>::apply(vt);
		}
	};
	template <class... IntSeq>
	static constexpr type gen_vtable(std::tuple<IntSeq...>) {
		type vt{};
		Alts<IntSeq...>::apply(vt);
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
	using type = md_array_t<decltype(&F<AlwaysZero<Dimensions>::value...>::value), Dimensions...>;
private:
	template <std::size_t... Is>
	static constexpr void apply_alt(type& vt, std::index_sequence<Is...>) {
		std::get<Is...>(vt) = F<Is...>::value;
	}
	template <class... IntSeq>
	struct Alts {
		static constexpr void apply(type&) {}
	};
	template <class IntSeq, class... Rest>
	struct Alts<IntSeq, Rest...> {
		static constexpr void apply(type& vt) {
			apply_alt(vt, IntSeq{});
			Alts<Rest...>::apply(vt);
		}
	};
	template <class... IntSeq>
	static constexpr type gen_vtable(std::tuple<IntSeq...>) {
		type vt{};
		Alts<IntSeq...>::apply(vt);
		return vt;
	}
public:
	static constexpr type value = gen_vtable(make_index_sequence_combination<Dimensions...>{});
};
template <template <std::size_t...> class F, std::size_t... Dimensions>
using make_index_sequence_vtable_t = typename make_index_sequence_vtable<F, Dimensions...>::type;
template <template <std::size_t...> class F, std::size_t... Dimensions>
inline constexpr auto make_index_sequence_vtable_v = make_index_sequence_vtable<F, Dimensions...>::value;

// invert_integer_array
template <class T, std::size_t Size, std::size_t N, class It, class U = remove_rcv_t<decltype(*std::declval<It>())>>
constexpr std::array<T, Size> invert_integer_array(It&& first, std::initializer_list<std::pair<U, T>> kvs = {}) {
	static_assert(N <= std::numeric_limits<T>::max(), "target type is too small");
	std::array<T, Size> t_arr{};
	for (std::size_t i = 0; i < Size; ++i) {
		t_arr[i] = std::numeric_limits<T>::max();
	}
	using UU = std::make_unsigned_t<U>;
	for (std::size_t i = 0; i < N; ++i) {
		t_arr[static_cast<UU>(first[i])] = static_cast<T>(i);
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

// hex_alphabet_(lowercase|uppercase)
inline constexpr const char* hex_alphabet_lowercase = "0123456789abcdef";
inline constexpr const char* hex_alphabet_uppercase = "0123456789ABCDEF";

class hex_option {
private:
    std::array<char, 16> alphabet_;
    std::array<unsigned char, 256> alphabet_invert_;

public:
    constexpr hex_option(const char* alphabet) noexcept: alphabet_(make_sized_array<16>(alphabet)),
        alphabet_invert_(invert_integer_array<unsigned char, 256, 16>(alphabet)) {}

    // b [0, 16)
    constexpr char encode(unsigned char b) const noexcept { return alphabet_[b]; }

    // c [0, 256)
    constexpr unsigned char decode(char c) const noexcept { return alphabet_invert_[static_cast<unsigned char>(c)]; }
};

inline constexpr hex_option hex_lowercase{hex_alphabet_lowercase};
inline constexpr hex_option hex_uppercase{hex_alphabet_uppercase};

// hex_encode_size
inline constexpr std::size_t hex_encode_size(std::size_t size) noexcept { return size * 2; }

// hex_encode
inline constexpr std::size_t hex_encode(const void* buf, std::size_t size, char* out,
		const hex_option& option = hex_lowercase) noexcept {
	const unsigned char* in = static_cast<const unsigned char*>(buf);
	const unsigned char* const in_end = in + size;
	while (in != in_end) {
		const auto c = *in++;
		*out++ = option.encode(c >> 4);
		*out++ = option.encode(c & 0xF);
	}
	return hex_encode_size(size);
}
inline std::string hex_encode(const void* buf, std::size_t size, const hex_option& option = hex_lowercase) {
	std::string s;
	s.resize(hex_encode_size(size));
	hex_encode(buf, size, s.data(), option);
	return s;
}

template <class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>& hex_encode(std::basic_ostream<CharT, Traits>& os, const void* data, std::size_t size,
		const hex_option& option = hex_lowercase) {
	constexpr std::size_t chunk_size = 256;
	char buf[chunk_size * 2];
	const unsigned char* in = static_cast<const unsigned char*>(data);
	const unsigned char* const in_end = in + size;
	for (; in < in_end; in += chunk_size) {
		const auto n = std::min(static_cast<std::size_t>(in_end - in), chunk_size);
		os.write(buf, hex_encode(in, n, buf, option));
	}
	return os;
}

// TODO hex_decode

template <class CharT, class Traits, std::size_t N>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const std::array<unsigned char, N>& byte_arr) {
	if (os.flags() & std::ios_base::uppercase) {
		return hex_encode(os, byte_arr.data(), byte_arr.size(), hex_uppercase);
	}
	return hex_encode(os, byte_arr.data(), byte_arr.size());
}

} // namespace esl

#endif //ESL_UTILITY_HPP

