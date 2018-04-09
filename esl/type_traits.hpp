
#ifndef ESL_TYPE_TRAITS_HPP
#define ESL_TYPE_TRAITS_HPP

#include <type_traits>
#include <iterator>
#include <tuple>

namespace esl {

inline constexpr std::size_t npos = std::size_t(-1);

// Macros to help impl traits
// --------------------------------------------------------

// ESL_IMPL_IS_WELL_FORMED
// name, name##_v
#define ESL_IMPL_IS_WELL_FORMED(name, type_expr) \
	template <class T, class = void> \
	struct name: std::false_type {}; \
	template <class T> \
	struct name<T, std::void_t<type_expr>>: std::true_type {}; \
	template <class T> \
	inline constexpr bool name##_v = name<T>::value;

// ESL_IMPL_IS_WELL_FORMED_ARGS
// name, name##_v
#define ESL_IMPL_IS_WELL_FORMED_ARGS(name, type_expr) \
	template <class T, class Args, class = void> \
	struct name##_tuple_: std::false_type {}; \
	template <class T, class... Args> \
	struct name##_tuple_<T, std::tuple<Args...>, std::void_t<type_expr>>: std::true_type {}; \
	template <class T, class... Args> \
	using name = name##_tuple_<T, std::tuple<Args...>>; \
	template <class T, class... Args> \
	inline constexpr bool name##_v = name<T, Args...>::value;

// ESL_IMPL_HAS_MEMBER_TYPE
// name, name##_v
#define ESL_IMPL_HAS_MEMBER_TYPE(name, member) ESL_IMPL_IS_WELL_FORMED(name, typename T::member)

// ESL_IMPL_HAS_NON_OVERLOADED_MEMBER
// name, name##_v
#define ESL_IMPL_HAS_NON_OVERLOADED_MEMBER(name, member) ESL_IMPL_IS_WELL_FORMED(name, decltype(&T::member))

// Type traits
// ---------------------------------------------------------

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
struct remove_low_const { using type = T; };
template <class T>
struct remove_low_const<const T&> { using type = T&; };
template <class T>
struct remove_low_const<const T&&> { using type = T&&; };
template <class T>
struct remove_low_const<const T*> { using type = T*; };
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
struct const_as { using type = std::remove_const_t<T>; };
template <class T, class Ref>
struct const_as<T, Ref, true> { using type = std::add_const_t<T>; };
template <class T, class Ref>
using const_as_t = typename const_as<T, Ref>::type;

// pointer_as, pointer_as_t
template <class T, class Ref, bool = std::is_pointer_v<Ref>>
struct pointer_as { using type = std::remove_pointer_t<T>; };
template <class T, class Ref>
struct pointer_as<T, Ref, true> { using type = std::add_pointer_t<T>; };
template <class T, class Ref>
using pointer_as_t = typename pointer_as<T, Ref>::type;

// reference_as, reference_as_t
template <class T, class Ref, class Enable = void>
struct reference_as { using type = std::remove_reference_t<T>; };
template <class T, class Ref>
struct reference_as<T, Ref, std::enable_if_t<std::is_lvalue_reference_v<Ref>>> { using type = to_lvalue_reference_t<T>; };
template <class T, class Ref>
struct reference_as<T, Ref, std::enable_if_t<std::is_rvalue_reference_v<Ref>>> { using type = to_rvalue_reference_t<T>; };
template <class T, class Ref>
using reference_as_t = typename reference_as<T, Ref>::type;

// is_one_of, is_one_of_v
template <class T, class... Us>
struct is_one_of: std::false_type {};
template <class T, class U, class... Us>
struct is_one_of<T, U, Us...>: std::bool_constant<std::is_same_v<T, U> || is_one_of<T, Us...>::value> {};
template <class T, class... Us>
inline constexpr bool is_one_of_v = is_one_of<T, Us...>::value;

// is_decay_to, is_decay_to_v
template <class From, class To>
struct is_decay_to: std::is_same<std::decay_t<From>, To> {};
template <class From, class To>
inline constexpr bool is_decay_to_v = is_decay_to<From, To>::value;

namespace details {

// is_base_of_template_test
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

} //namespace details

// is_base_of_template, is_base_of_template_v
template <template <class...> class B, class D>
using is_base_of_template = details::is_base_of_template<B, std::remove_cv_t<D>>;
template <template <class...> class B, class D>
inline constexpr bool is_base_of_template_v = is_base_of_template<B, D>::value;

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

// Supported operations
// ---------------------------------------------------------

namespace details {

// is_static_castable_test
template <class From, class To>
std::true_type is_static_castable_test(std::add_pointer_t<decltype(static_cast<To>(std::declval<From>()))>);
template <class From, class To>
std::false_type is_static_castable_test(...);
template <class From, class To>
using is_static_castable = decltype(is_static_castable_test<From, To>(nullptr));

} //namespace details

// is_static_castable, is_static_castable_v
template <class From, class To>
using is_static_castable = details::is_static_castable<From, To>;
template <class From, class To>
inline constexpr bool is_static_castable_v = is_static_castable<From, To>::value;

// is_datable, is_datable_v
ESL_IMPL_IS_WELL_FORMED(is_datable, decltype(std::data(std::declval<T>())))

// is_sizeable, is_sizeable_v
ESL_IMPL_IS_WELL_FORMED(is_sizeable, decltype(std::size(std::declval<T>())))

// is_emptiable, is_emptiable_v
ESL_IMPL_IS_WELL_FORMED(is_emptiable, decltype(std::empty(std::declval<T>())))

// is_emplaceable, is_emplaceable_v
ESL_IMPL_IS_WELL_FORMED_ARGS(is_emplaceable, decltype(std::declval<T>().emplace(std::forward<Args>(std::declval<Args>())...)))

// is_emplace_backable, is_emplace_backable_v
ESL_IMPL_IS_WELL_FORMED_ARGS(is_emplace_backable, decltype(std::declval<T>().emplace_back(std::forward<Args>(std::declval<Args>())...)))

// Has member
// ---------------------------------------------------------

// has_member_type_##name, has_member_type_##name_v
#define ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(name)  ESL_IMPL_HAS_MEMBER_TYPE(has_member_type_##name, name)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(value_type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(size_type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(difference_type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(allocator_type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(pointer)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(const_pointer)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(reference)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(const_reference)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(iterator)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(const_iterator)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(reverse_iterator)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(const_reverse_iterator)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(traits_type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(element_type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(deleter_type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(is_always_equal)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(weak_type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(native_handle_type)
ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_(mutex_type)
#undef ESL_IMPL_HAS_MEMBER_TYPE_INTERNAL_

// has_non_overloaded_operator_parentheses, has_non_overloaded_operator_parentheses_v
ESL_IMPL_HAS_NON_OVERLOADED_MEMBER(has_non_overloaded_operator_parentheses, operator())

// has_non_overloaded_operator_brackets, has_non_overloaded_operator_brackets_v
ESL_IMPL_HAS_NON_OVERLOADED_MEMBER(has_non_overloaded_operator_brackets, operator[])

// Variadic template traits
// ---------------------------------------------------------

// nth_type, nth_type_t
template <std::size_t I, class... Ts>
struct nth_type;
template <std::size_t I, class First, class... Rest>
struct nth_type<I, First, Rest...>: nth_type<I - 1, Rest...> { };
template <class First, class... Rest>
struct nth_type<0, First, Rest...> { using type = First; };
template <std::size_t I, class... Ts>
using nth_type_t = typename nth_type<I, Ts...>::type;

// index_of, index_of_v
template <class T, std::size_t Size, class... Ts>
struct index_of_;
template <class T, std::size_t Size, class First, class... Rest>
struct index_of_<T, Size, First, Rest...>: index_of_<T, Size, Rest...> {};
template <class T, std::size_t Size, class... Rest>
struct index_of_<T, Size, T, Rest...>: std::integral_constant<std::size_t, Size - sizeof...(Rest) - 1> {};
template <class T, class... Ts>
using index_of = index_of_<T, sizeof...(Ts), Ts...>;
template <class T, class... Ts>
inline constexpr std::size_t index_of_v = index_of<T, Ts...>::value;

// rindex_of, rindex_of_v
template <class T, std::size_t Size, std::size_t CSize, class... Ts>
struct rindex_of_: std::integral_constant<std::size_t, Size - CSize - 1> {};
template <class T, std::size_t Size, std::size_t CSize, class First, class... Rest>
struct rindex_of_<T, Size, CSize, First, Rest...>: rindex_of_<T, Size, CSize, Rest...> {};
template <class T, std::size_t Size, std::size_t CSize, class... Rest>
struct rindex_of_<T, Size, CSize, T, Rest...>: rindex_of_<T, Size, sizeof...(Rest), Rest...> {};
template <class T, std::size_t Size>
struct rindex_of_<T, Size, Size> {};
template <class T, class... Ts>
struct rindex_of: rindex_of_<T, sizeof...(Ts), sizeof...(Ts), Ts...> {};
template <class T, class... Ts>
inline constexpr std::size_t rindex_of_v = rindex_of<T, Ts...>::value;

// count_of, count_of_v
template <class T, class... Ts>
struct count_of: std::integral_constant<std::size_t, 0> {};
template <class T, class First, class... Rest>
struct count_of<T, First, Rest...>: count_of<T, Rest...> {};
template <class T, class... Rest>
struct count_of<T, T, Rest...>: std::integral_constant<std::size_t, count_of<T, Rest...>::value + 1> {};
template <class T, class... Ts>
inline constexpr std::size_t count_of_v = count_of<T, Ts...>::value;

// is_exactly_once, is_exactly_once_v
template <class T, class... Ts>
struct is_exactly_once: std::bool_constant<count_of_v<T, Ts...> == 1> {};
template <class T, class... Ts>
inline constexpr bool is_exactly_once_v = is_exactly_once<T, Ts...>::value;

// is_types_decay_to, is_types_decay_to_v
template <class T, class... Ts>
struct is_types_decay_to: std::false_type {};
template <class T, class First>
struct is_types_decay_to<T, First>: is_decay_to<First, T> {};
template <class T, class... Ts>
inline constexpr bool is_types_decay_to_v = is_types_decay_to<T, Ts...>::value;

// template_all_of, template_all_of_v
template <template <class> class Pred, class... Ts>
struct template_all_of: std::true_type {};
template <template <class> class Pred, class First, class... Rest>
struct template_all_of<Pred, First, Rest...>: std::bool_constant<Pred<First>::value && template_all_of<Pred, Rest...>::value> {};
template <template <class> class Pred, class... Ts>
inline constexpr bool template_all_of_v = template_all_of<Pred, Ts...>::value;

// template_any_of, template_any_of_v
template <template <class> class Pred, class... Ts>
struct template_any_of: std::false_type {};
template <template <class> class Pred, class First, class... Rest>
struct template_any_of<Pred, First, Rest...>: std::bool_constant<Pred<First>::value || template_any_of<Pred, Rest...>::value> {};
template <template <class> class Pred, class... Ts>
inline constexpr bool template_any_of_v = template_any_of<Pred, Ts...>::value;

// template_none_of, template_none_of_v
template <template <class> class Pred, class... Ts>
struct template_none_of: std::bool_constant<!template_any_of<Pred, Ts...>::value> {};
template <template <class> class Pred, class... Ts>
inline constexpr bool template_none_of_v = template_none_of<Pred, Ts...>::value;

// overloaded
// c++14
template <class... Ts>
struct overloaded {
	constexpr overloaded() = default;
};
template <class T, class... Rest>
struct overloaded<T, Rest...>: T, overloaded<Rest...> {
	template <class U, class... RestU>
	constexpr overloaded(U&& f, RestU&&... fs): T(std::forward<U>(f)), overloaded<Rest...>(std::forward<RestU>(fs)...) {}

