
#ifndef ESL_STRING_HPP
#define ESL_STRING_HPP

#include "macros.hpp"
#include "type_traits.hpp"
#include "memory.hpp"
#include "literal.hpp"
#include "locale.hpp"

#include <string>
#include <string_view>
#include <utility>
#include <cinttypes>
#include <sstream>
#include <regex>
#include <stdexcept>
#include <locale>

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

// is_stdchar, is_stdchar_v
template <class T>
using is_stdchar_ = is_one_of<T, char, wchar_t, char16_t, char32_t>;
template <class T>
using is_stdchar = is_stdchar_<std::remove_cv_t<T>>;
template <class T>
inline constexpr bool is_stdchar_v = is_stdchar<T>::value;

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

// is_stdzstring, is_stdzstring_v
template <class T>
using is_stdzstring_ = std::bool_constant<std::is_pointer_v<T> && is_stdchar_v<remove_pcv_t<T>>>;
template <class T>
using is_stdzstring = is_stdzstring_<std::decay_t<T>>;
template <class T>
inline constexpr bool is_stdzstring_v = is_stdzstring<T>::value;

// is_string_type, is_string_type_v
template <class T>
using is_string_type = std::bool_constant<is_stdzstring_v<T> || is_string_v<T> || is_string_view_v<T>>;
template <class T>
inline constexpr bool is_string_type_v = is_string_type<T>::value;

// stdchar_as_stdzstring
template <class T>
using stdchar_as_stdzstring = std::conditional<is_stdchar_v<std::decay_t<T>>, std::add_pointer_t<T>, T>;
template <class T>
using stdchar_as_stdzstring_t = typename stdchar_as_stdzstring<T>::type;

// string_traits

namespace details {

template <class T, class Enable = void>
struct string_traits: std::false_type {};
template <class T>
struct string_traits<T, std::enable_if_t<is_stdzstring_v<T>>>: std::true_type {
	using value_type = remove_pcv_t<std::decay_t<T>>;
	using traits_type = std::char_traits<value_type>;
	using allocator_type = std::allocator<value_type>;
};
template <class T>
struct string_traits<T, std::enable_if_t<is_string_v<T>>>: std::true_type {
	using value_type = typename T::value_type;
	using traits_type = typename T::traits_type;
	using allocator_type = typename T::allocator_type;
};
template <class T>
struct string_traits<T, std::enable_if_t<is_string_view_v<T>>>: std::true_type {
	using value_type = typename T::value_type;
	using traits_type = typename T::traits_type;
	using allocator_type = std::allocator<value_type>;
};

} // namespace details

template <class T, bool = details::string_traits<std::remove_cv_t<T>>::value>
struct string_traits;
template <class T>
struct string_traits<T, true> {
private:
	using base_type = details::string_traits<std::remove_cv_t<T>>;
public:
	using value_type = typename base_type::value_type;
	using traits_type = typename base_type::traits_type;
	using allocator_type = typename base_type::allocator_type;
	using string_type = std::basic_string<value_type, traits_type, allocator_type>;
	using string_view_type = std::basic_string_view<value_type, traits_type>;
};

/// functions ///

// from_string
// Generic c++17 std::from_chars
// TODO float
enum class from_string_errc {
	success = 0,
	invalid_argument = static_cast<int>(std::errc::invalid_argument),
	result_out_of_range = static_cast<int>(std::errc::result_out_of_range),
};

