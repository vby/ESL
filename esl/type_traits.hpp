
#ifndef ESL_TYPE_TRAITS_HPP
#define ESL_TYPE_TRAITS_HPP

#include "macros.hpp"

#include <type_traits>
#include <iterator>
#include <tuple>

namespace esl {

inline constexpr std::size_t npos = std::size_t(-1);

struct not_fallback_t {};
template <class D>
struct fallback: std::false_type { using type = D; };
template <>
struct fallback<not_fallback_t> { using value_type = bool; static constexpr bool value = false; };

// macros

// ESL_IMPL_WELL_FORMED
// name, name##_v
#define ESL_IMPL_WELL_FORMED(name, type_expr) \
	template <class T, class = void> \
	struct name: std::false_type {}; \
	template <class T> \
	struct name<T, std::void_t<type_expr>>: std::true_type {}; \
	template <class T> \
	inline constexpr bool name##_v = name<T>::value;

// ESL_IMPL_WELL_FORMED_ARGS
// name, name##_v
#define ESL_IMPL_WELL_FORMED_ARGS(name, type_expr) \
	template <class T, class Args, class = void> \
	struct name##_tuple_: std::false_type {}; \
	template <class T, class... Args> \
	struct name##_tuple_<T, std::tuple<Args...>, std::void_t<type_expr>>: std::true_type {}; \
	template <class T, class... Args> \
	using name = name##_tuple_<T, std::tuple<Args...>>; \
	template <class T, class... Args> \
	inline constexpr bool name##_v = name<T, Args...>::value;

// ESL_IMPL_HAS_MEMBER_VARIABLE
// name, name##_v
#define ESL_IMPL_HAS_MEMBER_VARIABLE(name, member) ESL_IMPL_WELL_FORMED(name, decltype(&T::member))

// ESL_IMPL_HAS_NON_OVERLOADED_MEMBER_FUNCTION
// name, name##_v
#define ESL_IMPL_HAS_NON_OVERLOADED_MEMBER_FUNCTION(name, member) ESL_IMPL_WELL_FORMED(name, decltype(&T::member))

// ESL_IMPL_MEMBER_TYPE
// name, name##_t
#define ESL_IMPL_MEMBER_TYPE(name, member) \
	template <class T, class D = ::esl::not_fallback_t, class = std::void_t<>> \
	struct name: ::esl::fallback<D> {}; \
	template <class T, class D> \
	struct name<T, D, std::void_t<typename T::member>>: std::true_type { using type = typename T::member; }; \
	template <class T> \
	inline constexpr bool name##_v = name<T>::value; \
	template <class T, class D = ::esl::not_fallback_t> \
	using name##_t = typename name<T, D>::type;

namespace details {

// not standard
//template <template <class...> class B, class... Ts>
//std::true_type is_base_of_template_test(B<Ts...>*);
template <template <class...> class B, class T>
std::true_type is_base_of_template_test(B<T>*);
template <template <class...> class B, class T, class T2>
std::true_type is_base_of_template_test(B<T, T2>*);
template <template <class...> class B, class T, class T2, class T3>
std::true_type is_base_of_template_test(B<T, T2, T3>*);
template <template <class...> class B, class T, class T2, class T3, class T4>
std::true_type is_base_of_template_test(B<T, T2, T3, T4>*);
template <template <class...> class B, class T, class T2, class T3, class T4, class T5>
std::true_type is_base_of_template_test(B<T, T2, T3, T4, T5>*);
template <template <class...> class B, class T, class T2, class T3, class T4, class T5, class T6>
std::true_type is_base_of_template_test(B<T, T2, T3, T4, T5, T6>*);
template <template <class...> class B>
std::false_type is_base_of_template_test(void*);
template <template <class...> class B, class D>
using is_base_of_template_pre = decltype(details::is_base_of_template_test<B>(std::declval<D*>()));
template <template <class...> class B, class D, class = void>
struct is_base_of_template: std::is_class<D> {};
template <template <class...> class B, class D>
struct is_base_of_template<B, D, std::void_t<details::is_base_of_template_pre<B, D>>>: details::is_base_of_template_pre<B, D> {};

template <class From, class To>
static std::true_type is_static_castable_test(std::add_pointer_t<decltype(static_cast<To>(std::declval<From>()))>);
template <class From, class To>
static std::false_type is_static_castable_test(...);
template <class From, class To>
using is_static_castable = decltype(is_static_castable_test<From, To>(nullptr));

template <class T>
struct overload_resolve_candidate { T operator()(T); };
template <class... Ts>
struct overload_resolve_overload: overload_resolve_candidate<Ts>... {
    using overload_resolve_candidate<Ts>::operator()...;
};

template <class T, class... Ts>
using overload_resolve_type = decltype(overload_resolve_overload<Ts...>{}(std::declval<T>()));

template <class T, class Tuple, class = void>
struct overload_resolve {};
template <class T, class... Ts>
struct overload_resolve<T, std::tuple<Ts...>, std::void_t<overload_resolve_type<T, Ts...>>> {
    using type = overload_resolve_type<T, Ts...>;
};

template <class T, class Tuple, class = void>
struct is_overload_resolvable: std::false_type {};
template <class T, class... Ts>
struct is_overload_resolvable<T, std::tuple<Ts...>, std::void_t<overload_resolve_type<T, Ts...>>>: std::true_type {};

} // namespace details

// is_base_of_template, is_base_of_template_v
template <template <class...> class B, class D>
using is_base_of_template = details::is_base_of_template<B, std::remove_cv_t<D>>;
template <template <class...> class B, class D>
inline constexpr bool is_base_of_template_v = is_base_of_template<B, D>::value;

// is_one_of
template <class T, class... Us>
struct is_one_of: std::false_type {};
template <class T, class U, class... Us>
struct is_one_of<T, U, Us...>: std::bool_constant<std::is_same_v<T, U> || is_one_of<T, Us...>::value> {};
template <class T, class... Us>
inline constexpr bool is_one_of_v = is_one_of<T, Us...>::value;

#define ESL_IMPL_HAS_NON_OVERLOADED_MEMBER_FUNCTION_ESL_(name, member) \
	ESL_IMPL_HAS_NON_OVERLOADED_MEMBER_FUNCTION(has_non_overloaded_##name, member)

#define ESL_IMPL_MEMBER_TYPE_ESL_(name)  ESL_IMPL_MEMBER_TYPE(member_type_##name, name)

// is_datable, is_datable_v
ESL_IMPL_WELL_FORMED(is_datable, decltype(std::data(std::declval<T>())))

// is_sizeable, is_sizeable_v
ESL_IMPL_WELL_FORMED(is_sizeable, decltype(std::size(std::declval<T>())))

// is_emptiable, is_emptiable_v
ESL_IMPL_WELL_FORMED(is_emptiable, decltype(std::empty(std::declval<T>())))

// is_static_castable, is_static_castable_v
template <class From, class To>
using is_static_castable = details::is_static_castable<From, To>;
template <class From, class To>
inline constexpr bool is_static_castable_v = is_static_castable<From, To>::value;

// is_emplaceable, is_emplaceable_v
ESL_IMPL_WELL_FORMED_ARGS(is_emplaceable, decltype(std::declval<T>().emplace(std::forward<Args>(std::declval<Args>())...)))

// is_emplace_backable, is_emplace_backable_v
ESL_IMPL_WELL_FORMED_ARGS(is_emplace_backable, decltype(std::declval<T>().emplace_back(std::forward<Args>(std::declval<Args>())...)))

// has_non_overloaded_operator_parentheses, has_non_overloaded_operator_parentheses_v
ESL_IMPL_HAS_NON_OVERLOADED_MEMBER_FUNCTION_ESL_(operator_parentheses, operator())

// has_non_overloaded_operator_brackets, has_non_overloaded_operator_brackets_v
ESL_IMPL_HAS_NON_OVERLOADED_MEMBER_FUNCTION_ESL_(operator_brackets, operator[])

// member_type_##name, member_type_##name_t, member_type_##name_v
ESL_IMPL_MEMBER_TYPE_ESL_(type)
ESL_IMPL_MEMBER_TYPE_ESL_(value_type)
ESL_IMPL_MEMBER_TYPE_ESL_(size_type)
ESL_IMPL_MEMBER_TYPE_ESL_(difference_type)
ESL_IMPL_MEMBER_TYPE_ESL_(allocator_type)
ESL_IMPL_MEMBER_TYPE_ESL_(pointer)
ESL_IMPL_MEMBER_TYPE_ESL_(const_pointer)
ESL_IMPL_MEMBER_TYPE_ESL_(reference)
ESL_IMPL_MEMBER_TYPE_ESL_(const_reference)
ESL_IMPL_MEMBER_TYPE_ESL_(iterator)
ESL_IMPL_MEMBER_TYPE_ESL_(const_iterator)
ESL_IMPL_MEMBER_TYPE_ESL_(reverse_iterator)
ESL_IMPL_MEMBER_TYPE_ESL_(const_reverse_iterator)
ESL_IMPL_MEMBER_TYPE_ESL_(traits_type)
ESL_IMPL_MEMBER_TYPE_ESL_(element_type)
ESL_IMPL_MEMBER_TYPE_ESL_(deleter_type)
ESL_IMPL_MEMBER_TYPE_ESL_(is_always_equal)
ESL_IMPL_MEMBER_TYPE_ESL_(weak_type)
ESL_IMPL_MEMBER_TYPE_ESL_(native_handle_type)
ESL_IMPL_MEMBER_TYPE_ESL_(mutex_type)


/// basic traits ///

// remove_cvr, remove_cvr_t
template <class T>
using remove_cvr = std::remove_cv<std::remove_reference_t<T>>;
template <class T>
using remove_cvr_t = typename remove_cvr<T>::type;

// remove_pcv, remove_pcv_t
template <class T>
using remove_pcv = std::remove_cv<std::remove_pointer_t<T>>;
template <class T>
using remove_pcv_t = typename remove_pcv<T>::type;

// remove_rp, remove_rp_t
template <class T>
using remove_rp = std::remove_pointer<std::remove_reference_t<T>>;
template <class T>
using remove_rp_t = typename remove_rp<T>::type;

// remove_rpcv, remove_rpcv_t
template <class T>
using remove_rpcv = std::remove_cv<remove_rp_t<T>>;
template <class T>
using remove_rpcv_t = typename remove_rpcv<T>::type;

// to_lvalue_reference, to_lvalue_reference_t
template <class T>
using to_lvalue_reference = std::add_lvalue_reference<std::remove_reference_t<T>>;
template <class T>
using to_lvalue_reference_t = typename to_lvalue_reference<T>::type;

// to_rvalue_reference, to_rvalue_reference_t
template <class T>
using to_rvalue_reference = std::add_rvalue_reference<std::remove_reference_t<T>>;
template <class T>
using to_rvalue_reference_t = typename to_rvalue_reference<T>::type;

// remove_low_const, remove_low_const_t
template <class T>
struct remove_low_const {
	using type = T;
};
template <class T>
struct remove_low_const<const T&> {
	using type = T&;
};
template <class T>
struct remove_low_const<const T&&> {
	using type = T&&;
};
template <class T>
struct remove_low_const<const T*> {
	using type = T*;
};
template <class T>
using remove_low_const_t = typename remove_low_const<T>::type;

// is_low_const, is_low_const_v
template <class T>
using is_low_const = std::is_const<remove_rp_t<T>>;
template <class T>
inline constexpr bool is_low_const_v = is_low_const<T>::value;

// is_pointer_or_reference, is_pointer_or_reference_v
template <class T>
using is_pointer_or_reference = std::bool_constant<std::is_pointer_v<T> || std::is_reference_v<T>>;
template <class T>
inline constexpr bool is_pointer_or_reference_v = is_pointer_or_reference<T>::value;

// const_as, const_as_t
template <class T, class Ref, bool = std::is_const_v<Ref>>
struct const_as {
	using type = std::remove_const_t<T>;
};
template <class T, class Ref>
struct const_as<T, Ref, true> {
	using type = std::add_const_t<T>;
};
template <class T, class Ref>
using const_as_t = typename const_as<T, Ref>::type;

// pointer_as, pointer_as_t
template <class T, class Ref, bool = std::is_pointer_v<Ref>>
struct pointer_as {
	using type = std::remove_pointer_t<T>;
};
template <class T, class Ref>
struct pointer_as<T, Ref, true> {
	using type = std::add_pointer_t<T>;
};
template <class T, class Ref>
using pointer_as_t = typename pointer_as<T, Ref>::type;

// reference_as, reference_as_t
template <class T, class Ref, class Enable = void>
struct reference_as {
	using type = std::remove_reference_t<T>;
};
template <class T, class Ref>
struct reference_as<T, Ref, std::enable_if_t<std::is_lvalue_reference_v<Ref>>> {
	using type = to_lvalue_reference_t<T>;
};
template <class T, class Ref>
struct reference_as<T, Ref, std::enable_if_t<std::is_rvalue_reference_v<Ref>>> {
	using type = to_rvalue_reference_t<T>;
};
template <class T, class Ref>
using reference_as_t = typename reference_as<T, Ref>::type;

// not_void_or, not_void_or_t
template <class T, class U>
using not_void_or = std::conditional<!std::is_void_v<T>, T, U>;
template <class T, class U>
using not_void_or_t = typename not_void_or<T, U>::type;

// overload_resolve, overload_resolve_t
template <class T, class... Ts>
struct overload_resolve: details::overload_resolve<T, std::tuple<Ts...>> {};
template <class T, class... Ts>
using overload_resolve_t = typename overload_resolve<T, Ts...>::type;

// is_overload_resolvable, is_overload_resolvable_v
template <class T, class... Ts>
struct is_overload_resolvable: details::is_overload_resolvable<T, std::tuple<Ts...>> {};
template <class T, class... Ts>
inline constexpr bool is_overload_resolvable_v = is_overload_resolvable<T, Ts...>::value;

/// types traits ///

// types_nth, types_nth_t
template <std::size_t I, class... Ts>
struct types_nth;
template <std::size_t I, class First, class... Rest>
struct types_nth<I, First, Rest...>: types_nth<I - 1, Rest...> { };
template <class First, class... Rest>
struct types_nth<0, First, Rest...> { using type = First; };
template <std::size_t I, class... Ts>
using types_nth_t = typename types_nth<I, Ts...>::type;

// types_index, types_index_v
template <class T, std::size_t Size, class... Ts>
struct types_index_ {};
template <class T, std::size_t Size, class First, class... Rest>
struct types_index_<T, Size, First, Rest...>: types_index_<T, Size, Rest...> {};
template <class T, std::size_t Size, class... Rest>
struct types_index_<T, Size, T, Rest...>: std::integral_constant<std::size_t, Size - sizeof...(Rest) - 1> {};
template <class T, class... Ts>
struct types_index: types_index_<T, sizeof...(Ts), Ts...> {};
template <class T, class... Ts>
inline constexpr std::size_t types_index_v = types_index<T, Ts...>::value;

// types_rindex, types_rindex_v
template <class T, std::size_t Size, std::size_t CSize, class... Ts>
struct types_rindex_: std::integral_constant<std::size_t, Size - CSize - 1> {};
template <class T, std::size_t Size, std::size_t CSize, class First, class... Rest>
struct types_rindex_<T, Size, CSize, First, Rest...>: types_rindex_<T, Size, CSize, Rest...> {};
template <class T, std::size_t Size, std::size_t CSize, class... Rest>
struct types_rindex_<T, Size, CSize, T, Rest...>: types_rindex_<T, Size, sizeof...(Rest), Rest...> {};
template <class T, std::size_t Size>
struct types_rindex_<T, Size, Size> {};
template <class T, class... Ts>
struct types_rindex: types_rindex_<T, sizeof...(Ts), sizeof...(Ts), Ts...> {};
template <class T, class... Ts>
inline constexpr std::size_t types_rindex_v = types_rindex<T, Ts...>::value;

// types_count, types_count_v
template <class T, class... Ts>
struct types_count: std::integral_constant<std::size_t, 0> {};
template <class T, class First, class... Rest>
struct types_count<T, First, Rest...>: types_count<T, Rest...> {};
template <class T, class... Rest>
struct types_count<T, T, Rest...>: std::integral_constant<std::size_t, types_count<T, Rest...>::value + 1> {};
template <class T, class... Ts>
inline constexpr std::size_t types_count_v = types_count<T, Ts...>::value;

// types_exactly_once, types_exactly_once_v
template <class T, class... Ts>
struct types_exactly_once: std::bool_constant<types_count_v<T, Ts...> == 1> {};
template <class T, class... Ts>
inline constexpr bool types_exactly_once_v = types_exactly_once<T, Ts...>::value;

// types_exactly_once_index, types_exactly_once_index_v
template <class T, bool Once, class... Ts>
struct types_exactly_once_index_ {};
template <class T, class... Ts>
struct types_exactly_once_index_<T, true, Ts...>: types_index<T, Ts...> {};
template <class T, class... Ts>
struct types_exactly_once_index: types_exactly_once_index_<T, types_exactly_once_v<T, Ts...>, Ts...> {};
template <class T, class... Ts>
inline constexpr std::size_t types_exactly_once_index_v = types_exactly_once_index<T, Ts...>::value;

/// tuple traits ///

// tuple_index, tuple_index_v
template <class T, class Tup>
struct tuple_index;
template <class T, class... Ts>
struct tuple_index<T, std::tuple<Ts...>>: types_index<T, Ts...> {};
template <class T, class Tup>
inline constexpr std::size_t tuple_index_v = tuple_index<T, Tup>::value;

// tuple_rindex, tuple_rindex_v
template <class T, class Tup>
struct tuple_rindex;
template <class T, class... Ts>
struct tuple_rindex<T, std::tuple<Ts...>>: types_rindex<T, Ts...> {};
template <class T, class Tup>
inline constexpr std::size_t tuple_rindex_v = tuple_rindex<T, Tup>::value;

// tuple_count, tuple_count_v
template <class T, class Tup>
struct tuple_count;
template <class T, class... Ts>
struct tuple_count<T, std::tuple<Ts...>>: types_count<T, Ts...> {};
template <class T, class Tup>
inline constexpr std::size_t tuple_count_v = tuple_count<T, Tup>::value;

// tuple_exactly_once, tuple_exactly_once_v
template <class T, class Tup>
struct tuple_exactly_once;
template <class T, class... Ts>
struct tuple_exactly_once<T, std::tuple<Ts...>>: types_exactly_once<T, Ts...> {};
template <class T, class... Ts>
inline constexpr bool tuple_exactly_once_v = tuple_exactly_once<T, Ts...>::value;

// tuple_exactly_once_index, tuple_exactly_once_index_v
template <class T, class Tup>
struct tuple_exactly_once_index;
template <class T, class... Ts>
struct tuple_exactly_once_index<T, std::tuple<Ts...>>: types_exactly_once_index<T, Ts...> {};
template <class T, class Tup>
inline constexpr std::size_t tuple_exactly_once_index_v = tuple_exactly_once_index<T, Tup>::value;

// tuple_sub, tuple_sub_t
template <class Tup, std::size_t Pos, std::size_t Cnt, class = std::make_index_sequence<Cnt>>
struct tuple_sub;
template <class Tup, std::size_t Pos, std::size_t Cnt, std::size_t... Is>
struct tuple_sub<Tup, Pos, Cnt, std::index_sequence<Is...>> {
	using type = std::tuple<std::tuple_element_t<Pos + Is, Tup>...>;
};
template <class Tup, std::size_t Pos, std::size_t Cnt>
using tuple_sub_t = typename tuple_sub<Tup, Pos, Cnt>::type;

// tuple_unique, tuple_unique_t
template <class Tup, class T, bool = tuple_count_v<T, Tup> != 0>
struct tuple_unique_append_ { using type = Tup; };
template <class T, class... Ts>
struct tuple_unique_append_<std::tuple<Ts...>, T, true> { using type = std::tuple<Ts..., T>; };
template <class Tup, class = std::tuple<>>
struct tuple_unique;
template <class TupU>
struct tuple_unique<std::tuple<>, TupU> { using type = TupU; };
template <class TupU, class T, class... Ts>
struct tuple_unique<std::tuple<T, Ts...>, TupU> { using type = typename tuple_unique<typename tuple_unique_append_<TupU, T>::type, std::tuple<Ts...>>::type; };
template <class Tup>
using tuple_unique_t = typename tuple_unique<Tup>::type;

// tuple_apply_template, tuple_apply_template_t
template <template <class...> class TT, class Tup>
struct tuple_apply_template;
template <template <class...> class TT, class... Ts>
struct tuple_apply_template<TT, std::tuple<Ts...>> {
	using type = TT<Ts...>;
};
template <template <class...> class TT, class Tup>
using tuple_apply_template_t = typename tuple_apply_template<TT, Tup>::type;

// aligned_type_storage
template <class T>
using aligned_type_storage = std::aligned_storage<sizeof(T), alignof(T)>;

/// function traits ///

// is_function_or_function_pointer, is_function_or_function_pointer_v
template <class T>
using is_function_or_function_pointer = std::is_function<std::remove_pointer_t<T>>;
template <class T>
inline constexpr bool is_function_or_function_pointer_v = is_function_or_function_pointer<T>::value;

// is_function_pointer, is_function_pointer_v
template <class T>
using is_function_pointer = std::bool_constant<std::is_pointer_v<T> && is_function_or_function_pointer_v<T>>;
template <class T>
inline constexpr bool is_function_pointer_v = is_function_pointer<T>::value;

// is_primitive_function, is_primitive_function_v
// function || function pointer || member function pointer
template <class T>
using is_primitive_function = std::bool_constant<is_function_or_function_pointer_v<T> || std::is_member_function_pointer_v<T>>;
template <class T>
inline constexpr bool is_primitive_function_v = is_primitive_function<T>::value;

// is_non_overloaded_invocable, is_non_overloaded_invocable_v
// primitive function || has non-overloaded operator()
template <class T>
using is_non_overloaded_invocable = std::bool_constant<is_primitive_function_v<T> || has_non_overloaded_operator_parentheses_v<T>>;
template <class T>
inline constexpr bool is_non_overloaded_invocable_v = is_non_overloaded_invocable<T>::value;

// function_of, function_of_t, noexcept_function_of_t
template <class T, class Ret, class... Args>
struct function_of {
	using type = Ret(Args...);
	using noexcept_type = Ret(Args...) noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<T&, Ret, Args...> {
	using type = Ret(Args...) &;
	using noexcept_type = Ret(Args...) & noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<T&&, Ret, Args...> {
	using type = Ret(Args...) &&;
	using noexcept_type = Ret(Args...) && noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<const T, Ret, Args...> {
	using type = Ret(Args...) const;
	using noexcept_type = Ret(Args...) const noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<const T&, Ret, Args...> {
	using type = Ret(Args...) const&;
	using noexcept_type = Ret(Args...) const& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<const T&&, Ret, Args...> {
	using type = Ret(Args...) const&&;
	using noexcept_type = Ret(Args...) const&& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<volatile T, Ret, Args...> {
	using type = Ret(Args...) volatile;
	using noexcept_type = Ret(Args...) volatile noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<volatile T&, Ret, Args...> {
	using type = Ret(Args...) volatile&;
	using noexcept_type = Ret(Args...) volatile& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<volatile T&&, Ret, Args...> {
	using type = Ret(Args...) volatile&&;
	using noexcept_type = Ret(Args...) volatile&& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<const volatile T, Ret, Args...> {
	using type = Ret(Args...) const volatile;
	using noexcept_type = Ret(Args...) const volatile noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<const volatile T&, Ret, Args...> {
	using type = Ret(Args...) const volatile&;
	using noexcept_type = Ret(Args...) const volatile& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of<const volatile T&&, Ret, Args...> {
	using type = Ret(Args...) const volatile&&;
	using noexcept_type = Ret(Args...) const volatile&& noexcept;
};
template <class T, class Ret, class... Args>
using function_of_t = typename function_of<T, Ret, Args...>::type;
template <class T, class Ret, class... Args>
using noexcept_function_of_t = typename function_of<T, Ret, Args...>::noexcept_type;

// member_function_pointer_of, member_function_pointer_of_t, noexcept_member_function_pointer_t
template <class T, class Ret, class... Args>
struct member_function_pointer_of {
private:
	using class_type = remove_cvr_t<T>;
	using function_of_type = function_of<T, Ret, Args...>;
public:
	using type = typename function_of_type::type class_type::*;
	using noexcept_type = typename function_of_type::noexcept_type class_type::*;
};
template <class T, class Ret, class... Args>
using member_function_pointer_of_t = typename member_function_pointer_of<T, Ret, Args...>::type;
template <class T, class Ret, class... Args>
using noexcept_member_function_pointer_t = typename member_function_pointer_of<T, Ret, Args...>::noexcept_type;

// function_traits
template <class T, template <class...> class ArgsC, bool IsNoexcept, class Ret, class... Args>
struct function_traits_base_ {
	using function_type = T;
	using result_type = Ret;
	using args_type = ArgsC<Args...>;
	using is_noexcept = std::bool_constant<IsNoexcept>;
};
template <class T, template <class...> class ArgsC = std::tuple, class = T>
struct function_traits;
// not noexcept
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...)>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) &>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) &&>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const&>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const&&>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) volatile>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) volatile &>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) volatile &&>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const volatile>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const volatile&>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const volatile&&>: function_traits_base_<T, ArgsC, false, Ret, Args...> {};
// noexcept
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) & noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) && noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const& noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const&& noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) volatile noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) volatile & noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) volatile && noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const volatile noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const volatile& noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};
template <class T, template <class...> class ArgsC, class Ret, class... Args>
struct function_traits<T, ArgsC, Ret(Args...) const volatile&& noexcept>: function_traits_base_<T, ArgsC, true, Ret, Args...> {};

// member_function_pointer_traits
template <class T, template <class...> class ArgsC = std::tuple>
struct member_function_pointer_traits;
template <class F, class T, template <class...> class ArgsC>
struct member_function_pointer_traits<F T::*, ArgsC>: function_traits<F, ArgsC> {
	using member_function_pointer_type = F T::*;
	using class_type = T;
};

// non_overloaded_operator_parentheses_traits
// operator() non-overload
template <class T>
using non_overloaded_operator_parentheses_traits = member_function_pointer_traits<decltype(&T::operator())>;

} // namespace esl

#endif // ESL_TYPE_TRAITS_HPP