	using T::operator();
	using overloaded<Rest...>::operator();
};
template <class T>
struct overloaded<T>: T {
	template <class U>
	constexpr overloaded(U&& f): T(std::forward<U>(f)) {}

	using T::operator();
};

namespace details {

template <class T, std::size_t I>
struct overloaded_resolution_function { std::integral_constant<std::size_t, I> operator()(T); };
template <class Tup, class Seq>
struct overloaded_resolution_overloaded;
template <class... Ts, std::size_t... Is>
struct overloaded_resolution_overloaded<std::tuple<Ts...>, std::index_sequence<Is...>> {
	using type = overloaded<overloaded_resolution_function<Ts, Is>...>;
};
template <class Tup>
using make_overloaded_resolution_overloaded =
	typename overloaded_resolution_overloaded<Tup, std::make_index_sequence<std::tuple_size_v<Tup>>>::type;
template <class T, class Tup>
using overloaded_resolution_index = decltype(std::declval<make_overloaded_resolution_overloaded<Tup>>()(std::declval<T>()));

template <class T, class Tup, class = void>
struct overloaded_resolution_tuple;
template <class T, class Tup>
struct overloaded_resolution_tuple<T, Tup, std::void_t<overloaded_resolution_index<T, Tup>>> {
    using type = std::tuple_element_t<overloaded_resolution_index<T, Tup>::value, Tup>;
};

} // namespace details

// overloaded_resolution, overloaded_resolution_t
template <class T, class... Ts>
using overloaded_resolution = details::overloaded_resolution_tuple<T, std::tuple<Ts...>>;
template <class T, class... Ts>
using overloaded_resolution_t = typename overloaded_resolution<T, Ts...>::type;

// tuple traits
// ---------------------------------------------------------

// tuple_index, tuple_index_v
template <class T, class Tup>
struct tuple_index;
template <class T, class... Ts>
struct tuple_index<T, std::tuple<Ts...>>: index_of<T, Ts...> {};
template <class T, class Tup>
inline constexpr std::size_t tuple_index_v = tuple_index<T, Tup>::value;

// tuple_rindex, tuple_rindex_v
template <class T, class Tup>
struct tuple_rindex;
template <class T, class... Ts>
struct tuple_rindex<T, std::tuple<Ts...>>: rindex_of<T, Ts...> {};
template <class T, class Tup>
inline constexpr std::size_t tuple_rindex_v = tuple_rindex<T, Tup>::value;

// tuple_count, tuple_count_v
template <class T, class Tup>
struct tuple_count;
template <class T, class... Ts>
struct tuple_count<T, std::tuple<Ts...>>: count_of<T, Ts...> {};
template <class T, class Tup>
inline constexpr std::size_t tuple_count_v = tuple_count<T, Tup>::value;

// is_tuple_exactly_once, is_tuple_exactly_once_v
template <class T, class Tup>
struct is_tuple_exactly_once;
template <class T, class... Ts>
struct is_tuple_exactly_once<T, std::tuple<Ts...>>: is_exactly_once<T, Ts...> {};
template <class T, class... Ts>
inline constexpr bool is_tuple_exactly_once_v = is_tuple_exactly_once<T, Ts...>::value;

// tuple_concat, tuple_concat_t
template <class... Tups>
struct tuple_concat;
template <class... Ts>
struct tuple_concat<std::tuple<Ts...>> { using type = std::tuple<Ts...>; };
template <class... Ts, class... Ts2>
struct tuple_concat<std::tuple<Ts...>, std::tuple<Ts2...>> { using type = std::tuple<Ts..., Ts2...>; };
template <class First, class... Rest>
struct tuple_concat<First, Rest...> {
	using type = typename tuple_concat<First, typename tuple_concat<Rest...>::type>::type;
};
template <class... Tups>
using tuple_concat_t = typename tuple_concat<Tups...>::type;

// tuple_concat_to, tuple_concat_to_t
template <class Tup, class TupTup>
struct tuple_concat_to;
template <class Tup, class... Tups>
struct tuple_concat_to<Tup, std::tuple<Tups...>> { using type = std::tuple<tuple_concat_t<Tup, Tups>...>; };
template <class Tup, class TupTup>
using tuple_concat_to_t = typename tuple_concat_to<Tup, TupTup>::type;

// tuple_combination, tuple_combination_t
template <class... Tups>
struct tuple_combination;
template <class... Ts>
struct tuple_combination<std::tuple<Ts...>> { using type = std::tuple<std::tuple<Ts>...>; };
template <class... Ts, class... Rest>
struct tuple_combination<std::tuple<Ts...>, Rest...> {
	using type = tuple_concat_t<tuple_concat_to_t<std::tuple<Ts>, typename tuple_combination<Rest...>::type>...>;
};
template <class... Tups>
using tuple_combination_t = typename tuple_combination<Tups...>::type;

// tuple_integer_sequence, tuple_integer_sequence_t
template <class T, class Tup>
struct tuple_integer_sequence;
template <class T>
struct tuple_integer_sequence<T, std::tuple<>> { using type = std::integer_sequence<T>; };
template <class T, class... Ts>
struct tuple_integer_sequence<T, std::tuple<Ts...>> { using type = std::integer_sequence<T, Ts::value...>; };
template <class T, class Tup>
using tuple_integer_sequence_t = typename tuple_integer_sequence<T, Tup>::type;

// tuple_sub, tuple_sub_t
template <std::size_t Pos, std::size_t Cnt, class Tup, class = std::make_index_sequence<Cnt>>
struct tuple_sub;
template <std::size_t Pos, std::size_t Cnt, class Tup, std::size_t... Is>
struct tuple_sub<Pos, Cnt, Tup, std::index_sequence<Is...>> { using type = std::tuple<std::tuple_element_t<Pos + Is, Tup>...>; };
template <std::size_t Pos, std::size_t Cnt, class Tup>
using tuple_sub_t = typename tuple_sub<Pos, Cnt, Tup>::type;

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

// tuple_apply, tuple_apply_t
template <template <class...> class TT, class Tup>
struct tuple_apply;
template <template <class...> class TT, class... Ts>
struct tuple_apply<TT, std::tuple<Ts...>> {
	using type = TT<Ts...>;
};
template <template <class...> class TT, class Tup>
using tuple_apply_t = typename tuple_apply<TT, Tup>::type;

// integer_sequence traits
// ---------------------------------------------------------

// make_integer_sequence
// Workaround for the MSVC bug of Parameter pack expansion
namespace details {

template <class T, T N>
struct MakeIntegerSequence {
        using type = std::make_integer_sequence<T, N>;
};

} // namespace details

// make_integer_sequence
template <class T, T N>
using make_integer_sequence = typename details::MakeIntegerSequence<T, N>::type;

// make_index_sequence
template <std::size_t N>
using make_index_sequence = typename details::MakeIntegerSequence<std::size_t, N>::type;

// index_sequence_for
template <class... Ts>
using index_sequence_for = make_index_sequence<sizeof...(Ts)>;

// integer_sequence_tuple, integer_sequence_tuple_t
template <class IntSeq>
struct integer_sequence_tuple;
template <class T, T... Ints>
struct integer_sequence_tuple<std::integer_sequence<T, Ints...>> {
	using type = std::tuple<std::integral_constant<T, Ints>...>;
};
template <class IntSeq>
using integer_sequence_tuple_t = typename integer_sequence_tuple<IntSeq>::type;

// integer_sequence_concat, integer_sequence_concat_t
template <class... IntSeq>
struct integer_sequence_concat;
template <class T, T... Ints>
struct integer_sequence_concat<std::integer_sequence<T, Ints...>> { using type = std::integer_sequence<T, Ints...>; };
template <class T, T... Ints, T... Ints2>
struct integer_sequence_concat<std::integer_sequence<T, Ints...>, std::integer_sequence<T, Ints2...>> { using type = std::integer_sequence<T, Ints..., Ints2...>; };
template <class First, class... Rest>
struct integer_sequence_concat<First, Rest...> {
	using type = typename integer_sequence_concat<First, typename integer_sequence_concat<Rest...>::type>::type;
};
template <class... IntSeq>
using integer_sequence_concat_t = typename integer_sequence_concat<IntSeq...>::type;

// integer_sequence_concat_to, integer_sequence_concat_to_t
template <class IntSeq, class TupIntSeq>
struct integer_sequence_concat_to;
template <class IntSeq, class... IntSeqs>
struct integer_sequence_concat_to<IntSeq, std::tuple<IntSeqs...>> { using type = std::tuple<integer_sequence_concat_t<IntSeq, IntSeqs>...>; };
template <class IntSeq, class TupIntSeq>
using integer_sequence_concat_to_t = typename integer_sequence_concat_to<IntSeq, TupIntSeq>::type;

// integer_sequence_combination, integer_sequence_combination_t
template <class... IntSeq>
struct integer_sequence_combination;
template <class T, T... Ints>
struct integer_sequence_combination<std::integer_sequence<T, Ints...>> { using type = std::tuple<std::integer_sequence<T, Ints>...>; };
template <class T, T... Ints, class... Rest>
struct integer_sequence_combination<std::integer_sequence<T, Ints...>, Rest...> {
	using type = tuple_concat_t<integer_sequence_concat_to_t<std::integer_sequence<T, Ints>, typename integer_sequence_combination<Rest...>::type>...>;
};
template <class... IntSeq>
using integer_sequence_combination_t = typename integer_sequence_combination<IntSeq...>::type;

// make_integer_sequence_combination
template <class T, T... Ns>
using make_integer_sequence_combination = integer_sequence_combination_t<make_integer_sequence<T, Ns>...>;

// make_integer_sequence_combination
template <std::size_t... Ns>
using make_index_sequence_combination = make_integer_sequence_combination<std::size_t, Ns...>;

// index_sequence_combination_for
template <class... Tups>
using index_sequence_combination_for = make_index_sequence_combination<std::tuple_size_v<Tups>...>;

// Miscellaneous transformations
// ---------------------------------------------------------


// Function traits
// ---------------------------------------------------------

// is_non_overloaded_invocable, is_non_overloaded_invocable_v
// primitive function || has non-overloaded operator()
template <class T>
using is_non_overloaded_invocable = std::bool_constant<is_primitive_function_v<T> || has_non_overloaded_operator_parentheses_v<T>>;
template <class T>
inline constexpr bool is_non_overloaded_invocable_v = is_non_overloaded_invocable<T>::value;

template <class T, class Ret, class... Args>
struct function_of_base_ {
	using type = Ret(Args...);
	using noexcept_type = Ret(Args...) noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<T&, Ret, Args...> {
	using type = Ret(Args...) &;
	using noexcept_type = Ret(Args...) & noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<T&&, Ret, Args...> {
	using type = Ret(Args...) &&;
	using noexcept_type = Ret(Args...) && noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<const T, Ret, Args...> {
	using type = Ret(Args...) const;
	using noexcept_type = Ret(Args...) const noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<const T&, Ret, Args...> {
	using type = Ret(Args...) const&;
	using noexcept_type = Ret(Args...) const& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<const T&&, Ret, Args...> {
	using type = Ret(Args...) const&&;
	using noexcept_type = Ret(Args...) const&& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<volatile T, Ret, Args...> {
	using type = Ret(Args...) volatile;
	using noexcept_type = Ret(Args...) volatile noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<volatile T&, Ret, Args...> {
	using type = Ret(Args...) volatile&;
	using noexcept_type = Ret(Args...) volatile& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<volatile T&&, Ret, Args...> {
	using type = Ret(Args...) volatile&&;
	using noexcept_type = Ret(Args...) volatile&& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<const volatile T, Ret, Args...> {
	using type = Ret(Args...) const volatile;
	using noexcept_type = Ret(Args...) const volatile noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<const volatile T&, Ret, Args...> {
	using type = Ret(Args...) const volatile&;
	using noexcept_type = Ret(Args...) const volatile& noexcept;
};
template <class T, class Ret, class... Args>
struct function_of_base_<const volatile T&&, Ret, Args...> {
	using type = Ret(Args...) const volatile&&;
	using noexcept_type = Ret(Args...) const volatile&& noexcept;
};

// function_of, function_of_t
template <class Ret, class ArgsTuple, class Spec, bool IsNoexcept = false, bool = std::is_class_v<std::decay_t<Spec>>>
struct function_of;
template <class Ret, class... Args, class Spec>
struct function_of<Ret, std::tuple<Args...>, Spec, false, false> {
	using type = typename function_of_base_<Spec, Ret, Args...>::type;
};
template <class Ret, class... Args, class Spec>
struct function_of<Ret, std::tuple<Args...>, Spec, true, false> {
	using type = typename function_of_base_<Spec, Ret, Args...>::noexcept_type;
};
template <class Ret, class... Args, class Spec>
struct function_of<Ret, std::tuple<Args...>, Spec, false, true> {
	using type = typename function_of_base_<Spec, Ret, Args...>::type std::decay_t<Spec>::*;
};
template <class Ret, class... Args, class Spec>
struct function_of<Ret, std::tuple<Args...>, Spec, true, true> {
	using type = typename function_of_base_<Spec, Ret, Args...>::noexcept_type std::decay_t<Spec>::*;
};
template <class Ret, class ArgsTuple, class Spec, bool IsNoexcept = false>
using function_of_t = typename function_of<Ret, ArgsTuple, Spec, IsNoexcept>::type;

template <class T, class ClassT, class Spec, bool IsNoexcept, class Ret, class... Args>
struct function_traits_base_ {
	using type = T;
	using class_type = ClassT;
	using function_type = T;
	using result_type = Ret;
	using args_tuple = std::tuple<Args...>;
	using specifier = Spec;
	using is_noexcept = std::bool_constant<IsNoexcept>;
};
// function_traits
template <class T, class ClassT = void, class Spec = bool, class = T>
struct function_traits;
// not noexcept
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...)>: function_traits_base_<T, ClassT, Spec, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) &>: function_traits_base_<T, ClassT, Spec&, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) &&>: function_traits_base_<T, ClassT, Spec&&, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const>: function_traits_base_<T, ClassT, Spec const, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const&>: function_traits_base_<T, ClassT, Spec const&, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const&&>: function_traits_base_<T, ClassT, Spec const&&, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) volatile>: function_traits_base_<T, ClassT, Spec volatile, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) volatile &>: function_traits_base_<T, ClassT, Spec volatile&, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) volatile &&>: function_traits_base_<T, ClassT, Spec volatile&&, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const volatile>: function_traits_base_<T, ClassT, Spec const volatile, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const volatile&>: function_traits_base_<T, ClassT, Spec const volatile&, false, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const volatile&&>: function_traits_base_<T, ClassT, Spec const volatile&&, false, Ret, Args...> {};
// noexcept
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) noexcept>: function_traits_base_<T, ClassT, Spec, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) & noexcept>: function_traits_base_<T, ClassT, Spec&, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) && noexcept>: function_traits_base_<T, ClassT, Spec&&, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const noexcept>: function_traits_base_<T, ClassT, Spec const, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const& noexcept>: function_traits_base_<T, ClassT, Spec const&, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const&& noexcept>: function_traits_base_<T, ClassT, Spec const&&, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) volatile noexcept>: function_traits_base_<T, ClassT, Spec volatile, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) volatile & noexcept>: function_traits_base_<T, ClassT, Spec volatile&, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) volatile && noexcept>: function_traits_base_<T, ClassT, Spec volatile&&, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const volatile noexcept>: function_traits_base_<T, ClassT, Spec const volatile, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const volatile& noexcept>: function_traits_base_<T, ClassT, Spec const volatile&, true, Ret, Args...> {};
template <class T, class ClassT, class Spec, class Ret, class... Args>
struct function_traits<T, ClassT, Spec, Ret(Args...) const volatile&& noexcept>: function_traits_base_<T, ClassT, Spec const volatile&&, true, Ret, Args...> {};
// member function
template <class T, class ClassT, class Spec, class CT, class F>
struct function_traits<T, ClassT, Spec, F CT::*>: function_traits<T, CT, CT, F> {
	using function_type = F;
};

