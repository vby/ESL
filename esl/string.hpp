
#ifndef ESL_STRING_HPP
#define ESL_STRING_HPP

#include "macros.hpp"
#include "type_traits.hpp"
#include "memory.hpp"

#include <string>
#include <string_view>

namespace esl {

// constexpr_strlen, constexpr_wcslen
template <class CharT>
inline constexpr std::size_t constexpr_strlen_(const CharT* s, std::size_t len = 0) {
	return *s == CharT(0) ? len : constexpr_strlen_(s + 1, len + 1);
}
inline constexpr std::size_t constexpr_strlen(const char* s) {
#ifdef ESL_COMPILER_GNU
	return __builtin_strlen(s);
#else
	return constexpr_strlen_(s);
#endif
}
inline constexpr std::size_t constexpr_wcslen(const wchar_t* s) {
	return constexpr_strlen_(s);
}

#define ESL_CONSTEXPR_STRCMP_OR_(l, r) (*l == '\0' || *l != *r) ? static_cast<int>(*l - *r)

// constexpr_strcmp, constexpr_wcscmp
template <class CharT>
inline constexpr int constexpr_strcmp_(const CharT* l, const CharT* r) {
	return ESL_CONSTEXPR_STRCMP_OR_(l, r) : constexpr_strcmp_(l + 1, r + 1);
}
inline constexpr int constexpr_strcmp(const char* l, const char* r) {
#ifdef ESL_COMPILER_GNU
	return __builtin_strcmp(l, r);
#else
	return constexpr_strcmp_(l, r);
#endif
}
inline constexpr int constexpr_wcscmp(const wchar_t* l, const wchar_t* r) {
	return constexpr_strcmp_(l, r);
}
// constexpr_strncmp, constexpr_wcsncmp
template <class CharT>
inline constexpr int constexpr_strncmp_(const CharT* l, const CharT* r, std::size_t count) {
	return count == 0 ? 0 : (ESL_CONSTEXPR_STRCMP_OR_(l, r) : constexpr_strncmp_(l + 1, r + 1, count - 1));
}
inline constexpr int constexpr_strncmp(const char* l, const char* r, std::size_t count) {
#ifdef ESL_COMPILER_GNU
	return __builtin_strncmp(l, r, count);
#else
	return constexpr_strncmp_(l, r, count);
#endif
}
inline constexpr int constexpr_wcsncmp(const wchar_t* l, const wchar_t* r, std::size_t count) {
	return constexpr_strncmp_(l, r, count);
}

// basic_unique_chars
// A unique_ptr of CharT[] that can be cast to std::basic_string or std::basic_string_view
template <class CharT, class Deleter = std::default_delete<CharT[]>>
struct basic_unique_chars: public std::unique_ptr<CharT[], Deleter> {
private:
	using unique_ptr = std::unique_ptr<CharT[], Deleter>;

public:
	using unique_ptr::unique_ptr;

	template <class Traits>
	explicit operator std::basic_string_view<CharT, Traits>() const noexcept {
		return (*this) ? this->get() : "";
	}

