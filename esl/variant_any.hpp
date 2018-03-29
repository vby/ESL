#ifndef ESL_VARIANT_ANY_HPP
#define ESL_VARIANT_ANY_HPP

#include "type_traits.hpp"

#include <utility>
#include <any>
#include <variant>

// variant_any
//
// * variant_any is a std::any
// * variant_any satisfied with std::variant
//  except:
//    * default constructed is no state and is valueless_by_exception()
//    * costructors are not constexpr except default
//    * operator= is strong exception guarantee

namespace esl {

inline constexpr std::size_t variant_any_npos = std::variant_npos;

template <class... Ts>
class variant_any;

} // namespace esl

namespace std {

// std::variant_size
template <class... Ts>
struct variant_size<::esl::variant_any<Ts...>>: integral_constant<size_t, sizeof...(Ts)> {};

// std::variant_alternative
template <size_t I, class... Ts>
struct variant_alternative<I, ::esl::variant_any<Ts...>>: ::esl::types_nth<I, Ts...> {};

} // namespace std

namespace esl {

template <class... Ts>
class variant_any: public std::any {
private:
	template <class T>
	using exactly_once_index_t = typename types_exactly_once_index<T, Ts...>::type;

	template <class T, class RT>
	using select_type_enable_if = std::enable_if<types_exactly_once_v<RT, Ts...> && std::is_constructible_v<RT, T>, RT>;
	template <class T, bool = is_overload_resolvable_v<T, Ts...>>
	struct select_type {};
	template <class T>
	struct select_type<T, true>: select_type_enable_if<T, overload_resolve_t<T, Ts...>> {};
	// workaround for clang++
	template <class T, bool = std::is_same_v<std::decay_t<T>, variant_any>>
	struct accepted_type {};
	template <class T>
	struct accepted_type<T, false>: select_type<T> {};
	template <class T>
	using accepted_type_t = typename accepted_type<T>::type;

private:
	std::size_t index_;

public:
	constexpr variant_any() noexcept: index_(variant_any_npos) {}

	variant_any(const variant_any& other): any(static_cast<const any&>(other)), index_(other.index_) {}

	variant_any(variant_any&& other) noexcept: any(static_cast<any&&>(std::move(other))), index_(other.index_) {
		other.index_ = variant_any_npos;
	}

	template <class T, class AT = accepted_type_t<T&&>>
	variant_any(T&& value): variant_any(std::in_place_type<AT>, std::forward<T>(value)) {}

	template <class T, class... Args, class I = exactly_once_index_t<T>>
	explicit variant_any(std::in_place_type_t<T>, Args&&... args):
		any(std::in_place_type<T>, std::forward<Args>(args)...), index_(I::value) {}

	template <class T, class U, class... Args, class I = exactly_once_index_t<T>>
	explicit variant_any(std::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args):
		any(std::in_place_type<T>, il, std::forward<Args>(args)...), index_(I::value) {}

	template <std::size_t I, class... Args>
	explicit variant_any(std::in_place_index_t<I>, Args&&... args):
		any(std::in_place_type<std::variant_alternative_t<I, variant_any>>, std::forward<Args>(args)...), index_(I) {}

	template <std::size_t I, class U, class... Args>
	explicit variant_any(std::in_place_index_t<I>, std::initializer_list<U> il, Args&&... args):
		any(std::in_place_type<std::variant_alternative_t<I, variant_any>>, il, std::forward<Args>(args)...), index_(I) {}

	// TODO constructor with allocator_tag_t

