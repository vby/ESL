
#ifndef ESL_STRING_HPP
#define ESL_STRING_HPP

#include "macros.hpp"
#include "type_traits.hpp"
#include "memory.hpp"

#include <string>
#include <string_view>
#include <regex>
#include <utility>
#include <cinttypes>

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


/// traits ///

// is_char, is_char_v
template <class T>
using is_char_ = is_one_of<T, char, wchar_t, char16_t, char32_t>;
template <class T>
using is_char = is_char_<std::remove_cv_t<T>>;
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

// is_zstring, is_zstring_v
template <class T>
using is_zstring_ = std::bool_constant<std::is_pointer_v<T> && is_char_v<remove_pcv_t<T>>>;
template <class T>
using is_zstring = is_zstring_<std::decay_t<T>>;
template <class T>
inline constexpr bool is_zstring_v = is_zstring<T>::value;

// is_pair_string, is_pair_string_v
template <class T, bool = is_base_of_template_v<std::pair, T>>
struct is_pair_string_: std::false_type {};
template <class T>
struct is_pair_string_<T, true>: std::bool_constant<is_zstring_v<typename T::first_type> && (std::is_same_v<typename T::first_type, typename T::second_type>
		|| std::is_unsigned_v<typename T::second_type>)> {};
template <class T>
using is_pair_string = is_pair_string_<std::remove_cv_t<T>>;
template <class T>
inline constexpr bool is_pair_string_v = is_pair_string<T>::value;

// is_string_type, is_string_type_v
template <class T>
using is_string_type = std::bool_constant<is_zstring_v<T> || is_string_or_string_view_v<T> || is_pair_string_v<T>>;
template <class T>
inline constexpr bool is_string_type_v = is_string_type<T>::value;

// char_as_zstring
template <class T>
using char_as_zstring = std::conditional<is_char_v<std::decay_t<T>>, std::add_pointer_t<T>, T>;
template <class T>
using char_as_zstring_t = typename char_as_zstring<T>::type;

// string_view_of, string_view_of_t
template <class T>
struct string_view_of_ {
	using type = std::basic_string_view<typename T::value_type, typename T::traits_type>;
};
template <class CharT>
struct string_view_of_<CharT*> {
	using type = std::basic_string_view<std::remove_cv_t<CharT>>;
};
template <class T, bool = is_zstring_v<T> || is_string_or_string_view_v<T>, bool = is_pair_string_v<T>>
struct string_view_of;
template <class T>
struct string_view_of<T, true, false> {
	using type = typename string_view_of_<std::decay_t<T>>::type;
};
template <class T>
struct string_view_of<T, false, true> {
	using type = std::basic_string_view<remove_pcv_t<std::decay_t<typename T::first_type>>>;
};
template <class T>
using string_view_of_t = typename string_view_of<T>::type;

// string_of, string_of_t (ignoring cv)
template <class T, class StrV = string_view_of_t<T>>
struct string_of_ {
	using type = std::basic_string<typename StrV::value_type, typename StrV::traits_type, member_type_allocator_type_t<T, std::allocator<typename StrV::value_type>>>;
};
template <class T>
struct string_of {
	using type = typename string_of_<std::decay_t<T>>::type;
};
template <class T>
using string_of_t = typename string_of<T>::type;

// char_of, char_of_v
template <class CharT, char ch>
struct char_of: std::integral_constant<CharT, ch> {};
template <class CharT, char ch>
inline constexpr CharT char_of_v = char_of<CharT, ch>::value;


/// functions ///

// make_string_view
template <class S, class StrV = string_view_of_t<S>, class = std::enable_if_t<std::is_constructible_v<StrV, const S&>>>
inline StrV make_string_view(const S& s) noexcept { return StrV(s); }

template <class CharT, class StrV = std::basic_string_view<CharT>>
inline StrV make_string_view(const CharT* s, typename StrV::size_type count) noexcept { return StrV(s, count); }

