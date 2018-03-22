
#ifndef ESL_LITERAL_HPP
#define ESL_LITERAL_HPP

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
	template <> struct name<char> { static constexpr const char* value = s; }; \
	template <> struct name<wchar_t> { static constexpr const wchar_t* value = L##s; }; \
	template <> struct name<char16_t> { static constexpr const char16_t* value = u##s; }; \
	template <> struct name<char32_t> { static constexpr const char32_t* value = U##s; }; \
	template <class CharT> inline constexpr const CharT* name##_v = name<CharT>::value;

namespace esl {


} //namespace esl

#endif // ESL_LITERAL_HPP

