#ifndef ESL_VARIANT_ANY_HPP
#define ESL_VARIANT_ANY_HPP

#include "type_traits.hpp"

#include <utility>
#include <any>
#include <variant>

namespace esl {

inline constexpr std::size_t variant_any_npos = std::variant_npos;

template <class... Ts>
class variant_any;

template <class... Ts>
class variant_any: public std::any {
private:
	template <std::size_t I>
	using alternative_type = types_nth_t<I, Ts...>;

	template <class T>
	using alternative_index = types_index_t<std::decay_t<T>, Ts...>;

private:
	std::size_t index_;

public:
	constexpr variant_any() noexcept: index_(variant_any_npos) {}

	variant_any(const variant_any& other): any(static_cast<const any&>(other)), index_(other.index_) {}

	variant_any(variant_any&& other) noexcept: any(static_cast<any&&>(std::move(other))), index_(other.index_) {
		other.index_ = variant_any_npos;
	}

	template <class T, class I = alternative_index<T>>
	variant_any(T&& value): any(std::forward<T>(value)), index_(I::value) {}

	template <class T, class... Args, class I = alternative_index<T>>
	explicit variant_any(std::in_place_type_t<T>, Args&&... args): any(std::in_place_type<T>, std::forward<Args>(args)...), index_(I::value) {}

	template <class T, class U, class... Args, class I = alternative_index<T>>
	explicit variant_any(std::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args): any(std::in_place_type<T>, il, std::forward<Args>(args)...), index_(I::value) {}

	template <std::size_t I, class... Args, class T = alternative_type<I>>
	explicit variant_any(std::in_place_index_t<I>, Args&&... args): any(std::in_place_type<T>, std::forward<Args>(args)...), index_(I) {}

	template <std::size_t I, class U, class... Args, class T = alternative_type<I>>
	explicit variant_any(std::in_place_index_t<I>, std::initializer_list<U> il, Args&&... args): any(std::in_place_type<T>, il, std::forward<Args>(args)...), index_(I) {}

	variant_any& operator=(const variant_any& other) noexcept {
		this->any::operator=(static_cast<const any&>(other));
		index_ = other.index_;
		return *this;
	}

	variant_any& operator=(variant_any&& other) noexcept {
		this->any::operator=(static_cast<any&&>(std::move(other)));
		index_ = other.index_;
		other.index_ = variant_any_npos;
		return *this;
	}

	template <class T, class I = alternative_index<T>>
	variant_any& operator=(T&& value) {
		this->any::operator=(std::forward<T>(value));
		index_ = I::value;
		return *this;
	}

	using any::has_value;

	using any::type;

	constexpr std::size_t index() const noexcept { return index_; }

	void reset() {
		this->any::reset();
		index_ = variant_any_npos;
	}

	template <class T, class... Args, class I = alternative_index<T>>
	std::decay_t<T>& emplace(Args&&... args) {
		index_ = I::value;
		return this->any::emplace<T>(std::forward<Args>(args)...);
	}

	template <class T, class U, class... Args, class I = alternative_index<T>>
	std::decay_t<T>& emplace(std::initializer_list<U> il, Args&&... args) {
		index_ = I::value;
		return this->any::emplace<T>(il, std::forward<Args>(args)...);
	}

	template <std::size_t I, class... Args, class T = alternative_type<I>>
	T& emplace(Args&&... args) {
		index_ = I;
		return this->any::emplace<T>(std::forward<Args>(args)...);
	}

	template <std::size_t I, class U, class... Args, class T = alternative_type<I>>
	T& emplace(std::initializer_list<U> il, Args&&... args) {
		index_ = I;
		return this->any::emplace<T>(il, std::forward<Args>(args)...);
	}

	void swap(variant_any& other) noexcept {
		this->any::swap(static_cast<any&>(other));
		std::swap(index_, other.index_);
	}
};

} // namespace esl

