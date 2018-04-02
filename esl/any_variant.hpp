#ifndef ESL_ANY_VARIANT_HPP
#define ESL_ANY_VARIANT_HPP

#include "any_storage.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "array.hpp"
#include "functional.hpp"

#include <variant>

// any_variant
// * Use for create a variant with incomplete types
// * any_variant satisfied with std::variant
//  except:
//    * Costructors are not constexpr except default

namespace esl {

template <class... Ts>
class any_variant;

} // namespace esl

namespace std {

// std::variant_size
template <class... Ts>
struct variant_size<::esl::any_variant<Ts...>>: integral_constant<size_t, sizeof...(Ts)> {};

// std::variant_alternative
template <size_t I, class... Ts>
struct variant_alternative<I, ::esl::any_variant<Ts...>>: ::esl::nth_type<I, Ts...> {};

} // namespace std

namespace esl {

namespace details {
	struct any_variant_storage_access;
}

template <class... Ts>
class any_variant {
private:
	friend struct details::any_variant_storage_access;

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

protected:
	void reset() noexcept {
		if (index_ != std::variant_npos) {
			storage_destruct_vtable[index_](storage_);
			index_ = std::variant_npos;
		}
	}

	template <class T, std::size_t I, class... Args>
	ESL_ATTR_FORCEINLINE T& do_emplace(Args&&... args) {
		this->reset();
		auto& val = storage_.construct<T>(std::forward<Args>(args)...);
		index_ = I;
		return val;
	}

public:
	constexpr any_variant() noexcept: storage_(std::in_place_type<nth_type_t<0, Ts...>>), index_(0) {}

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

	constexpr bool valueless_by_exception() const noexcept { return index_ == std::variant_npos; }

	// Modifiers

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
};

namespace details {
	struct any_variant_storage_access {
		template <class... Ts>
		static typename any_variant<Ts...>::Storage& get(any_variant<Ts...>& v) noexcept { return v.storage_; }

		template <class... Ts>
		static const typename any_variant<Ts...>::Storage& get(const any_variant<Ts...>& v) noexcept { return v.storage_; }
	};
}

} // namespace esl

namespace std {

// std::holds_alternative
template <class T, class... Ts>
inline constexpr bool holds_alternative(const ::esl::any_variant<Ts...>& v) noexcept {
	return v.index() == ::esl::index_of_v<T, Ts...>;
}

// std::get<I>
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::any_variant<Ts...>>& get(::esl::any_variant<Ts...>& v) {
	if (v.index() != I) {
		throw std::bad_variant_access{};
	}
	return ::esl::details::any_variant_storage_access::get(v).template get<variant_alternative_t<I, ::esl::any_variant<Ts...>>>();
}
template <size_t I, class... Ts>
inline constexpr const variant_alternative_t<I, ::esl::any_variant<Ts...>>& get(const ::esl::any_variant<Ts...>& v) {
	if (v.index() != I) {
		throw std::bad_variant_access{};
	}
	return ::esl::details::any_variant_storage_access::get(v).template get<variant_alternative_t<I, ::esl::any_variant<Ts...>>>();
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::any_variant<Ts...>>&& get(::esl::any_variant<Ts...>&& v) {
	return std::move(std::get<I>(v));
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::any_variant<Ts...>> const&& get(const ::esl::any_variant<Ts...>&& v) {
	return std::move(std::get<I>(v));
}
// std::get<T>
template <class T, class... Ts>
inline constexpr T& get(::esl::any_variant<Ts...>& v) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return get<::esl::index_of_v<T, Ts...>>(v);
}
template <class T, class... Ts>
inline constexpr const T& get(const ::esl::any_variant<Ts...>& v) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return get<::esl::index_of_v<T, Ts...>>(v);
}
template <class T, class... Ts>
inline constexpr T&& get(::esl::any_variant<Ts...>&& v) {
	return move(get<T>(v));
}
template <class T, class... Ts>
inline constexpr const T&& get(const ::esl::any_variant<Ts...>&& v) {
	return move(get<T>(v));
}

// std::get_if
template <size_t I, class... Ts>
inline constexpr add_pointer_t<variant_alternative_t<I, ::esl::any_variant<Ts...>>> get_if(::esl::any_variant<Ts...>* vap) {
	if (!vap || vap->index() != I) {
		return nullptr;
	}
	return &::esl::details::any_variant_storage_access::get(*vap).template get<variant_alternative_t<I, ::esl::any_variant<Ts...>>>();
}
template <size_t I, class... Ts>
inline constexpr add_pointer_t<const variant_alternative_t<I, ::esl::any_variant<Ts...>>> get_if(const ::esl::any_variant<Ts...>* vap) {
	if (!vap || vap->index() != I) {
		return nullptr;
	}
	return &::esl::details::any_variant_storage_access::get(*vap).template get<variant_alternative_t<I, ::esl::any_variant<Ts...>>>();
}
template <class T, class... Ts>
inline constexpr add_pointer_t<T> get_if(::esl::any_variant<Ts...>* vap) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return get_if<::esl::index_of_v<T, Ts...>>(vap);
}
template <class T, class... Ts>
inline constexpr add_pointer_t<const T> get_if(const ::esl::any_variant<Ts...>* vap) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return get_if<::esl::index_of_v<T, Ts...>>(vap);
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
	static size_t hash_(const hasher_tuple_type& hashers, const ::esl::any_variant<Ts...>& v) {
		return ::esl::hash_combine(std::get<I>(hashers)(std::get<I>(v)), I);
	}
	template <size_t... Is>
	static constexpr array<hash_func_type, sizeof...(Ts)> gen_hash_vtable(index_sequence<Is...>) {
		return {{hash_<Is>...}};
	}
	static constexpr auto hash_vtable = gen_hash_vtable(index_sequence_for<Ts...>{});
