#ifndef ESL_FLEX_VARIANT_HPP
#define ESL_FLEX_VARIANT_HPP

#include "macros.hpp"
#include "flex_storage.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "array.hpp"
#include "functional.hpp"

#include <variant>

// flex_variant
// * Use for create a variant with incomplete types
// * flex_variant satisfied with std::variant
//  except:
//    * Costructors are not constexpr except default

namespace esl {

template <class... Ts>
class flex_variant;

} // namespace esl

namespace std {

// std::variant_size
template <class... Ts>
struct variant_size<::esl::flex_variant<Ts...>>: integral_constant<size_t, sizeof...(Ts)> {};

// std::variant_alternative
template <size_t I, class... Ts>
struct variant_alternative<I, ::esl::flex_variant<Ts...>>: ::esl::nth_type<I, Ts...> {};

} // namespace std

namespace esl {

namespace details {
	struct FlexVariantStorageAccess;

	template <class Storage, class... Ts>
	struct FlexVariantStorageCopy {
		static constexpr auto vtable = make_tuple_vtable_v<Storage::template copy_construct_function, std::tuple<Ts...>>;
	};
	template <class Storage, class... Ts>
	struct FlexVariantStorageMove {
		static constexpr auto vtable = make_tuple_vtable_v<Storage::template move_construct_function, std::tuple<Ts...>>;
	};
}

template <class... Ts>
class flex_variant {
private:
	friend struct details::FlexVariantStorageAccess;

	using Storage = flex_storage<>;

	static constexpr auto storage_destruct_vtable = make_tuple_vtable_v<Storage::destruct_function, std::tuple<Ts...>>;
	static constexpr auto storage_swap_vtable = make_tuple_vtable_v<Storage::swap_function, std::tuple<Ts...>, std::tuple<Ts...>>;

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
	template <class T, bool = std::is_same_v<std::decay_t<T>, flex_variant>>
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
	//template <class T0 = nth_type_t<0, Ts...>, class = std::enable_if_t<std::is_default_constructible_v<T0>>>
	constexpr flex_variant() noexcept: storage_(std::in_place_type<nth_type_t<0, Ts...>>), index_(0) {}

	//template <bool Dep = true, class = std::enable_if_t<Dep && template_all_of_v<std::is_copy_constructible, Ts...>>>
	flex_variant(const flex_variant& other): index_(other.index_) {
		if (index_ != std::variant_npos) {
			details::FlexVariantStorageCopy<Storage, Ts...>::vtable[index_](storage_, other.storage_);
		}
	}

	//template <bool Dep = true, class = std::enable_if_t<Dep && template_all_of_v<std::is_move_constructible, Ts...>>>
	flex_variant(flex_variant&& other) noexcept: index_(other.index_) {
		if (index_ != std::variant_npos) {
			details::FlexVariantStorageMove<Storage, Ts...>::vtable[index_](storage_, std::move(other.storage_));
			other.index_ = std::variant_npos;
		}
	}

	template <class T, class AT = typename AcceptedType<T&&>::type>
	flex_variant(T&& value): flex_variant(std::in_place_type<AT>, std::forward<T>(value)) {}

	template <class T, class... Args, class I = typename ExactlyOnceIndex<T>::type>
	explicit flex_variant(std::in_place_type_t<T>, Args&&... args):
		storage_(std::in_place_type<T>, std::forward<Args>(args)...), index_(I::value) {}

	template <class T, class U, class... Args, class I = typename ExactlyOnceIndex<T>::type>
	explicit flex_variant(std::in_place_type_t<T>, std::initializer_list<U> il, Args&&... args):
		storage_(std::in_place_type<T>, il, std::forward<Args>(args)...), index_(I::value) {}

	template <std::size_t I, class... Args>
	explicit flex_variant(std::in_place_index_t<I>, Args&&... args):
		storage_(std::in_place_type<std::variant_alternative_t<I, flex_variant>>, std::forward<Args>(args)...), index_(I) {}

	template <std::size_t I, class U, class... Args>
	explicit flex_variant(std::in_place_index_t<I>, std::initializer_list<U> il, Args&&... args):
		storage_(std::in_place_type<std::variant_alternative_t<I, flex_variant>>, il, std::forward<Args>(args)...), index_(I) {}

	// TODO constructor with allocator_tag_t

	flex_variant& operator=(const flex_variant& other) {
		this->reset();
		if (other.index_ != std::variant_npos) {
			details::FlexVariantStorageCopy<Storage, Ts...>::vtable[other.index_](storage_, other.storage_);
			index_ = other.index_;
		}
		return *this;
	}

	flex_variant& operator=(flex_variant&& other) noexcept {
		this->reset();
		if (other.index_ != std::variant_npos) {
			details::FlexVariantStorageMove<Storage, Ts...>::vtable[other.index_](storage_, std::move(other.storage_));
			index_ = other.index_;
			other.index_ = std::variant_npos;
		}
		return *this;
	}

	template <class T, class AT = typename AcceptedType<T&&>::type>
	flex_variant& operator=(T&& value) {
		this->emplace<AT>(std::forward<T>(value));
		return *this;
	}

