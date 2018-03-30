#ifndef ESL_VARIANT_ANY_HPP
#define ESL_VARIANT_ANY_HPP

#include "type_traits.hpp"
#include "functional.hpp"

#include <utility>
#include <any>
#include <array>

// variant_any
//
// * variant_any is a std::any
// * variant_any satisfied with std::variant
//  except:
//    * default constructed is no state and is valueless_by_exception()
//    * costructors are not constexpr except default
//    * operator= is strong exception guarantee

namespace esl {

template <class... Ts>
class variant_any;

} // namespace esl


// FIXME: must include this file before <variant>, otherwise std::visit will not work.
// Forward declaration for std::visit
namespace std {
template <size_t I, class T>
struct variant_alternative;
template <size_t I, class... Ts>
constexpr typename variant_alternative<I, ::esl::variant_any<Ts...>>::type& get(::esl::variant_any<Ts...>& v);
template <size_t I, class... Ts>
constexpr typename variant_alternative<I, ::esl::variant_any<Ts...>>::type&& get(::esl::variant_any<Ts...>&& v);
template <size_t I, class... Ts>
constexpr typename variant_alternative<I, ::esl::variant_any<Ts...>>::type const& get(const ::esl::variant_any<Ts...>& v);
template <size_t I, class... Ts>
constexpr typename variant_alternative<I, ::esl::variant_any<Ts...>>::type const&& get(const ::esl::variant_any<Ts...>&& v);
template <class T, class... Ts>
constexpr T& get(::esl::variant_any<Ts...>& v);
template <class T, class... Ts>
constexpr T&& get(::esl::variant_any<Ts...>&& v);
template <class T, class... Ts>
constexpr const T& get(const ::esl::variant_any<Ts...>& v);
template <class T, class... Ts>
constexpr const T&& get(const ::esl::variant_any<Ts...>&& v);
} // namespace std

#include <variant>

namespace std {

// std::variant_size
template <class... Ts>
struct variant_size<::esl::variant_any<Ts...>>: integral_constant<size_t, sizeof...(Ts)> {};

// std::variant_alternative
template <size_t I, class... Ts>
struct variant_alternative<I, ::esl::variant_any<Ts...>>: ::esl::nth_type<I, Ts...> {};

} // namespace std

namespace esl {

inline constexpr std::size_t variant_any_npos = std::variant_npos;

template <class... Ts>
class variant_any: public std::any {
private:
	template <class T, bool = is_exactly_once_v<T, Ts...>>
	struct exactly_once_index {};
	template <class T>
	struct exactly_once_index<T, true>: index_of<T, Ts...> {};

	template <class T, class RT>
	using select_type_enable_if = std::enable_if<is_exactly_once_v<RT, Ts...> && std::is_constructible_v<RT, T>, RT>;
	template <class T, bool = is_overload_resolvable_v<T, Ts...>>
	struct select_type {};
	template <class T>
	struct select_type<T, true>: select_type_enable_if<T, overload_resolution_t<T, Ts...>> {};
	// workaround for clang++
	template <class T, bool = std::is_same_v<std::decay_t<T>, variant_any>>
	struct accepted_type {};
	template <class T>
	struct accepted_type<T, false>: select_type<T> {};

private:
	std::size_t index_;

public:
	constexpr variant_any() noexcept: index_(variant_any_npos) {}

	variant_any(const variant_any& other): any(static_cast<const any&>(other)), index_(other.index_) {}

	variant_any(variant_any&& other) noexcept: any(static_cast<any&&>(std::move(other))), index_(other.index_) {
		other.index_ = variant_any_npos;
	}

	template <class T, class AT = typename accepted_type<T&&>::type>
	variant_any(T&& value): variant_any(std::in_place_type<AT>, std::forward<T>(value)) {}

	template <class T, class... Args, class I = typename exactly_once_index<T>::type>
	explicit variant_any(std::in_place_type_t<T>, Args&&... args):
		any(std::in_place_type<T>, std::forward<Args>(args)...), index_(I::value) {}

	template <class T, class U, class... Args, class I = typename exactly_once_index<T>::type>
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
	template <class T, class AT = typename accepted_type<T&&>::type>
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
	template <class T, class... Args, class I = typename exactly_once_index<T>::type>
	T& emplace(Args&&... args) {
		return this->emplace<T, I::value>(std::forward<Args>(args)...);
	}