template <class S, class T, class STraits = string_traits<S>, class = std::enable_if_t<std::is_integral_v<T>>>
constexpr std::pair<from_string_errc, std::size_t> from_string(const S& s, T& value, int base = 10) noexcept {
	using char_type = typename STraits::value_type;
	using traits_type = typename STraits::traits_type;
	const typename STraits::string_view_type v(s);
	const auto first = v.data();
	const auto last = first + v.size();
	if (base < 2 || first == last) {
		return {from_string_errc::invalid_argument, 0};
	}
	auto it = first;
	auto begin = it;
	[[maybe_unused]] bool neg = false;
	if constexpr (std::is_signed_v<T>) {
		if (traits_type::eq(*it, ascii_constant_v<char_type, '-'>)) {
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
		if (!traits_type::lt(ch, ascii_constant_v<char_type, '0'>) && !traits_type::lt(ascii_constant_v<char_type, '9'>, ch)) {
			chval = ch - ascii_constant_v<char_type, '0'>;
		} else if (!traits_type::lt(ch, ascii_constant_v<char_type, 'a'>) && !traits_type::lt(ascii_constant_v<char_type, 'z'>, ch)) {
			chval = ch - ascii_constant_v<char_type, 'a'> + 10;
		} else if (!traits_type::lt(ch, ascii_constant_v<char_type, 'A'>) && !traits_type::lt(ascii_constant_v<char_type, 'Z'>, ch)) {
			chval = ch - ascii_constant_v<char_type, 'A'> + 10;
		} else {
			break;
		}
		if (chval >= base) {
			break;
		}
		if (val > cutoff || (val == cutoff && chval > cutlimit)) {
			return {from_string_errc::result_out_of_range, it - first};
		}
		val = val * base + chval;
		++it;
	}
	if (it == begin) {
		return {from_string_errc::invalid_argument, it - first};
	}
	if constexpr (std::is_signed_v<T>) {
		value = neg ? -val : val;
	} else {
		value = val;
	}
	return {from_string_errc::success, it - first};
}

// split
template <class T, class S, class OutputIt>
OutputIt split_(const T& v, const S& d, std::size_t dn, OutputIt d_first) {
	typename T::size_type pos = 0;
	do {
		typename T::size_type end_pos = v.find(d, pos);
		if (end_pos == T::npos) {
			*d_first = v.substr(pos);
			++d_first;
			break;
		}
		*d_first = v.substr(pos, end_pos - pos);
		++d_first;
		pos = end_pos + dn;
	} while(true);
	return d_first;
}
template <class T, class OutputIt, class STraits = string_traits<T>>
OutputIt split(const T& s, typename STraits::value_type delim, OutputIt d_first) {
	return split_(typename STraits::string_view_type(s), delim, 1, d_first);
}
template <class T, class OutputIt, class STraits = string_traits<T>>
OutputIt split(const T& s, const typename STraits::string_view_type& delim, OutputIt d_first) {
	return split_(typename STraits::string_view_type(s), delim, delim.size(), d_first);
}

// join
template <class T, class InputIt, class STraits = string_traits<T>>
typename STraits::string_type join(const T& s, InputIt first, InputIt last) {
	typename STraits::string_view_type v(s);
	typename STraits::string_type rs;
	if (first != last) {
		do {
			rs.append(*first);
			rs.append(v);
			++first;
		} while (first != last);
		rs.resize(rs.size() - v.size());
	}
	return rs;
}
template <class CharT, class InputIt, class = std::enable_if_t<is_stdchar_v<CharT>>>
std::basic_string<CharT> join(CharT d, InputIt first, InputIt last) {
	return join(std::basic_string_view<CharT>(&d, 1), first, last);
}


/// format ///
// Similar to python's str.format
// See https://docs.python.org/3.5/library/string.html#formatstrings
// NOTE: This implemention is locale aware

// bad_format

class bad_format: std::runtime_error {
public:
	using size_type = typename std::string::size_type;
	static constexpr size_type npos = std::string::npos;

private:
	size_type pos_;

public:
	bad_format(const char* what, size_type pos = npos): runtime_error(what), pos_(pos) {}

	bad_format(const std::string& what, size_type pos = npos): runtime_error(what), pos_(pos) {}

	size_type position() const noexcept { return pos_; }
};

namespace details {

using format_xflag = unsigned int;
struct format_xflags {
	static constexpr format_xflag center = 0x1;
	static constexpr format_xflag space_sign = 0x2;
	static constexpr format_xflag grouping = 0x4;
	static constexpr format_xflag binary = 0x8;
	static constexpr format_xflag character = 0x10;
	static constexpr format_xflag number = 0x20;
	static constexpr format_xflag percent = 0x40;
};

template <class CharT, class Traits, class T>
static void format_out(std::basic_ostream<CharT, Traits>& os, const void* vp, format_xflag xflags) {
	const auto& value = *static_cast<std::add_pointer_t<std::add_const_t<T>>>(vp);
	if constexpr (std::is_arithmetic_v<T>) {
		if constexpr (std::is_integral_v<T>) {
			if (xflags & format_xflags::character) {
				os << static_cast<CharT>(value);
				return;
			}
		}
		if (xflags & (format_xflags::grouping | format_xflags::number)) {
			auto loc = os.imbue(preferred_locale());
			os << value;
			os.imbue(loc);
			return;
		}
	}
	// TODO
	os << value;
}

template <class CharT, class Traits = std::char_traits<CharT>>
struct format_argument {
	const void* const vp;
	void(* const out)(std::basic_ostream<CharT, Traits>&, const void*, format_xflag);
	format_xflag xflags;

	template <class T>
	explicit constexpr format_argument(const T& val) noexcept: vp(std::addressof(val)), out(format_out<CharT, Traits, T>), xflags(0) {}
};

template <class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const format_argument<CharT, Traits>& arg) {
	arg.out(os, arg.vp, arg.xflags);
	return os;
}

template <class Traits, class CharT>
constexpr std::pair<bool, const CharT*> format_find_colon_or_close_brace(const CharT* first, const CharT* last) {
	while (first != last) {
		const auto ch = *first;
		if (Traits::eq(ch, ascii_constant_v<CharT, ':'>)) {
			return {true, first};
		} else if (Traits::eq(ch, ascii_constant_v<CharT, '}'>)) {
			break;
		}
		++first;
	}
	return {false, first};
}

ESL_IMPL_STRING_LITERAL_CONSTANT(format_spec_regex_string, R"(^(?:(.?)([<>=^]))?([+\- ]?)(#?)(0?)([0-9]*)([_,]?)(\.[0-9]*)?([bcdeEfFgGnosxX%]?))")

// NOTE: global regex instance not working on MSVC, unknown bug, use static
// template <class CharT>
// inline const std::basic_regex<CharT> format_spec_regex(format_spec_regex_string_v<CharT>, std::regex_constants::ECMAScript | std::regex_constants::optimize);
template <class CharT>
inline const std::basic_regex<CharT>& format_spec_regex() noexcept {
	static const std::basic_regex<CharT> r(format_spec_regex_string_v<CharT>, std::regex_constants::ECMAScript | std::regex_constants::optimize);
	return r;
}

template <class Alloc, class CharT, class Traits>
const CharT* format_spec(const std::basic_string_view<CharT, Traits>& v, std::basic_ostream<CharT, Traits>& os, format_xflag& xflags) {
	using string_view_type = std::basic_string_view<CharT, Traits>;
	const auto first = v.data();
	const auto last = v.data() + v.size();
	std::match_results<const CharT*, typename std::allocator_traits<Alloc>::template rebind_alloc<std::sub_match<const CharT*>>> m;
	if (!std::regex_search(first, last, m, format_spec_regex<CharT>())) {
		return first;
	}
	auto& matched = m[0];
	if (matched.second != last) {
		return matched.second;
	}
	auto& fill = m[1];
	if (fill.first != fill.second) {
		os.fill(*fill.first);
	}
	auto& align = m[2];
	if (align.first != align.second) {
		const auto c = *align.first;
		if (Traits::eq(c, ascii_constant_v<CharT, '<'>)) {
			os.setf(std::ios_base::left);
		} else if (Traits::eq(c, ascii_constant_v<CharT, '>'>)) {
			os.setf(std::ios_base::right);
		} else if (Traits::eq(c, ascii_constant_v<CharT, '='>)) {
			os.setf(std::ios_base::internal);
		} else if (Traits::eq(c, ascii_constant_v<CharT, '^'>)) {
			xflags |= format_xflags::center;
		} else {
			return align.first;
		}
	}
	auto& sign = m[3];
	if (sign.first != sign.second) {
		// default '-'
		const auto c = *sign.first;
		if (Traits::eq(c, ascii_constant_v<CharT, '+'>)) {
			os.setf(std::ios_base::showpos);
		} else if (Traits::eq(c, ascii_constant_v<CharT, ' '>)) {
			xflags |= format_xflags::space_sign;
		} else {
			return sign.first;
		}
	}
	if (m.length(4)) { // #
		os.setf(std::ios_base::showbase);
	}
	if (m.length(5)) { // 0, override m[1]
		os.fill(ascii_constant_v<CharT, '0'>);
	}
	auto& width = m[6];
	if (width.first != width.second) {
		std::size_t w = 0;
		from_string(string_view_type(width.first, width.second - width.first), w);
		os.width(w);
	}
	if (m.length(7)) { // grouping_option
		xflags |= format_xflags::grouping;
	}
	auto& precision = m[8];
	if (precision.first != precision.second) {
		std::size_t p = 0;
		from_string(string_view_type(precision.first + 1, precision.second - precision.first - 1), p);
		os.precision(p);
	}
	auto& type = m[9];
	if (type.first != type.second) {
		// default string 's', integer 'd', float 'g'
		const auto c = *type.first;
		os.unsetf(std::ios_base::boolalpha);
		if (Traits::eq(c, ascii_constant_v<CharT, 'b'>)) {
			xflags |= format_xflags::binary;
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'c'>)) {
			xflags |= format_xflags::character;
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'd'>)) {
			os.setf(std::ios_base::dec);
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'o'>)) {
			os.setf(std::ios_base::oct);
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'x'>)) {
			os.setf(std::ios_base::hex);
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'X'>)) {
			os.setf(std::ios_base::hex | std::ios_base::uppercase);
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'f'>)) {
			os.setf(std::ios_base::fixed | std::ios_base::showpoint);
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'F'>)) {
			os.setf(std::ios_base::fixed | std::ios_base::showpoint | std::ios_base::uppercase);
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'e'>)) {
			os.setf(std::ios_base::scientific);
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'E'>)) {
			os.setf(std::ios_base::scientific | std::ios_base::uppercase);
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'G'>)) {
			os.setf(std::ios_base::uppercase);
		} else if (Traits::eq(c, ascii_constant_v<CharT, 'n'>)) {
			xflags |= format_xflags::number;
		} else if (Traits::eq(c, ascii_constant_v<CharT, '%'>)) {
			xflags |= format_xflags::percent;
		} else {
			return type.first;
		}
	}
	return last;
}

} // namespace details

