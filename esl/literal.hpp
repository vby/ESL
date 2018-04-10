
#ifndef ESL_LITERAL_HPP
#define ESL_LITERAL_HPP

#include "array.hpp"

#include <utility>

// macros

// ESL_IMPL_CHAR_LITERAL_CONSTANT
// name, name##_v
#define ESL_IMPL_CHAR_LITERAL_CONSTANT(name, c) \
	template <class CharT> struct name; \
	template <> struct name<char>: std::integral_constant<char, c> {}; \
	template <> struct name<wchar_t>: std::integral_constant<wchar_t, L##c> {}; \
	template <> struct name<char16_t>: std::integral_constant<char16_t, u##c> {}; \
	template <> struct name<char32_t>: std::integral_constant<char32_t, U##c> {}; \
	template <class CharT> inline constexpr CharT name##_v = name<CharT>::value;

// ESL_IMPL_STRING_LITERAL_CONSTANT
// name, name##_v
#define ESL_IMPL_STRING_LITERAL_CONSTANT(name, s) \
	template <class CharT> struct name; \
	inline constexpr char name##_char[] = s; \
	template <> struct name<char> { \
		using type = std::array<char, array_size_v<decltype(name##_char)>>; \
		static constexpr type value = make_array(name##_char); \
	}; \
	inline constexpr wchar_t name##_wchar[] = L##s; \
	template <> struct name<wchar_t> { \
		using type = std::array<wchar_t, array_size_v<decltype(name##_wchar)>>; \
		static constexpr type value = make_array(name##_wchar); \
	}; \
	inline constexpr char16_t name##_char16[] = u##s; \
	template <> struct name<char16_t> { \
		using type = std::array<char16_t, array_size_v<decltype(name##_char16)>>; \
		static constexpr type value = make_array(name##_char16); \
	}; \
	inline constexpr char32_t name##_char32[] = U##s; \
	template <> struct name<char32_t> { \
		using type = std::array<char32_t, array_size_v<decltype(name##_char32)>>; \
		static constexpr type value = make_array(name##_char32); \
	}; \
	template <class CharT> inline constexpr typename name<CharT>::type name##_v = name<CharT>::value;

namespace esl {

//NOTE:
// char128_constant and char_constant use for convert char to CharT in compile time
// The behavior is unspecified if the char is not printable.

// char128_constant, char128_constant_v
ESL_IMPL_STRING_LITERAL_CONSTANT(char128_constant,
		"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
		" !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7F")

// char_constant, char_constant_v
template <class CharT, char c>
struct char_constant: std::integral_constant<CharT, char128_constant_v<CharT>[static_cast<unsigned char>(c)]> {};
template <class CharT, char c>
inline constexpr CharT char_constant_v = char_constant<CharT, c>::value;

} //namespace esl

#endif // ESL_LITERAL_HPP