template <class CharT, class StrV = std::basic_string_view<CharT>>
inline StrV make_string_view(const CharT* first, const CharT* last) noexcept { return StrV(first, last - first); }

template <class Pair, class = std::enable_if_t<is_pair_string_v<Pair>>>
inline decltype(auto) make_string_view(const Pair& p) noexcept { return make_string_view(p.first, p.second); }

template <class CharT, class = std::enable_if_t<is_char_v<CharT>>, class StrV = std::basic_string_view<CharT>>
inline StrV make_string_view(const CharT& c) noexcept { return StrV(&c, 1); }

// make_string
template <class S, class Str = string_of_t<S>>
inline Str make_string(const S& s) { return Str(s); }

template <class S, class Str = string_of_t<S>>
inline Str make_string(S&& s) { return Str(std::move(s)); }

// from
// generic c++17 std::from_chars
// TODO float
template <class S, class T, class StrV = string_view_of_t<S>, class = std::enable_if_t<std::is_integral_v<T>>>
constexpr std::pair<bool, typename StrV::iterator> from(const S& s, T& value, int base = 10) {
	using CharT = typename StrV::value_type;
	using Traits = typename StrV::traits_type;
	auto v = make_string_view(s);
	auto it = v.begin();
	auto last = v.end();
	if (base < 2 || it == last) {
		return {false, it};
	}
	auto begin = v.begin();
	bool neg = false;
	if constexpr (std::is_signed_v<T>) {
		if (Traits::eq(*it, char_of_v<CharT, '-'>)) {
			neg = true;
			++begin;
			++it;
		}
	}
	const T cutoff = std::numeric_limits<T>::max() / base;
	const int cutlimit = static_cast<int>(std::numeric_limits<T>::max() % base);
	T val = 0;
	while (it != last) {
		const auto ch = *it;
		int chval = 0;
		if (!Traits::lt(ch, char_of_v<CharT, '0'>) && !Traits::lt(char_of_v<CharT, '9'>, ch)) {
			chval = ch - char_of_v<CharT, '0'>;
		} else if (!Traits::lt(ch, char_of_v<CharT, 'a'>) && !Traits::lt(char_of_v<CharT, 'z'>, ch)) {
			chval = ch - char_of_v<CharT, 'a'> + 10;
		} else if (!Traits::lt(ch, char_of_v<CharT, 'A'>) && !Traits::lt(char_of_v<CharT, 'Z'>, ch)) {
			chval = ch - char_of_v<CharT, 'A'> + 10;
		} else {
			break;
		}
		if (chval >= base) {
			break;
		}
		if (val > cutoff || (val == cutoff && chval > cutlimit)) {
			return {false, it};
		}
		val = val * base + chval;
		++it;
	}
	if (it == begin) {
		return {false, it};
	}
	value = neg ? -val : val;
	return {true, it};
}

// split
template <class S, class OutputIt, class StrV = string_view_of_t<S>>
OutputIt split(const S& s, typename StrV::value_type delim, OutputIt d_first) {
	auto v = make_string_view(s);
	typename StrV::size_type pos = 0;
	const auto size = s.length();
	for (typename StrV::size_type i = 0; i < size; ++i) {
		if (StrV::traits_type::eq(v[i], delim)) {
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
OutputIt split(const S& s, const typename StrVOf::type& dv, OutputIt d_first) {
	auto v = make_string_view(s);
	typename StrV::size_type pos = 0;
	do {
		typename StrV::size_type end_pos = v.find(dv, pos);
		if (end_pos == StrV::npos) {
			*d_first = v.substr(pos);
			++d_first;
			break;
		}
		*d_first = v.substr(pos, end_pos - pos);
		++d_first;
		pos = end_pos + dv.size();
	} while(true);
	return d_first;
}

// join
template <class S, class InputIt, class Str = string_of_t<char_as_zstring_t<S>>>
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
	return s;
}

} //namespace esl

#endif //ESL_STRING_HPP

