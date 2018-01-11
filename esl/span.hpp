
#include <limits>
#include <type_traits>
#include <stdexcept>
#include <algorithm>
#include <array>
#include <vector>
#include <tuple>

#define _ESL_THROW_OUT_OF_RANGE(func) throw std::out_of_range(func "out of range")

namespace esl {

namespace details {

using span_npos = std::integral_constant<std::size_t, std::numeric_limits<std::size_t>::max()>;

// span_storage
template <class pointer, std::size_t N>
class span_storage {
public:
	constexpr span_storage(pointer data=nullptr) noexcept : data_(data) {}

	constexpr pointer data() const noexcept { return data_; } 
	constexpr std::size_t size() const noexcept { return N; }

private:
	pointer data_;
};
template <class pointer>
class span_storage<pointer, span_npos::value> {
public:
	constexpr span_storage(pointer data, std::size_t size) noexcept : data_(data), size_(size) {}
	constexpr span_storage() noexcept : span_storage(nullptr, 0) {}

	constexpr pointer data() const noexcept { return data_; } 
	constexpr std::size_t size() const noexcept { return size_; }

private:
	pointer data_;
	std::size_t size_;
};

// span_sub_size
template <std::size_t N, std::size_t Pos, std::size_t Cnt, bool = Pos <= N>
class span_subspan_size;
template <std::size_t Pos, std::size_t Cnt>
class span_subspan_size<span_npos::value, Pos, Cnt, true> {
public:
	static constexpr std::size_t value = span_npos::value; 
};
template <std::size_t N, std::size_t Pos, std::size_t Cnt>
class span_subspan_size<N, Pos, Cnt, true> {
public:
	static constexpr std::size_t value = std::min(N - Pos, Cnt);
};

} // namespace details

using details::span_npos; 

template <class T, std::size_t N=span_npos::value>
class span {
public:
	using element_type = T;
	using value_type = std::remove_cv_t<T>; 
	using size_type = std::size_t; 
	using difference_type = std::ptrdiff_t; 
	using reference = element_type&; 
	using pointer = element_type*;
	using iterator = element_type*; 
	using const_iterator = const element_type*; 
    using reverse_iterator = iterator;
    using const_reverse_iterator = const_iterator;
	
public:
	static constexpr size_type npos = span_npos::value; 

public:
	constexpr span() noexcept = default;

	// Fixed
	constexpr span(pointer data) noexcept: storage_(data) {}

	// Dynamic
	constexpr span(pointer data, size_type size) noexcept: storage_(data, size) {}
	constexpr span(pointer first, pointer last) noexcept: span(first, last - first) {}
	template <size_type M>
	constexpr span(element_type(&arr)[M]) noexcept: span(arr, M) {}
	template <size_type M>
	constexpr span(const std::array<value_type, M>& arr) noexcept: span(arr, M) {}
	constexpr span(const std::vector<value_type>& vec) noexcept: span(vec.data(), vec.size()) {} 
	template <size_type M> 
	constexpr span(const span<T, M>& span) noexcept: storage_(span.data(), span.size()) {}

	// Copy & Move
	constexpr span(const span&) noexcept = default; 
	constexpr span(span&&) noexcept = default; 

	~span() noexcept = default;

	// Copy & Move
	constexpr span& operator=(const span&) noexcept = default; 
	constexpr span& operator=(span&&) noexcept = default; 

	// Element access

	// Exceptions: std::out_of_range if N == npos && pos >= size()
	template <size_type Pos>
	constexpr reference at() const noexcept(N != npos);
	// Exceptions: std::out_of_range if pos >= size()
	constexpr reference at(size_type pos) const {
		if (pos >= size()) { 
			_ESL_THROW_OUT_OF_RANGE("esl::span::at");
		}
		return data()[pos];
	}

	constexpr reference operator[](size_type pos) const noexcept { return data()[pos]; }
	constexpr reference front() const noexcept { return *begin(); }
	constexpr reference back() const noexcept { return *rbegin(); }
	constexpr pointer data() const noexcept { return storage_.data(); } 

	// Exceptions: std::out_of_range if N == npos && pos > size()
	template <size_type Pos, size_type Cnt=npos, size_type Size=details::span_subspan_size<N, Pos, Cnt>::value>
	constexpr span<T, Size> subspan() const noexcept(N != npos); 
	// Exceptions: std::out_of_range if pos > size()
	constexpr span<T> subspan(size_type pos, size_type cnt = span_npos::value) const { 
		if (pos > size()) {
			_ESL_THROW_OUT_OF_RANGE("esl::span::subspan");
		}
		size_type rcnt = size() - pos;
		return span<T>{data() + pos, std::min(rcnt, cnt)}; 
	}

