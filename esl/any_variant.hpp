#ifndef ESL_ANY_VARIANT_HPP
#define ESL_ANY_VARIANT_HPP

#include "any_storage.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "functional.hpp"

#include <utility>
#include <array>

// any_variant
// * Use for create a variant with incomplete types
// * any_variant satisfied with std::variant
//  except:
//    * Default constructed is no state and is valueless_by_exception()
//    * Costructors are not constexpr except default

namespace esl {

template <class... Ts>
class any_variant;

} // namespace esl


// FIXME: must include this file before <variant>, otherwise std::visit will not work.
// Forward declaration for std::visit
namespace std {
template <size_t I, class T>
struct variant_alternative;
template <size_t I, class... Ts>
constexpr typename variant_alternative<I, ::esl::any_variant<Ts...>>::type& get(::esl::any_variant<Ts...>& v);
template <size_t I, class... Ts>
constexpr typename variant_alternative<I, ::esl::any_variant<Ts...>>::type&& get(::esl::any_variant<Ts...>&& v);
template <size_t I, class... Ts>
constexpr typename variant_alternative<I, ::esl::any_variant<Ts...>>::type const& get(const ::esl::any_variant<Ts...>& v);
template <size_t I, class... Ts>
constexpr typename variant_alternative<I, ::esl::any_variant<Ts...>>::type const&& get(const ::esl::any_variant<Ts...>&& v);
template <class T, class... Ts>
constexpr T& get(::esl::any_variant<Ts...>& v);
template <class T, class... Ts>
constexpr T&& get(::esl::any_variant<Ts...>&& v);
template <class T, class... Ts>
constexpr const T& get(const ::esl::any_variant<Ts...>& v);
template <class T, class... Ts>
constexpr const T&& get(const ::esl::any_variant<Ts...>&& v);
} // namespace std

#include <variant>

namespace std {

// std::variant_size
template <class... Ts>
struct variant_size<::esl::any_variant<Ts...>>: integral_constant<size_t, sizeof...(Ts)> {};

// std::variant_alternative
template <size_t I, class... Ts>
struct variant_alternative<I, ::esl::any_variant<Ts...>>: ::esl::nth_type<I, Ts...> {};

} // namespace std

namespace esl {

template <class... Ts>
class any_variant {
private:
	using Storage = any_storage<>;

	template <class T>
	struct StorageCopy {
		static void value(Storage& to, const Storage& other) { to.template from<T>(other); }
	};
	template <class T>
	struct StorageMove {
		static void value(Storage& to, Storage&& other) { to.template from<T>(std::move(other)); }
	};
	template <class T>
	struct StorageDestruct {
		static void value(Storage& s) { s.template destruct<T>(); }
	};
	template <class T, class U>
	struct StorageSwap {
		static void value(Storage& s, Storage& other) { s.template swap<T, U>(other); }
	};

	static constexpr auto storage_copy_vtable = make_vtable_v<StorageCopy, Ts...>;
	static constexpr auto storage_move_vtable = make_vtable_v<StorageMove, Ts...>;
	static constexpr auto storage_destruct_vtable = make_vtable_v<StorageDestruct, Ts...>;
	static constexpr auto storage_swap_vtable = make_vtable2_v<StorageSwap, std::tuple<Ts...>, std::tuple<Ts...>>;

	Storage storage_;
	std::size_t index_;

private:
	template <class T, bool = is_exactly_once_v<T, Ts...>>
	struct ExactlyOnceIndex {};
	template <class T>
	struct ExactlyOnceIndex<T, true>: index_of<T, Ts...> {};

	template <class T, class RT>
	using SelectTypeEnableIf = std::enable_if<is_exactly_once_v<RT, Ts...> && std::is_constructible_v<RT, T>, RT>;
	template <class T, bool = is_overload_resolvable_v<T, Ts...>>
	struct SelectType {};
	template <class T>
	struct SelectType<T, true>: SelectTypeEnableIf<T, overload_resolution_t<T, Ts...>> {};
	// workaround for clang++
	template <class T, bool = std::is_same_v<std::decay_t<T>, any_variant>>
	struct AcceptedType {};
	template <class T>
	struct AcceptedType<T, false>: SelectType<T> {};

public:
	constexpr any_variant() noexcept: index_(std::variant_npos) {}

	any_variant(const any_variant& other): index_(other.index_) {
		if (index_ != std::variant_npos) {
			storage_copy_vtable[index_](storage_, other.storage_);
		}
	}

