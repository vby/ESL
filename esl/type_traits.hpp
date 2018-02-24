
#ifndef ESL_TYPE_TRAITS_HPP
#define ESL_TYPE_TRAITS_HPP

#include "macros.hpp"

#include <type_traits>
#include <iterator>
#include <tuple>

namespace esl {

// ESL_TRAITS_WELL_FORMED
#define ESL_TRAITS_WELL_FORMED(name, type_expr) \
	template <class T, class = std::void_t<>> \
	struct name: std::false_type {}; \
	template <class T> \
	struct name<T, std::void_t<type_expr>>: std::true_type {}; \
	template <class T> \
	inline constexpr bool name##_v = name<T>::value;

// ESL_TRAITS_HAS_MEMBER_VARIABLE
#define ESL_TRAITS_HAS_MEMBER_VARIABLE(name, member) ESL_TRAITS_WELL_FORMED(name, decltype(&T::member))

// ESL_TRAITS_HAS_NON_OVERLOADED_MEMBER_FUNCTION
#define ESL_TRAITS_HAS_NON_OVERLOADED_MEMBER_FUNCTION(name, member) ESL_TRAITS_WELL_FORMED(name, decltype(&T::member))

#define ESL_TRAITS_HAS_NON_OVERLOADED_MEMBER_FUNCTION_ESL_(name, member) \
	ESL_TRAITS_HAS_NON_OVERLOADED_MEMBER_FUNCTION(has_non_overloaded_##name, member)

struct non_fallback_type {};
template <class D>
struct false_fallback_base: std::false_type { using type = D; };
template <>
struct false_fallback_base<non_fallback_type> { using value_type = bool; static constexpr bool value = false; };

// ESL_TRAITS_MEMBER_TYPE
#define ESL_TRAITS_MEMBER_TYPE(name, member) \
	template <class T, class D = ::esl::non_fallback_type, class = std::void_t<>> \
	struct name: ::esl::false_fallback_base<D> {}; \
	template <class T, class D> \
	struct name<T, D, std::void_t<typename T::member>>: std::true_type { using type = typename T::member; }; \
	template <class T> \
	inline constexpr bool name##_v = name<T>::value; \
	template <class T, class D = ::esl::non_fallback_type> \
	using name##_t = typename name<T, D>::type;

#define ESL_TRAITS_MEMBER_TYPE_ESL_(name)  ESL_TRAITS_MEMBER_TYPE(member_type_##name, name)

// is_datable, is_datable_v
ESL_TRAITS_WELL_FORMED(is_datable, decltype(std::data(std::declval<T>())))

// is_sizeable, is_sizeable_v
ESL_TRAITS_WELL_FORMED(is_sizeable, decltype(std::size(std::declval<T>())))

// is_emptiable, is_emptiable_v
ESL_TRAITS_WELL_FORMED(is_emptiable, decltype(std::empty(std::declval<T>())))

// is_static_castable, is_static_castable_v
template <class From, class To>
struct is_static_castable {
private:
	template <class T>
	static void test(std::add_pointer_t<decltype(static_cast<To>(std::declval<From>()))>);
	template <class T>
	static void* test(...);
public:
	using type = typename std::is_void<decltype(test<void>(nullptr))>::type;
	using value_type = bool;
	static constexpr bool value = type::value;
};
template <class From, class To>
inline constexpr bool is_static_castable_v = is_static_castable<From, To>::value;

// has_non_overloaded_operator_parentheses, has_non_overloaded_operator_parentheses_v
ESL_TRAITS_HAS_NON_OVERLOADED_MEMBER_FUNCTION_ESL_(operator_parentheses, operator())

// has_non_overloaded_operator_brackets, has_non_overloaded_operator_brackets_v
ESL_TRAITS_HAS_NON_OVERLOADED_MEMBER_FUNCTION_ESL_(operator_brackets, operator[])

// member_type_##name, member_type_##name_t, member_type_##name_v
ESL_TRAITS_MEMBER_TYPE_ESL_(type)
ESL_TRAITS_MEMBER_TYPE_ESL_(value_type)
ESL_TRAITS_MEMBER_TYPE_ESL_(size_type)
ESL_TRAITS_MEMBER_TYPE_ESL_(difference_type)
ESL_TRAITS_MEMBER_TYPE_ESL_(allocator_type)
ESL_TRAITS_MEMBER_TYPE_ESL_(pointer)
ESL_TRAITS_MEMBER_TYPE_ESL_(const_pointer)
ESL_TRAITS_MEMBER_TYPE_ESL_(reference)
ESL_TRAITS_MEMBER_TYPE_ESL_(const_reference)
ESL_TRAITS_MEMBER_TYPE_ESL_(iterator)
ESL_TRAITS_MEMBER_TYPE_ESL_(const_iterator)
ESL_TRAITS_MEMBER_TYPE_ESL_(reverse_iterator)
ESL_TRAITS_MEMBER_TYPE_ESL_(const_reverse_iterator)
ESL_TRAITS_MEMBER_TYPE_ESL_(traits_type)
ESL_TRAITS_MEMBER_TYPE_ESL_(element_type)
ESL_TRAITS_MEMBER_TYPE_ESL_(deleter_type)
ESL_TRAITS_MEMBER_TYPE_ESL_(is_always_equal)
ESL_TRAITS_MEMBER_TYPE_ESL_(weak_type)
ESL_TRAITS_MEMBER_TYPE_ESL_(native_handle_type)
ESL_TRAITS_MEMBER_TYPE_ESL_(mutex_type)

// add_const_lvalue_reference, add_const_lvalue_reference_t
template <class T>
using add_const_lvalue_reference = std::add_lvalue_reference<std::add_const_t<T>>;;
template <class T>
using add_const_lvalue_reference_t = typename add_const_lvalue_reference<T>::type;

// remove_const_cv_reference
template <class T>
using remove_const_cv = std::remove_const<std::remove_cv_t<T>>;
template <class T>
using remove_const_cv_t = typename remove_const_cv<T>::type;

// remove_cv_reference, remove_cv_reference_t
template <class T>
using remove_cv_reference = std::remove_cv<std::remove_reference_t<T>>;
template <class T>
using remove_cv_reference_t = typename remove_cv_reference<T>::type;

// remove_const_cv_reference
template <class T>
using remove_const_cv_reference = std::remove_const<remove_cv_reference<T>>;
template <class T>
using remove_const_cv_reference_t = typename remove_const_cv_reference<T>::type;

// to_lvalue_reference, to_lvalue_reference_t
template <class T>
using to_lvalue_reference = std::add_lvalue_reference<remove_cv_reference_t<T>>;
template <class T>
using to_lvalue_reference_t = typename to_lvalue_reference<T>::type;

// to_rvalue_reference, to_rvalue_reference_t
template <class T>
using to_rvalue_reference = std::add_rvalue_reference<remove_cv_reference_t<T>>;
template <class T>
using to_rvalue_reference_t = typename to_rvalue_reference<T>::type;

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
	using class_type = remove_cv_reference_t<T>;
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