namespace std {

// std::variant_size
template <class... Ts>
struct variant_size<::esl::variant_any<Ts...>>: integral_constant<std::size_t, sizeof...(Ts)> {};

// std::variant_alternative
template <std::size_t I, class... Ts>
struct variant_alternative<I, ::esl::variant_any<Ts...>>: ::esl::types_nth<I, Ts...> {};

// std::holds_alternative
template <class T, class... Ts>
inline constexpr bool holds_alternative(const ::esl::variant_any<Ts...>& va) noexcept {
	return va.index() == ::esl::types_index_v<::std::decay_t<T>, Ts...>;
}

// std::get
template <std::size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::variant_any<Ts...>>& get(::esl::variant_any<Ts...>& v) {
	return std::any_cast<variant_alternative_t<I, ::esl::variant_any<Ts...>>&>(v);
}
template <std::size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::variant_any<Ts...>>&& get(::esl::variant_any<Ts...>&& v) {
	return std::any_cast<variant_alternative_t<I, ::esl::variant_any<Ts...>>&&>(std::move(v));
}
template <std::size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::variant_any<Ts...>> const& get(const ::esl::variant_any<Ts...>& v) {
	return std::any_cast<variant_alternative_t<I, ::esl::variant_any<Ts...>> const&>(v);
}
template <std::size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::variant_any<Ts...>> const&& get(const ::esl::variant_any<Ts...>&& v) {
	return std::any_cast<variant_alternative_t<I, ::esl::variant_any<Ts...>> const&&>(std::move(v));
}
template <class T, class... Ts, std::size_t I = ::esl::types_index_v<T, Ts...>>
inline constexpr T& get(::esl::variant_any<Ts...>& v) { return std::any_cast<T&>(v); }
template <class T, class... Ts, std::size_t I = ::esl::types_index_v<T, Ts...>>
inline constexpr T&& get(::esl::variant_any<Ts...>&& v) { return std::any_cast<T&&>(std::move(v)); }
template <class T, class... Ts, std::size_t I = ::esl::types_index_v<T, Ts...>>
inline constexpr const T& get(const ::esl::variant_any<Ts...>& v) { return std::any_cast<const T&>(v); }
template <class T, class... Ts, std::size_t I = ::esl::types_index_v<T, Ts...>>
inline constexpr const T&& get(const ::esl::variant_any<Ts...>&& v) { return std::any_cast<const T&&>(std::move(v)); }

// std::swap
template <class... Ts>
inline void swap(::esl::variant_any<Ts...>& lhs, ::esl::variant_any<Ts...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	lhs.swap(rhs);
}

} // namespace std

namespace esl {

using std::variant_size;
using std::variant_size_v;
using std::variant_alternative_t;
using std::holds_alternative;
using std::get;

namespace details {

template <template <class> class comp, class... Ts>
struct variant_any_comp {
	using comp_func_type = bool (*)(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs);

	template <class T>
	static constexpr bool comp_(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) { return comp<T>{}(std::get<T>(lhs), std::get<T>(rhs)); }

	static constexpr comp_func_type comps[sizeof...(Ts)] = {comp_<Ts>...};

public:
	constexpr bool operator()(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) const { return comps[lhs.index()](lhs, rhs); }
};

} // namespace details

// operators
template <class... Ts>
inline constexpr bool operator==(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	return lhs.index() == rhs.index() && (lhs.index() == variant_any_npos || details::variant_any_comp<std::equal_to, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator!=(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	return lhs.index() != rhs.index() || (lhs.index() != variant_any_npos && details::variant_any_comp<std::not_equal_to, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator<(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	if (rhs.index() == variant_any_npos) {
		return false;
	}
	if (lhs.index() == variant_any_npos) {
		return true;
	}
	return lhs.index() < rhs.index() || (lhs.index() == rhs.index() && details::variant_any_comp<std::less, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator>(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	if (lhs.index() == variant_any_npos) {
		return false;
	}
	if (rhs.index() == variant_any_npos) {
		return true;
	}
	return lhs.index() > rhs.index() || (lhs.index() == rhs.index() && details::variant_any_comp<std::greater, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator<=(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	if (lhs.index() == variant_any_npos) {
		return true;
	}
	if (rhs.index() == variant_any_npos) {
		return false;
	}
	return lhs.index() < rhs.index() || (lhs.index() == rhs.index() && details::variant_any_comp<std::less_equal, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator>=(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	if (rhs.index() == variant_any_npos) {
		return true;
	}
	if (lhs.index() == variant_any_npos) {
		return false;
	}
	return lhs.index() > rhs.index() || (lhs.index() == rhs.index() && details::variant_any_comp<std::greater_equal, Ts...>{}(lhs, rhs));
}

} // namespace esl

#endif // ESL_VARIANT_ANY_HPP