public:
	size_t operator()(const ::esl::any_variant<Ts...>& v) const { return hash_vtable[v.index()](hashers_, v); }
};

} // namespace std

namespace esl {

using std::variant_size;
using std::variant_size_v;
using std::variant_alternative;
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

template <class ResultType, class Visitor, class... Variants>
struct variant_visit {
private:
	template <std::size_t... Is>
	static constexpr ResultType invoke_alt(Visitor vis, Variants... vars) {
		return std::invoke(std::forward<Visitor>(vis), std::get<Is>(std::forward<Variants>(vars))...);
	}
	// Failed compile on MSVC(VS 2017)
	// MSVC std::make_index_sequence use __make_integer_seq, bugly expend with Variadic parameters
	template <std::size_t... Dimensions>
	struct MultiArray {
		using type = multi_array_t<ResultType(*)(Visitor, Variants...), Dimensions...>;
		using indexes_type = tuple_combination_t<integer_sequence_tuple_t<std::make_index_sequence<Dimensions>>...>;
	};
	using VTableMultiArray = MultiArray<std::variant_size_v<std::remove_reference_t<Variants>>...>;
	using ArrayType = typename VTableMultiArray::type;

	template <std::size_t... Is>
	static constexpr void apply_alt(ArrayType& vtbl, std::index_sequence<Is...>) {
		std::get<Is...>(vtbl) = invoke_alt<Is...>;
	}

	template <class Tup = typename VTableMultiArray::indexes_type>
	struct VTable {};
	template <class... Ts>
	struct VTable<std::tuple<Ts...>> {
		static constexpr ArrayType gen_vtable() {
			ArrayType vtbl{};
			(..., apply_alt(vtbl, tuple_integer_sequence_t<std::size_t, Ts>{}));
			return vtbl;
		}
		static constexpr ArrayType value = gen_vtable();
	};

public:
	static constexpr ResultType invoke(Visitor vis, Variants... vars) {
		return access(VTable<>::value, vars.index()...)(std::forward<Visitor>(vis), std::forward<Variants>(vars)...);
	}
};

} // namespace details

// visit
template <class Visitor, class... Variants>
inline constexpr decltype(auto) visit(Visitor&& vis, Variants&&... vars) {
	// TODO: use std::common_type, libstdc++ seems also get result type by this way
	using ResultType = decltype(std::invoke(std::forward<Visitor>(vis), std::get<0>(std::forward<Variants>(vars))...));
	return details::variant_visit<ResultType, Visitor&&, Variants&&...>::invoke(std::forward<Visitor>(vis), std::forward<Variants>(vars)...);
}

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