	~flex_variant() {
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
	std::variant_alternative_t<I, flex_variant>& emplace(Args&&... args) {
		return this->do_emplace<std::variant_alternative_t<I, flex_variant>, I>(std::forward<Args>(args)...);
	}

	template <std::size_t I, class U, class... Args>
	std::variant_alternative_t<I, flex_variant>& emplace(std::initializer_list<U> il, Args&&... args) {
		return this->do_emplace<std::variant_alternative_t<I, flex_variant>, I>(il, std::forward<Args>(args)...);
	}

	void swap(flex_variant& other) noexcept {
		if (index_ == std::variant_npos) {
			if (other.index_ != std::variant_npos) {
				details::FlexVariantStorageMove<Storage, Ts...>::vtable[other.index_](storage_, std::move(other.storage_));
				index_ = other.index_;
				other.index_ = std::variant_npos;
			}
		} else {
			if (other.index_ == std::variant_npos) {
				details::FlexVariantStorageMove<Storage, Ts...>::vtable[index_](other.storage_, std::move(storage_));
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

struct FlexVariantStorageAccess {
	template <class... Ts>
	static typename flex_variant<Ts...>::Storage& get(flex_variant<Ts...>& v) noexcept { return v.storage_; }

	template <class... Ts>
	static const typename flex_variant<Ts...>::Storage& get(const flex_variant<Ts...>& v) noexcept { return v.storage_; }
};

} // namespace details

} // namespace esl

namespace std {

// std::holds_alternative
template <class T, class... Ts>
inline constexpr bool holds_alternative(const ::esl::flex_variant<Ts...>& v) noexcept {
	return v.index() == ::esl::index_of_v<T, Ts...>;
}

// std::get<I>
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::flex_variant<Ts...>>& get(::esl::flex_variant<Ts...>& v) {
	if (v.index() != I) {
		throw std::bad_variant_access{};
	}
	return ::esl::details::FlexVariantStorageAccess::get(v).template get<variant_alternative_t<I, ::esl::flex_variant<Ts...>>>();
}
template <size_t I, class... Ts>
inline constexpr const variant_alternative_t<I, ::esl::flex_variant<Ts...>>& get(const ::esl::flex_variant<Ts...>& v) {
	if (v.index() != I) {
		throw std::bad_variant_access{};
	}
	return ::esl::details::FlexVariantStorageAccess::get(v).template get<variant_alternative_t<I, ::esl::flex_variant<Ts...>>>();
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::flex_variant<Ts...>>&& get(::esl::flex_variant<Ts...>&& v) {
	return std::move(std::get<I>(v));
}
template <size_t I, class... Ts>
inline constexpr variant_alternative_t<I, ::esl::flex_variant<Ts...>> const&& get(const ::esl::flex_variant<Ts...>&& v) {
	return std::move(std::get<I>(v));
}
// std::get<T>
template <class T, class... Ts>
inline constexpr T& get(::esl::flex_variant<Ts...>& v) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return get<::esl::index_of_v<T, Ts...>>(v);
}
template <class T, class... Ts>
inline constexpr const T& get(const ::esl::flex_variant<Ts...>& v) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return get<::esl::index_of_v<T, Ts...>>(v);
}
template <class T, class... Ts>
inline constexpr T&& get(::esl::flex_variant<Ts...>&& v) {
	return move(get<T>(v));
}
template <class T, class... Ts>
inline constexpr const T&& get(const ::esl::flex_variant<Ts...>&& v) {
	return move(get<T>(v));
}

// std::get_if
template <size_t I, class... Ts>
inline constexpr add_pointer_t<variant_alternative_t<I, ::esl::flex_variant<Ts...>>> get_if(::esl::flex_variant<Ts...>* vap) {
	if (!vap || vap->index() != I) {
		return nullptr;
	}
	return &::esl::details::FlexVariantStorageAccess::get(*vap).template get<variant_alternative_t<I, ::esl::flex_variant<Ts...>>>();
}
template <size_t I, class... Ts>
inline constexpr add_pointer_t<const variant_alternative_t<I, ::esl::flex_variant<Ts...>>> get_if(const ::esl::flex_variant<Ts...>* vap) {
	if (!vap || vap->index() != I) {
		return nullptr;
	}
	return &::esl::details::FlexVariantStorageAccess::get(*vap).template get<variant_alternative_t<I, ::esl::flex_variant<Ts...>>>();
}
template <class T, class... Ts>
inline constexpr add_pointer_t<T> get_if(::esl::flex_variant<Ts...>* vap) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return get_if<::esl::index_of_v<T, Ts...>>(vap);
}
template <class T, class... Ts>
inline constexpr add_pointer_t<const T> get_if(const ::esl::flex_variant<Ts...>* vap) {
	static_assert(::esl::is_exactly_once_v<T, Ts...>, "T should occur for exactly once in alternatives");
	return get_if<::esl::index_of_v<T, Ts...>>(vap);
}

// std::swap
template <class... Ts>
inline void swap(::esl::flex_variant<Ts...>& lhs, ::esl::flex_variant<Ts...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	lhs.swap(rhs);
}

// std::hash
template <class... Ts>
struct hash<::esl::flex_variant<Ts...>> {
private:
	using hasher_type = tuple<hash<decay_t<Ts>>...>;
	hasher_type hasher_;

	template <size_t I> struct HashFunction {
		static size_t value(const hasher_type& hasher, const ::esl::flex_variant<Ts...>& v) {
			return ::esl::hash_combine(get<I>(hasher)(std::get<I>(v)), I);
		}
	};
	static constexpr auto vtable = ::esl::make_index_sequence_vtable_v<HashFunction, sizeof...(Ts)>;
public:
	size_t operator()(const ::esl::flex_variant<Ts...>& v) const { return vtable[v.index()](hasher_, v); }
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
struct VariantComp {
private:
	template <std::size_t I> struct CompFunction {
		static bool value(const flex_variant<Ts...>& lhs, const flex_variant<Ts...>& rhs) {
			return Comp<nth_type_t<I, Ts...>>{}(std::get<I>(lhs), std::get<I>(rhs));
		}
	};
	static constexpr auto vtable = make_index_sequence_vtable_v<CompFunction, sizeof...(Ts)>;
public:
	constexpr bool operator()(const flex_variant<Ts...>& lhs, const flex_variant<Ts...>& rhs) const { return vtable[lhs.index()](lhs, rhs); }
};

template <class ResultType, class Visitor, class... Variants>
struct VariantVisit {
private:
	template <std::size_t... Is> struct InvokeFunction {
		static constexpr ResultType value(Visitor vis, Variants... vars) {
			return std::invoke(std::forward<Visitor>(vis), std::get<Is>(std::forward<Variants>(vars))...);
		}
	};
	static constexpr auto vtable = make_index_sequence_vtable_v<InvokeFunction, std::variant_size_v<std::remove_reference_t<Variants>>...>;
	static constexpr std::size_t index(std::size_t index) noexcept(false) {
		if (index == std::variant_npos) {
			throw std::bad_variant_access{};
		}
		return index;
	}
public:
	static constexpr ResultType invoke(Visitor vis, Variants... vars) {
		return access(vtable, index(vars.index())...)(std::forward<Visitor>(vis), std::forward<Variants>(vars)...);
	}
};

} // namespace details

// visit
template <class Visitor, class... Variants>
inline constexpr decltype(auto) visit(Visitor&& vis, Variants&&... vars) {
	// TODO: use std::common_type, libstdc++ seems also get result type by this way
	using ResultType = decltype(std::invoke(std::forward<Visitor>(vis), std::get<0>(std::forward<Variants>(vars))...));
	return details::VariantVisit<ResultType, Visitor&&, Variants&&...>::invoke(std::forward<Visitor>(vis), std::forward<Variants>(vars)...);
}

// operators
template <class... Ts>
inline constexpr bool operator==(const flex_variant<Ts...>& lhs, const flex_variant<Ts...>& rhs) {
	return lhs.index() == rhs.index() && (lhs.valueless_by_exception() || details::VariantComp<std::equal_to, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator!=(const flex_variant<Ts...>& lhs, const flex_variant<Ts...>& rhs) {
	return lhs.index() != rhs.index() || (!lhs.valueless_by_exception() && details::VariantComp<std::not_equal_to, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator<(const flex_variant<Ts...>& lhs, const flex_variant<Ts...>& rhs) {
	if (rhs.valueless_by_exception()) {
		return false;
	}
	if (lhs.valueless_by_exception()) {
		return true;
	}
	return lhs.index() < rhs.index() || (lhs.index() == rhs.index() && details::VariantComp<std::less, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator>(const flex_variant<Ts...>& lhs, const flex_variant<Ts...>& rhs) {
	if (lhs.valueless_by_exception()) {
		return false;
	}
	if (rhs.valueless_by_exception()) {
		return true;
	}
	return lhs.index() > rhs.index() || (lhs.index() == rhs.index() && details::VariantComp<std::greater, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator<=(const flex_variant<Ts...>& lhs, const flex_variant<Ts...>& rhs) {
	if (lhs.valueless_by_exception()) {
		return true;
	}
	if (rhs.valueless_by_exception()) {
		return false;
	}
	return lhs.index() < rhs.index() || (lhs.index() == rhs.index() && details::VariantComp<std::less_equal, Ts...>{}(lhs, rhs));
}
template <class... Ts>
inline constexpr bool operator>=(const flex_variant<Ts...>& lhs, const flex_variant<Ts...>& rhs) {
	if (rhs.valueless_by_exception()) {
		return true;
	}
	if (lhs.valueless_by_exception()) {
		return false;
	}
	return lhs.index() > rhs.index() || (lhs.index() == rhs.index() && details::VariantComp<std::greater_equal, Ts...>{}(lhs, rhs));
}

} // namespace esl

#endif // ESL_FLEX_VARIANT_HPP