	template <class Traits, class Alloc>
	operator std::basic_string<CharT, Traits, Alloc>() const {
		return (*this) ? this->get() : "";
	}
};

using unique_chars = basic_unique_chars<char>;
using wunique_chars = basic_unique_chars<wchar_t>;
using u16unique_chars = basic_unique_chars<char16_t>;
using u32unique_chars = basic_unique_chars<char32_t>;

using c_unique_chars = basic_unique_chars<char, c_free_delete>;
using c_wunique_chars = basic_unique_chars<wchar_t, c_free_delete>;
using c_u16unique_chars = basic_unique_chars<char16_t, c_free_delete>;
using c_u32unique_chars = basic_unique_chars<char32_t, c_free_delete>;

namespace details {

template <class T>
struct string_of {
	using string_type = std::basic_string<typename T::value_type, typename T::traits_type, member_type_allocator_type_t<T, std::allocator<typename T::value_type>>>;
	using string_view_type = std::basic_string_view<typename T::value_type, typename T::traits_type>;
};
template <class CharT>
struct string_of<CharT*> {
	using string_type = std::basic_string<std::remove_const_t<CharT>>;
	using string_view_type = std::basic_string_view<std::remove_const_t<CharT>>;
};

} // namespace details

// is_char, is_char_v
template <class T>
using is_char = is_one_of<T, char, wchar_t, char16_t, char32_t>;
template <class T>
inline constexpr bool is_char_v = is_char<T>::value;

// is_string, is_string_v
template <class T>
using is_string = is_base_of_template<std::basic_string, T>;
template <class T>
inline constexpr bool is_string_v = is_string<T>::value;

// is_string_view, is_string_view_v
template <class T>
using is_string_view = is_base_of_template<std::basic_string_view, T>;
template <class T>
inline constexpr bool is_string_view_v = is_string_view<T>::value;

// is_string_or_string_view, is_string_or_string_view_v
template <class T>
using is_string_or_string_view = std::bool_constant<is_string_v<T> || is_string_view_v<T>>;
template <class T>
inline constexpr bool is_string_or_string_view_v = is_string_or_string_view<T>::value;

// is_cstring, is_cstring_v
template <class T>
using is_cstring_ = std::bool_constant<std::is_pointer_v<T> && is_char_v<remove_cv_pointer_t<T>>>;
template <class T>
using is_cstring = is_cstring_<std::decay_t<T>>;
template <class T>
inline constexpr bool is_cstring_v = is_cstring<T>::value;

// is_string_type, is_string_type_v
template <class T>
struct is_string_type: std::bool_constant<is_cstring_v<T> || is_string_or_string_view_v<T>> {};
template <class T>
inline constexpr bool is_string_type_v = is_string_type<T>::value;

// string_of, string_of_t
// SFINAE friendly
template <class T, bool = is_string_type_v<std::decay_t<T>>>
struct string_of;
template <class T>
struct string_of<T, true> {
	using type = typename details::string_of<std::decay_t<T>>::string_type;
};
template <class T>
using string_of_t = typename string_of<T>::type;

// string_view_of, string_view_of_t
// SFINAE friendly
template <class T, bool = is_string_type_v<std::decay_t<T>>>
struct string_view_of;
template <class T>
struct string_view_of<T, true> {
	using type = typename details::string_of<std::decay_t<T>>::string_view_type;
};
template <class T>
using string_view_of_t = typename string_view_of<T>::type;

// make_string_view
template <class S, class StrV = string_view_of_t<S>>
inline StrV make_string_view(const S& s) noexcept { return StrV(s); }

template <class S, class StrV = string_view_of_t<S>>
inline StrV make_string_view(const S& s, typename StrV::size_type pos, typename StrV::size_type count = StrV::npos) noexcept {
	return StrV(s).substr(pos, count);
}

// make_string
template <class S, class Str = string_of_t<S>>
inline Str make_string(const S& s) { return Str(s); }

template <class S, class Str = string_of_t<S>>
inline Str make_string(S&& s) { return Str(std::move(s)); }

template <class S, class Str = string_of_t<S>, class StrV = string_view_of_t<S>>
inline Str make_string(const S& s, typename StrV::size_type pos, typename StrV::size_type count = StrV::npos) {
	return Str(make_string_view(s, pos, count));
}

// split
template <class S, class OutputIt, class StrV = string_view_of_t<S>>
OutputIt split(const S& s, typename StrV::value_type delim, OutputIt d_first) {
	auto v = make_string_view(s);
	typename StrV::size_type pos = 0;
	for (typename StrV::size_type i = 0; i < s.length(); ++i) {
		if (s[i] == delim) {
			*d_first = v.substr(pos, i - pos);
			++d_first;
			pos = i + 1;
		}
	}
	*d_first = v.substr(pos);
	++d_first;
	return d_first;
}
template <class S, class OutputIt, class StrVOf = string_view_of<S>, class StrV = typename StrVOf::type>
OutputIt split(const S& s, const typename StrVOf::type& delim, OutputIt d_first) {
	auto v = make_string_view(s);
	typename StrV::size_type pos = 0;
	do {
		typename StrV::size_type end_pos = v.find(delim, pos);
		if (end_pos == StrV::npos) {
			*d_first = v.substr(pos);
			++d_first;
			break;
		}
		*d_first = v.substr(pos, end_pos - pos);
		++d_first;
		pos = end_pos + delim.size();
	} while(true);
	return d_first;
}

// join
template <class S, class InputIt, class Str = string_of_t<S>>
Str join(const S& delim, InputIt first, InputIt last) {
	auto dv = make_string_view(delim);
	Str s;
	if (first != last) {
		do {
			s.append(*first);
			s.append(dv);
			++first;
		} while (first != last);
		s.resize(s.size() - dv.size());
	}
	return std::move(s);
}

} //namespace esl

#endif //ESL_STRING_HPP