// function_specifier, function_specifier_t
template <class T, class = void>
struct function_specifier;
template <class T>
struct function_specifier<T, std::void_t<function_traits<T>>> {
	using type = typename function_traits<T>::specifier;
};
template <class T>
using function_specifier_t = typename function_specifier<T>::type;

// mf_class, mf_class_t
template <class T, bool = std::is_member_function_pointer_v<T>>
struct mf_class;
template <class T>
struct mf_class<T, true> {
	using type = typename function_traits<T>::class_type;
};
template <class T>
using mf_class_t = typename mf_class<T>::type;

// is_noexcept, is_noexcept_v
template <class T, class = void>
struct is_noexcept: std::false_type {};
template <class T>
struct is_noexcept<T, std::void_t<function_traits<T>>>: function_traits<T>::is_noexcept {};
template <class T>
inline constexpr bool is_noexcept_v = is_noexcept<T>::value;

// remove_noexcept, remove_noexcept_t
template <class T, class = void>
struct remove_noexcept {
	using type = T;
};
template <class T>
struct remove_noexcept<T, std::void_t<function_traits<T>>> {
private:
	using Traits = function_traits<T>;
public:
	using type = function_of_t<typename Traits::result_type, typename Traits::args_tuple, typename Traits::specifier>;
};
template <class T>
using remove_noexcept_t = typename remove_noexcept<T>::type;

// add_noexcept, add_noexcept_t
template <class T, class = void>
struct add_noexcept {
	using type = T;
};
template <class T>
struct add_noexcept<T, std::void_t<function_traits<T>>> {
private:
	using Traits = function_traits<T>;
public:
	using type = function_of_t<typename Traits::result_type, typename Traits::args_tuple, typename Traits::specifier, true>;
};
template <class T>
using add_noexcept_t = typename add_noexcept<T>::type;

} // namespace esl

#endif // ESL_TYPE_TRAITS_HPP

