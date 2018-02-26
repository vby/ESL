
#ifndef ESL_STRING_HPP
#define ESL_STRING_HPP

#include "macros.hpp"
#include "type_traits.hpp"
#include "memory.hpp"

#include <string>
#include <string_view>

namespace esl {

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

	template <class Traits, class Allocator>
	operator std::basic_string<CharT, Traits, Allocator>() const {
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

// Traits

// is_string_or_string_view, is_string_or_string_view_v
template <class T, class CharT = member_type_value_type_t<T, void>,
		 class Traits = member_type_traits_type_t<T, void>, class Allocator = member_type_allocator_type_t<T, void>>
struct is_string_or_string_view: std::is_base_of<std::basic_string<CharT, Traits, Allocator>, T> {};
template <class T, class CharT, class Traits>
struct is_string_or_string_view<T, CharT, Traits, void>: std::is_base_of<std::basic_string_view<CharT, Traits>, T> {};
template <class T, class Traits, class Allocator>
struct is_string_or_string_view<T, void, Traits, Allocator>: std::false_type {};
template <class T, class CharT, class Allocator>
struct is_string_or_string_view<T, CharT, void, Allocator>: std::false_type {};
template <class T, class Allocator>
struct is_string_or_string_view<T, void, void, Allocator>: std::false_type {};
template <class T, class Traits>
struct is_string_or_string_view<T, void, Traits, void>: std::false_type {};
template <class T, class CharT>
struct is_string_or_string_view<T, CharT, void, void>: std::false_type {};
template <class T>
struct is_string_or_string_view<T, void, void, void>: std::false_type {};
template <class T>
inline constexpr bool is_string_or_string_view_v = is_string_or_string_view<T>::value;

// string_view_of, string_view_of_t
template <class T, class Traits = typename T::traits_type>
struct string_view_of {
	using type = std::basic_string_view<typename T::value_type, Traits>;
};
template <class T, class Traits = typename T::traits_type>
using string_view_of_t = typename string_view_of<T, Traits>::type;

// string_of, string_of_t
template <class T, class Traits = typename T::traits_type,
		class Allocator = member_type_allocator_type_t<T, std::allocator<typename T::value_type>>>
struct string_of {
	using type = std::basic_string<typename T::value_type, Traits, Allocator>;
};
template <class T, class Traits = typename T::traits_type,
		class Allocator = member_type_allocator_type_t<T, std::allocator<typename T::value_type>>>
using string_of_t = typename string_of<T, Traits, Allocator>::type;

// to_string_view
template <class CharT, class Traits, class Allocator>
inline std::basic_string_view<CharT, Traits> to_string_view(const std::basic_string<CharT, Traits, Allocator>& s) noexcept {
	// operator std::basic_string_view<CharT, Traits>
	return s;
}
template <class CharT, class Traits = std::char_traits<CharT>>
inline std::basic_string_view<CharT, Traits> to_string_view(const CharT* s) noexcept {
	return std::basic_string_view<CharT, Traits>(s);
}

// to_string
template <class CharT, class Traits, class Allocator = std::allocator<CharT>>
inline std::basic_string<CharT, Traits, Allocator> to_string(const std::basic_string_view<CharT, Traits>& s) {
	return std::basic_string<CharT, Traits, Allocator>(s);
}
template <class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
inline std::basic_string<CharT, Traits, Allocator> to_string(const CharT* s) {
	return std::basic_string<CharT, Traits, Allocator>(s);
}

// substr return string_view
template <class CharT, class Traits, class T = std::basic_string_view<CharT, Traits>>
inline std::basic_string_view<CharT, Traits> substr(const std::basic_string_view<CharT, Traits>& s,
		typename T::size_type pos = 0, typename T::size_type count = T::npos) {
	return s.substr(pos, count);
}
template <class CharT, class Traits, class Allocator, class T = std::basic_string<CharT, Traits, Allocator>>
inline std::basic_string_view<CharT, Traits> substr(const std::basic_string<CharT, Traits, Allocator>& s,
		typename T::size_type pos = 0, typename T::size_type count = T::npos) {
	return std::basic_string_view<CharT, Traits>(s).substr(pos, count);
}
template <class CharT, class Traits = std::char_traits<CharT>, class T = std::basic_string_view<CharT, Traits>>
inline std::basic_string_view<CharT, Traits> substr(const CharT* s,
		typename T::size_type pos = 0, typename T::size_type count = T::npos) {
	return std::basic_string_view<CharT, Traits>(s).substr(pos, count);
}

// split by CharT
template <class CharT, class Traits, class OutputIt>
OutputIt split(const std::basic_string_view<CharT, Traits>& s, CharT delim, OutputIt d_first) {
	using size_type = typename std::basic_string_view<CharT, Traits>::size_type;
	size_type pos = 0;
	for (size_type i = 0; i < s.length(); ++i) {
		if (s[i] == delim) {
			*d_first = s.substr(pos, i - pos);
			++d_first;
			pos = i + 1;
		}
	}
	*d_first = s.substr(pos);
	++d_first;
	return d_first;
}
template <class CharT, class Traits, class Allocator, class OutputIt>
inline OutputIt split(const std::basic_string<CharT, Traits, Allocator>& s, CharT delim, OutputIt d_first) {
	return split(std::basic_string_view<CharT, Traits>(s), delim, d_first);
}
template <class CharT, class Traits = std::char_traits<CharT>, class OutputIt>
inline OutputIt split(const CharT* s, CharT delim, OutputIt d_first) {
	return split(std::basic_string_view<CharT, Traits>(s), delim, d_first);
}

// split by str
template <class CharT, class Traits, class OutputIt>
OutputIt split_(const std::basic_string_view<CharT, Traits>& s, const std::basic_string_view<CharT, Traits>& delim, OutputIt d_first) {
	using size_type = typename std::basic_string_view<CharT, Traits>::size_type;
	size_type pos = 0;
	do {
		size_type end_pos = s.find(delim, pos);
		if (end_pos == std::basic_string_view<CharT, Traits>::npos) {
			*d_first = s.substr(pos);
			++d_first;
			break;
		}
		*d_first = s.substr(pos, end_pos - pos);
		++d_first;
		pos = end_pos + delim.size();
	} while(true);
	return d_first;
}
template <class CharT, class Traits, class SV, class OutputIt>
inline OutputIt split(const std::basic_string_view<CharT, Traits>& s, const SV& delim, OutputIt d_first) {
	return split_(s, std::basic_string_view<CharT, Traits>(delim), d_first);
}
template <class CharT, class Traits, class Allocator, class SV, class OutputIt>
inline OutputIt split(const std::basic_string<CharT, Traits, Allocator>& s, const SV& delim, OutputIt d_first) {
	return split_(std::basic_string_view<CharT, Traits>(s), std::basic_string_view<CharT, Traits>(delim), d_first);
}
template <class CharT, class Traits = std::char_traits<CharT>, class SV, class OutputIt>
inline OutputIt split(const CharT* s, const SV& delim, OutputIt d_first) {
	return split_(std::basic_string_view<CharT, Traits>(s), std::basic_string_view<CharT, Traits>(delim), d_first);
}

// join
template <class Allocator, class CharT, class Traits, class InputIt>
std::basic_string<CharT, Traits, Allocator> join_(const std::basic_string_view<CharT, Traits>& delim, InputIt first, InputIt last) {
	std::basic_string<CharT, Traits, Allocator> str;
	if (first != last) {
		do {
			str.append(*first);
			str.append(delim);
			++first;
		} while (first != last);
		str.resize(str.size() - delim.size());
	}
	return std::move(str);
}
template <class CharT, class Allocator = std::allocator<CharT>, class Traits, class InputIt>
inline std::basic_string<CharT, Traits, Allocator> join_(const std::basic_string_view<CharT, Traits>& delim, InputIt first, InputIt last) {
	return join_<Allocator>(delim, first, last);
}
template <class CharT, class Traits, class Allocator, class InputIt>
inline std::basic_string<CharT, Traits, Allocator> join(const std::basic_string<CharT, Traits, Allocator>& delim, InputIt first, InputIt last) {
	return join_<Allocator>(std::basic_string_view<CharT, Traits>(delim), first, last);
}
template <class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>, class InputIt>
inline std::basic_string<CharT, Traits, Allocator> join(const CharT* delim, InputIt first, InputIt last) {
	return join_<Allocator>(std::basic_string_view<CharT, Traits>(delim), first, last);
}

} //namespace esl

#endif //ESL_STRING_HPP