template <class T, class... Args, class STraits = string_traits<T>>
typename STraits::string_type format(const T& fmt, Args&&... args) {
	using char_type = typename STraits::value_type;
	using traits_type = typename STraits::traits_type;
	using allocator_type = typename STraits::allocator_type;
	using string_view_type = typename STraits::string_view_type;

	string_view_type fmt_v(fmt);
	details::format_argument<char_type, traits_type> fargs[sizeof...(Args)] = {details::format_argument<char_type, traits_type>(args)...};

	std::basic_ostringstream<char_type, traits_type, allocator_type> os;
	std::size_t next_index = 0;
	const auto first = fmt_v.data();
	const auto last = first + fmt_v.size();
	auto it = first;

	while (it != last) {
		// <prefix>{field}<suffix> -> field}<suffix>
		auto lit = it;
		while (lit != last) {
			const auto ch = *lit;
			if (traits_type::eq(ch, ascii_constant_v<char_type, '{'>)) {
				if (lit != last - 1 && traits_type::eq(lit[1], ascii_constant_v<char_type, '{'>)) {
					os << string_view_type(it , lit + 1 - it);
					lit += 2;
					it = lit;
					continue;
				}
				break;
			} else if (traits_type::eq(ch, ascii_constant_v<char_type, '}'>)) {
				if (lit != last - 1 && traits_type::eq(lit[1], ascii_constant_v<char_type, '}'>)) {
					os << string_view_type(it , lit + 1 - it);
					lit += 2;
					it = lit;
					continue;
				}
				throw bad_format("esl::format: unexpected '}'", lit - first);
			}
			++lit;
		}
		if (lit == last) {
			os << string_view_type(it, last - it);
			break;
		}
		if (lit != it) {
			os << string_view_type(it, lit - it);
		}
		it = lit + 1;
		// field}<suffix> -> field + <suffix>
		auto [cfound, rit] = details::format_find_colon_or_close_brace<traits_type>(it, last);
		if (rit == last) {
			throw bad_format("esl::format: missing '}'", lit - first);
		}
		auto index = next_index;
		if (rit == it) {
			++next_index;
		} else {
			const string_view_type v(it, rit - it);
			auto fr = from_string(v, index);
			if (fr.second != v.size()) { // index
				throw bad_format("esl::format: bad format index", it - first + fr.second);
			}
			next_index = index + 1;
		}
		if (index >= sizeof...(Args)) {
			throw bad_format("esl::format: index out of range", lit + 1 - first);
		}
		auto& farg = fargs[index];
		// reset ostream
		os.unsetf(os.flags());
		os.setf(std::ios_base::boolalpha);
		os.fill(ascii_constant_v<char_type, ' '>);
		os.precision(6);
		if (cfound) {
			it = rit + 1;
			auto pos = string_view_type(it, last - it).find(ascii_constant_v<char_type, '}'>);
			if (pos == string_view_type::npos) {
				throw bad_format("esl::format: missing '}'", lit - first);
			}
			rit = it + pos;
			if (pos != 0) {
				auto ptr = details::format_spec<allocator_type>(string_view_type(it, rit - it), os, farg.xflags);
				if (ptr != rit) {
					throw bad_format("esl::format: bad format spec", ptr - first);
				}
			}
		}
		os << farg;
		it = rit + 1;
	}
	return os.str();
}

} //namespace esl

#endif //ESL_STRING_HPP

