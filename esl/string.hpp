
#ifndef ESL_STRING_HPP
#define ESL_STRING_HPP

#include "macros.hpp"
#include "type_traits.hpp"
#include "memory.hpp"

#include <string>
#include <string_view>

namespace esl {

template <class CharT, class Deleter = std::default_delete<CharT[]>>
struct basic_unique_string: public std::unique_ptr<CharT[], Deleter> {
private:
	using unique_ptr = std::unique_ptr<CharT[], Deleter>;

public:
	using unique_ptr::unique_ptr; 

	template <class Traits>
	explicit operator std::basic_string_view<CharT, Traits>() const noexcept {
		return this->get();
	}

	template <class Traits, class Allocator>
	operator std::basic_string<CharT, Traits, Allocator>() const {
		return this->get();
	}
};

using unique_string = basic_unique_string<char>;
using wunique_string = basic_unique_string<wchar_t>;
using u16unique_string = basic_unique_string<char16_t>;
using u32unique_string = basic_unique_string<char32_t>;

using c_unique_string = basic_unique_string<char, c_default_delete>;
using c_wunique_string = basic_unique_string<wchar_t, c_default_delete>;
using c_u16unique_string = basic_unique_string<char16_t, c_default_delete>;
using c_u32unique_string = basic_unique_string<char32_t, c_default_delete>;

// SV: std::basic_string<> or std::basic_string_view<>

// Traits
template <class SV, class Traits = typename SV::traits_type>
struct string_string_view {
	using type = std::basic_string_view<typename SV::value_type, Traits>;
};
template <class SV, class Traits = typename SV::traits_type>
using string_string_view_t = typename string_string_view<SV, Traits>::type; 

template <class SV, class Traits = typename SV::traits_type, 
		class Allocator = member_type_allocator_type_t<SV, std::allocator<typename SV::value_type>>>
struct string_view_string {
	using type = std::basic_string<typename SV::value_type, Traits, Allocator>;
};
template <class SV, class Traits = typename SV::traits_type, 
		class Allocator = member_type_allocator_type_t<SV, std::allocator<typename SV::value_type>>>
using string_view_string_t = typename string_view_string<SV, Traits, Allocator>::type;

template <class CharT, class Traits, class Allocator>
std::basic_string_view<CharT, Traits> string_substr(const std::basic_string<CharT, Traits, Allocator>& s, 
		typename std::basic_string<CharT, Traits, Allocator>::size_type pos = 0, 
		typename std::basic_string<CharT, Traits, Allocator>::size_type count = std::basic_string<CharT, Traits, Allocator>::npos) {
	return static_cast<std::basic_string_view<CharT, Traits>>(s).substr(pos, count);
}
template <class CharT, class Traits>
std::basic_string_view<CharT, Traits> string_substr(const std::basic_string_view<CharT, Traits>& v, 
		typename std::basic_string_view<CharT, Traits>::size_type pos = 0, 
		typename std::basic_string_view<CharT, Traits>::size_type count = std::basic_string_view<CharT, Traits>::npos) {
	return v.substr(pos, count);
}

template <class T, class SV>
T& string_assign_(T& o, const SV& s, std::true_type) {
	return o = s;
}
// string = string_view
template <class T, class V>
T& string_assign_(T& o, const V& s, std::false_type) {
	return o = string_view_string_t<V>(s);
}
template <class T, class SV>
T& string_assign(T& o, const SV& s) {
	return string_assign_(o, s, typename std::is_assignable<T, SV>::type{});
}

template <class SV, class OutputIt>
OutputIt string_split(const SV& s, typename SV::value_type delim, OutputIt d_first) {
	typename SV::size_type pos = 0;
	typename SV::size_type len = s.length();
	for (typename SV::size_type i = 0; i < len; ++i) {
		if (s[i] == delim) {
			string_assign(*d_first, string_substr(s, pos, i - pos));
			++d_first;
			pos = i + 1;
		}   
	}
	string_assign(*d_first, string_substr(s, pos));
	++d_first;
	return d_first;
}

template <class SV, class OutputIt>
OutputIt string_split(const SV& s, const string_string_view_t<SV>& delim, OutputIt d_first) {
	typename SV::size_type pos = 0; 
	typename SV::size_type end_pos;
	do {
		end_pos = s.find(delim, pos);
		if (end_pos == SV::npos) {
			string_assign(*d_first, string_substr(s, pos));
			++d_first;
			break;
		}
		string_assign(*d_first, string_substr(s, pos, end_pos - pos));
		++d_first;
		pos = end_pos + delim.size();
	} while(true);
	return d_first;
}

} //namespace esl

#endif //ESL_STRING_HPP