	template <class T, class U, class... Args, class I = typename exactly_once_index<T>::type>
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
	return va.index() == ::esl::index_of_v<T, Ts...>;
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
template <class T, class... Ts>
inline constexpr T& get(::esl::variant_any<Ts...>& v) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return any_cast<T&>(v);
}
template <class T, class... Ts>
inline constexpr T&& get(::esl::variant_any<Ts...>&& v) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return any_cast<T&&>(move(v));
}
template <class T, class... Ts>
inline constexpr const T& get(const ::esl::variant_any<Ts...>& v) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return any_cast<const T&>(v);
}
template <class T, class... Ts>
inline constexpr const T&& get(const ::esl::variant_any<Ts...>&& v) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return any_cast<const T&&>(move(v));
}

// std::get_if
template <size_t I, class... Ts>
inline constexpr add_pointer_t<variant_alternative_t<I, ::esl::variant_any<Ts...>>> get_if(::esl::variant_any<Ts...>* vap) {
	return (vap && vap->index() == I) ? &std::get<I>(vap) : nullptr;
}
template <size_t I, class... Ts>
inline constexpr add_pointer_t<const variant_alternative_t<I, ::esl::variant_any<Ts...>>> get_if(const ::esl::variant_any<Ts...>* vap) {
	return (vap && vap->index() == I) ? &std::get<I>(vap) : nullptr;
}
template <class T, class... Ts>
inline constexpr add_pointer_t<T> get_if(::esl::variant_any<Ts...>* vap) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	using index_type = ::esl::index_of<T, Ts...>;
	return (vap && vap->index() == index_type::value) ? &std::get<index_type::value>(vap) : nullptr;
}
template <class T, class... Ts>
inline constexpr add_pointer_t<const T> get_if(const ::esl::variant_any<Ts...>* vap) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	using index_type = ::esl::index_of<T, Ts...>;
	return (vap && vap->index() == index_type::value) ? &std::get<index_type::value>(vap) : nullptr;
}

// std::swap
template <class... Ts>
inline void swap(::esl::variant_any<Ts...>& lhs, ::esl::variant_any<Ts...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	lhs.swap(rhs);
}

// std::hash
template <class... Ts>
struct hash<::esl::variant_any<Ts...>> {
private:
	using hasher_tuple_type = tuple<hash<remove_const_t<Ts>>...>;
	using hash_func_type = size_t (*)(const hasher_tuple_type&, const ::esl::variant_any<Ts...>&);
	hasher_tuple_type hashers_;
	template <size_t I>
	static size_t hash_(const hasher_tuple_type& hashers, const ::esl::variant_any<Ts...>& va) {
		return ::esl::hash_combine(std::get<I>(hashers)(std::get<I>(va)), I);
	}
	template <size_t... Is>
	static constexpr array<hash_func_type, sizeof...(Ts)> gen_hashers(index_sequence<Is...>) {
		return {{hash_<Is>...}};
	}
	static constexpr auto hashers = gen_hashers(index_sequence_for<Ts...>{});
public:
	size_t operator()(const ::esl::variant_any<Ts...>& va) const { return hashers[va.index()](hashers_, va); }
};

} // namespace std

namespace esl {

using std::variant_size;
using std::variant_size_v;
using std::variant_alternative_t;
using std::holds_alternative;
using std::get;
using std::get_if;

namespace details {

template <template <class> class comp, class... Ts>
struct variant_any_comp {
private:
	using comp_func_type = bool (*)(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs);
	template <std::size_t I, class T>
	static bool comp_(const variant_any<Ts...>& lhs, const variant_any<Ts...>& rhs) { return comp<T>{}(std::get<I>(lhs), std::get<I>(rhs)); }
	template <std::size_t... Is>
	static constexpr std::array<comp_func_type, sizeof...(Ts)> gen_comps(std::index_sequence<Is...>) {
		return {{comp_<Is, Ts>...}};
	}
	static constexpr auto comps = gen_comps(std::index_sequence_for<Ts...>{});
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
	return lhs.index() != rhs.index() || (!lhs.valueless_by_exception() && details::variant_any_comp<std::not_equal_to, Ts...>{}(lhs, rhs));
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