	any_variant(any_variant&& other) noexcept: index_(other.index_) {
		if (index_ != std::variant_npos) {
			storage_move_vtable[index_](storage_, std::move(other.storage_));
			other.index_ = std::variant_npos;
		}
	}

	template <class T, class AT = typename AcceptedType<T&&>::type>
	any_variant(T&& value): any_variant(std::in_place_type<AT>, std::forward<T>(value)) {}

	template <class T, class... Args, class I = typename ExactlyOnceIndex<T>::type>
	explicit any_variant(std::in_place_type_t<T>, Args&&... args):
		storage_(std::in_place_type<T>, std::forward<Args>(args)...), index_(I::value) {}

	template <class T, class U, class... Args, class I = typename ExactlyOnceIndex<T>::type>
	explicit any_variant(std::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args):
		storage_(std::in_place_type<T>, il, std::forward<Args>(args)...), index_(I::value) {}

	template <std::size_t I, class... Args>
	explicit any_variant(std::in_place_index_t<I>, Args&&... args):
		storage_(std::in_place_type<std::variant_alternative_t<I, any_variant>>, std::forward<Args>(args)...), index_(I) {}

	template <std::size_t I, class U, class... Args>
	explicit any_variant(std::in_place_index_t<I>, std::initializer_list<U> il, Args&&... args):
		storage_(std::in_place_type<std::variant_alternative_t<I, any_variant>>, il, std::forward<Args>(args)...), index_(I) {}

	// TODO constructor with allocator_tag_t

	any_variant& operator=(const any_variant& other) {
		this->reset();
		if (other.index_ != std::variant_npos) {
			storage_copy_vtable[other.index_](storage_, other.storage_);
			index_ = other.index_;
		}
		return *this;
	}

	any_variant& operator=(any_variant&& other) noexcept {
		this->reset();
		if (other.index_ != std::variant_npos) {
			storage_move_vtable[other.index_](storage_, std::move(other.storage_));
			index_ = other.index_;
			other.index_ = std::variant_npos;
		}
		return *this;
	}

	template <class T, class AT = typename AcceptedType<T&&>::type>
	any_variant& operator=(T&& value) {
		this->emplace<AT>(std::forward<T>(value));
		return *this;
	}

	~any_variant() {
		if (index_ != std::variant_npos) {
			storage_destruct_vtable[index_](storage_);
		}
	}

	// Observers

	constexpr std::size_t index() const noexcept { return index_; }

	constexpr bool has_value() const noexcept { return index_ != std::variant_npos; }

	// satisfied with std::variant
	constexpr bool valueless_by_exception() const noexcept { return !this->has_value(); }

	// Modifiers

protected:
	template <class T, std::size_t I, class... Args>
	ESL_ATTR_FORCEINLINE T& do_emplace(Args&&... args) {
		this->reset();
		auto& val = storage_.construct<T>(std::forward<Args>(args)...);
		index_ = I;
		return val;
	}

public:
	template <class T, class... Args, class I = typename ExactlyOnceIndex<T>::type>
	T& emplace(Args&&... args) {
		return this->do_emplace<T, I::value>(std::forward<Args>(args)...);
	}

	template <class T, class U, class... Args, class I = typename ExactlyOnceIndex<T>::type>
	T& emplace(std::initializer_list<U> il, Args&&... args) {
		return this->do_emplace<T, I::value>(il, std::forward<Args>(args)...);
	}

	template <std::size_t I, class... Args>
	std::variant_alternative_t<I, any_variant>& emplace(Args&&... args) {
		return this->do_emplace<std::variant_alternative_t<I, any_variant>, I>(std::forward<Args>(args)...);
	}

	template <std::size_t I, class U, class... Args>
	std::variant_alternative_t<I, any_variant>& emplace(std::initializer_list<U> il, Args&&... args) {
		return this->do_emplace<std::variant_alternative_t<I, any_variant>, I>(il, std::forward<Args>(args)...);
	}

	void swap(any_variant& other) noexcept {
		if (index_ == std::variant_npos) {
			if (other.index_ != std::variant_npos) {
				storage_move_vtable[other.index_](storage_, std::move(other.storage_));
				index_ = other.index_;
				other.index_ = std::variant_npos;
			}
		} else {
			if (other.index_ == std::variant_npos) {
				storage_move_vtable[index_](other.storage_, std::move(storage_));
				other.index_ = index_;
				index_ = std::variant_npos;
			} else {
				storage_swap_vtable[index_][other.index_](storage_, other.storage_);
				std::swap(index_, other.index_);
			}
		}
	}