	// (strong exception guarantee)
	variant_any& operator=(const variant_any& other) {
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

	// (strong exception guarantee)
	template <class T, class AT = accepted_type_t<T&&>>
	variant_any& operator=(T&& value) {
		return this->operator=(variant_any(std::in_place_type<AT>, std::forward<T>(value)));
	}

	// Observers

	constexpr std::size_t index() const noexcept { return index_; }

	constexpr bool valueless_by_exception() const noexcept { return !this->has_value(); }

	using any::has_value;

	// Modifiers

protected:
	template <class T, std::size_t I, class... Args>
	ESL_ATTR_FORCEINLINE T& emplace(Args&&... args) {
		index_ = I;
		try {
			return this->any::emplace<T>(std::forward<Args>(args)...);
		} catch(...) {
			index_ = variant_any_npos;
			throw;
		}
	}

public:
	template <class T, class... Args, class I = exactly_once_index_t<T>>
	T& emplace(Args&&... args) {
		return this->emplace<T, I::value>(std::forward<Args>(args)...);
	}

	template <class T, class U, class... Args, class I = exactly_once_index_t<T>>
	T& emplace(std::initializer_list<U> il, Args&&... args) {
		return this->emplace<T, I::value>(il, std::forward<Args>(args)...);
	}

	template <std::size_t I, class... Args>
	std::variant_alternative_t<I, variant_any>& emplace(Args&&... args) {
		return this->emplace<std::variant_alternative_t<I, variant_any>, I>(std::forward<Args>(args)...);
	}

	template <std::size_t I, class U, class... Args>
	std::variant_alternative_t<I, variant_any>& emplace(std::initializer_list<U> il, Args&&... args) {
		return this->emplace<std::variant_alternative_t<I, variant_any>, I>(il, std::forward<Args>(args)...);
	}

	void swap(variant_any& other) noexcept {
		this->any::swap(static_cast<any&>(other));
		std::swap(index_, other.index_);
	}

	void reset() noexcept {
		this->any::reset();
		index_ = variant_any_npos;
	}
};

} // namespace esl

namespace std {

// std::holds_alternative
template <class T, class... Ts>
inline constexpr bool holds_alternative(const ::esl::variant_any<Ts...>& va) noexcept {
	return va.index() == ::esl::types_index_v<T, Ts...>;
}

// std::get
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::variant_any<Ts...>>& get(::esl::variant_any<Ts...>& v) {
	return any_cast<variant_alternative_t<I, ::esl::variant_any<Ts...>>&>(v);
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::variant_any<Ts...>>&& get(::esl::variant_any<Ts...>&& v) {
	return any_cast<variant_alternative_t<I, ::esl::variant_any<Ts...>>&&>(move(v));
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::variant_any<Ts...>> const& get(const ::esl::variant_any<Ts...>& v) {
	return any_cast<variant_alternative_t<I, ::esl::variant_any<Ts...>> const&>(v);
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::variant_any<Ts...>> const&& get(const ::esl::variant_any<Ts...>&& v) {
	return any_cast<variant_alternative_t<I, ::esl::variant_any<Ts...>> const&&>(move(v));
}
template <class T, class... Ts, class = enable_if_t<::esl::types_exactly_once_v<T, Ts...>>>
inline constexpr T& get(::esl::variant_any<Ts...>& v) { return any_cast<T&>(v); }
template <class T, class... Ts, class = enable_if_t<::esl::types_exactly_once_v<T, Ts...>>>
inline constexpr T&& get(::esl::variant_any<Ts...>&& v) { return any_cast<T&&>(move(v)); }
template <class T, class... Ts, class = enable_if_t<::esl::types_exactly_once_v<T, Ts...>>>
inline constexpr const T& get(const ::esl::variant_any<Ts...>& v) { return any_cast<const T&>(v); }
template <class T, class... Ts, class = enable_if_t<::esl::types_exactly_once_v<T, Ts...>>>
inline constexpr const T&& get(const ::esl::variant_any<Ts...>&& v) { return any_cast<const T&&>(move(v)); }

// std::swap
template <class... Ts>
inline void swap(::esl::variant_any<Ts...>& lhs, ::esl::variant_any<Ts...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	lhs.swap(rhs);
}

// TODO std::visit, std::hash

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
	return lhs.index() == rhs.index() && (lhs.valueless_by_exception() || details::variant_any_comp<std::equal_to, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator!=(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	return lhs.index() != rhs.index() || (lhs.index() != variant_any_npos && details::variant_any_comp<std::not_equal_to, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator<(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	if (rhs.valueless_by_exception()) {
		return false;
	}
	if (lhs.valueless_by_exception()) {
		return true;
	}
	return lhs.index() < rhs.index() || (lhs.index() == rhs.index() && details::variant_any_comp<std::less, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator>(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	if (lhs.valueless_by_exception()) {
		return false;
	}
	if (rhs.valueless_by_exception()) {
		return true;
	}
	return lhs.index() > rhs.index() || (lhs.index() == rhs.index() && details::variant_any_comp<std::greater, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator<=(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	if (lhs.valueless_by_exception()) {
		return true;
	}
	if (rhs.valueless_by_exception()) {
		return false;
	}
	return lhs.index() < rhs.index() || (lhs.index() == rhs.index() && details::variant_any_comp<std::less_equal, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator>=(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) {
	if (rhs.valueless_by_exception()) {
		return true;
	}
	if (lhs.valueless_by_exception()) {
		return false;
	}
	return lhs.index() > rhs.index() || (lhs.index() == rhs.index() && details::variant_any_comp<std::greater_equal, Ts...>{}(lhs, rhs));
}

} // namespace esl

#endif // ESL_VARIANT_ANY_HPP