	// Capacity
	constexpr bool empty() const noexcept { return size() == 0; }
	constexpr size_type size() const noexcept { return storage_.size(); }
	
	// Iterators
	iterator begin() const noexcept { return data(); }
	iterator end() const noexcept { return data() + size(); }

	const_iterator cbegin() const noexcept { return begin(); } 
	const_iterator cend() const noexcept { return end(); } 

	reverse_iterator rbegin() const noexcept { return data() - 1 + size(); }
	reverse_iterator rend() const noexcept { return data() - 1; }

	const_reverse_iterator crbegin() const noexcept { return rbegin(); }
	const_reverse_iterator crend() const noexcept { return rend(); }

private:
	details::span_storage<pointer, N> storage_;
};

namespace details {

template <class T, std::size_t N>
struct span_helper {
	using Span = span<T, N>;

	template <std::size_t Pos, class = std::enable_if_t<Pos < N>>
	static constexpr typename Span::reference at(const Span& sp) noexcept { return sp[Pos]; }

	template <std::size_t Pos, std::size_t Cnt, std::size_t Size>
	static constexpr span<T, Size> subspan(const Span& sp) noexcept { return span<T, Size>{&sp[Pos]}; }
};
template <class T>
struct span_helper<T, span_npos::value> {
	using Span = span<T>;

	template <std::size_t Pos>
	static constexpr typename Span::reference at(const Span& sp) { return sp.at(Pos); }

	template <std::size_t Pos, std::size_t Cnt, std::size_t Size>
	static constexpr span<T, Size> subspan(const Span& sp) { return sp.subspan(Pos, Cnt); }
};

} // namespace details

// span::at
template <class T, std::size_t N>
template <std::size_t Pos>
inline constexpr auto span<T, N>::at() const noexcept(N != npos) -> reference { return details::span_helper<T, N>::template at<Pos>(*this); }

// span::subspan
template <class T, std::size_t N>
template <std::size_t Pos, std::size_t Cnt, std::size_t Size>
inline constexpr span<T, Size> span<T, N>::subspan() const noexcept(N != npos) { return details::span_helper<T, N>::template subspan<Pos, Cnt, Size>(*this); } 

// Comparison
template <class T, std::size_t N, std::size_t M>
inline constexpr bool operator==(const span<T, N>& lhs, const span<T, M>& rhs) {
	return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <class T, std::size_t N, std::size_t M>
inline constexpr bool operator!=(const span<T, N>& lhs, const span<T, M>& rhs) {
	return !(lhs == rhs);
}

// Traits
template <class T>
class is_span: public std::false_type {};
template <class T, std::size_t N>
class is_span<span<T, N>>: public std::true_type {};

template <class T>
class is_fixed_span: std::false_type {};
template <class T, std::size_t N>
class is_fixed_span<span<T, N>>: public std::integral_constant<bool, N == span_npos::value> {};

// make_span
template <class T, std::size_t N>
inline constexpr span<T, N> make_span(T(&arr)[N]) noexcept {
	return span<T, N>{arr};
}
template <class T, std::size_t N>
inline constexpr span<T, N> make_span(std::array<T, N>& arr) noexcept {
	return span<T, N>{arr.data()};
}
template <class T, std::size_t N>
inline constexpr span<const T, N> make_span(const std::array<T, N>& arr) noexcept {
	return span<const T, N>{arr.data()};
}
template <class T>
inline constexpr span<T> make_span(T* data, std::size_t cnt) noexcept {
	return span<T>{data, cnt};
}
template <class T>
inline constexpr span<T> make_span(T* first, T* last) noexcept {
	return span<T>{first, last};
}
template <class T>
inline constexpr span<T> make_span(std::vector<T>& vec) noexcept {
	return span<T>{vec.data(), vec.size()};
}
template <class T>
inline constexpr span<const T> make_span(const std::vector<T>& vec) noexcept {
	return span<const T>{vec.data(), vec.size()};
}

} // namespace esl

// std::tuple_size<fixed-span>, std::get<I>(fixed-span) 
namespace std {

// std::tuple_size
template <class T, size_t N>
class tuple_size<esl::span<T, N>>: public integral_constant<size_t, N> {};
template <class T>
class tuple_size<esl::span<T, esl::span_npos::value>> {};

// std::get
template <size_t I, class T, size_t N, class = std::enable_if<N != esl::span_npos::value>>
inline constexpr typename esl::span<T, N>::reference get(const esl::span<T, N>& span) noexcept {
	return span.template at<I>(); 
}

} // namespace std