	void reset() noexcept {
		if (index_ != std::variant_npos) {
			storage_destruct_vtable[index_](storage_);
			index_ = std::variant_npos;
		}
	}

protected:
	template <std::size_t I>
	ESL_ATTR_FORCEINLINE constexpr void do_check_index() const {
		if (index_ == std::variant_npos || index_ != I) {
			throw std::bad_variant_access{};
		}
	}

public:
	template <std::size_t I>
	constexpr std::variant_alternative_t<I, any_variant>& get() & {
		this->do_check_index<I>();
		return storage_.get<std::variant_alternative_t<I, any_variant>>();
	}

	template <std::size_t I>
	constexpr const std::variant_alternative_t<I, any_variant>& get() const& {
		this->do_check_index<I>();
		return storage_.get<std::variant_alternative_t<I, any_variant>>();
	}

	template <std::size_t I>
	constexpr std::variant_alternative_t<I, any_variant>&& get() && {
		return std::move(this->get<I>());
	}

	template <std::size_t I>
	constexpr const std::variant_alternative_t<I, any_variant>&& get() const&& {
		return std::move(this->get<I>());
	}

	template <class T>
	constexpr T& get() & {
		static_assert(is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
		return this->get<index_of_v<T, Ts...>>();
	}

	template <class T>
	constexpr const T& get() const& {
		static_assert(is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
		return this->get<index_of_v<T, Ts...>>();
	}

	template <class T>
	constexpr T&& get() && {
		static_assert(is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
		return std::move(this->get<index_of_v<T, Ts...>>());
	}

	template <class T>
	constexpr const T&& get() const&& {
		static_assert(is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
		return std::move(this->get<index_of_v<T, Ts...>>());
	}
};

} // namespace esl

namespace std {

// std::holds_alternative
template <class T, class... Ts>
inline constexpr bool holds_alternative(const ::esl::any_variant<Ts...>& va) noexcept {
	return va.index() == ::esl::index_of_v<T, Ts...>;
}

// std::get
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::any_variant<Ts...>>& get(::esl::any_variant<Ts...>& v) {
	return v.template get<I>();
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::any_variant<Ts...>>&& get(::esl::any_variant<Ts...>&& v) {
	return std::move(v).template get<I>();
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::any_variant<Ts...>> const& get(const ::esl::any_variant<Ts...>& v) {
	return v.template get<I>();
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::any_variant<Ts...>> const&& get(const ::esl::any_variant<Ts...>&& v) {
	return std::move(v).template get<I>();
}
template <class T, class... Ts>
inline constexpr T& get(::esl::any_variant<Ts...>& v) {
	return v.template get<T>();
}
template <class T, class... Ts>
inline constexpr T&& get(::esl::any_variant<Ts...>&& v) {
	return std::move(v).template get<T>();
}
template <class T, class... Ts>
inline constexpr const T& get(const ::esl::any_variant<Ts...>& v) {
	return v.template get<T>();
}
template <class T, class... Ts>
inline constexpr const T&& get(const ::esl::any_variant<Ts...>&& v) {
	return std::move(v).template get<T>();
}

// std::get_if
template <size_t I, class... Ts>
inline constexpr add_pointer_t<variant_alternative_t<I, ::esl::any_variant<Ts...>>> get_if(::esl::any_variant<Ts...>* vap) {
	return (vap && vap->index() == I) ? &std::get<I>(vap) : nullptr;
}
template <size_t I, class... Ts>
inline constexpr add_pointer_t<const variant_alternative_t<I, ::esl::any_variant<Ts...>>> get_if(const ::esl::any_variant<Ts...>* vap) {
	return (vap && vap->index() == I) ? &std::get<I>(vap) : nullptr;
}
template <class T, class... Ts>
inline constexpr add_pointer_t<T> get_if(::esl::any_variant<Ts...>* vap) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	using index_type = ::esl::index_of<T, Ts...>;
	return (vap && vap->index() == index_type::value) ? &std::get<index_type::value>(vap) : nullptr;
}
template <class T, class... Ts>
inline constexpr add_pointer_t<const T> get_if(const ::esl::any_variant<Ts...>* vap) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	using index_type = ::esl::index_of<T, Ts...>;
	return (vap && vap->index() == index_type::value) ? &std::get<index_type::value>(vap) : nullptr;
}

// std::swap
template <class... Ts>
inline void swap(::esl::any_variant<Ts...>& lhs, ::esl::any_variant<Ts...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	lhs.swap(rhs);
}

// std::hash
template <class... Ts>
struct hash<::esl::any_variant<Ts...>> {
private:
	using hasher_tuple_type = tuple<hash<remove_const_t<Ts>>...>;
	using hash_func_type = size_t (*)(const hasher_tuple_type&, const ::esl::any_variant<Ts...>&);
	hasher_tuple_type hashers_;
	template <size_t I>
	static size_t hash_(const hasher_tuple_type& hashers, const ::esl::any_variant<Ts...>& va) {
		return ::esl::hash_combine(std::get<I>(hashers)(std::get<I>(va)), I);
	}
	template <size_t... Is>
	static constexpr array<hash_func_type, sizeof...(Ts)> gen_hash_vtable(index_sequence<Is...>) {
		return {{hash_<Is>...}};
	}
	static constexpr auto hash_vtable = gen_hash_vtable(index_sequence_for<Ts...>{});
public:
	size_t operator()(const ::esl::any_variant<Ts...>& va) const { return hash_vtable[va.index()](hashers_, va); }
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

template <template <class> class Comp, class... Ts>
struct any_variant_comp {
private:
	using comp_func_type = bool (*)(const any_variant<Ts...>& lhs, const any_variant<Ts...>& rhs);
	template <std::size_t I, class T>
	static bool comp_(const any_variant<Ts...>& lhs, const any_variant<Ts...>& rhs) { return Comp<T>{}(std::get<I>(lhs), std::get<I>(rhs)); }
	template <std::size_t... Is>
	static constexpr std::array<comp_func_type, sizeof...(Ts)> gen_comp_vtable(std::index_sequence<Is...>) {
		return {{comp_<Is, Ts>...}};
	}
	static constexpr auto comp_vtable = gen_comp_vtable(std::index_sequence_for<Ts...>{});
public:
	constexpr bool operator()(const any_variant<Ts...>& lhs, const any_variant<Ts...>& rhs) const { return comp_vtable[lhs.index()](lhs, rhs); }
};

} // namespace details

// operators
template <class... Ts>
inline constexpr bool operator==(const any_variant<Ts...>& lhs, const any_variant<Ts...>& rhs) {
	return lhs.index() == rhs.index() && (lhs.valueless_by_exception() || details::any_variant_comp<std::equal_to, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator!=(const any_variant<Ts...>& lhs, const any_variant<Ts...>& rhs) {
	return lhs.index() != rhs.index() || (!lhs.valueless_by_exception() && details::any_variant_comp<std::not_equal_to, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator<(const any_variant<Ts...>& lhs, const any_variant<Ts...>& rhs) {
	if (rhs.valueless_by_exception()) {
		return false;
	}
	if (lhs.valueless_by_exception()) {
		return true;
	}
	return lhs.index() < rhs.index() || (lhs.index() == rhs.index() && details::any_variant_comp<std::less, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator>(const any_variant<Ts...>& lhs, const any_variant<Ts...>& rhs) {
	if (lhs.valueless_by_exception()) {
		return false;
	}
	if (rhs.valueless_by_exception()) {
		return true;
	}
	return lhs.index() > rhs.index() || (lhs.index() == rhs.index() && details::any_variant_comp<std::greater, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator<=(const any_variant<Ts...>& lhs, const any_variant<Ts...>& rhs) {
	if (lhs.valueless_by_exception()) {
		return true;
	}
	if (rhs.valueless_by_exception()) {
		return false;
	}
	return lhs.index() < rhs.index() || (lhs.index() == rhs.index() && details::any_variant_comp<std::less_equal, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator>=(const any_variant<Ts...>& lhs, const any_variant<Ts...>& rhs) {
	if (rhs.valueless_by_exception()) {
		return true;
	}
	if (lhs.valueless_by_exception()) {
		return false;
	}
	return lhs.index() > rhs.index() || (lhs.index() == rhs.index() && details::any_variant_comp<std::greater_equal, Ts...>{}(lhs, rhs));
}

} // namespace esl

#endif // ESL_ANY_VARIANT_HPP

