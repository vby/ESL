#ifndef ESL_ARRAY_HPP
#define ESL_ARRAY_HPP

#include "type_traits.hpp"

#include <array>
#include <iterator>
#include <utility>

namespace esl {

// md_array, md_array_t
template <class T, std::size_t... Dimensions>
struct md_array {
    using type = T;
};
template <class T, std::size_t First, std::size_t... Rest>
struct md_array<T, First, Rest...> {
    using type = std::array<typename md_array<T, Rest...>::type, First>;
};
template <class T, std::size_t... Dimensions>
using md_array_t = typename md_array<T, Dimensions...>::type;

// md_carray, md_carray_t
template <class T, std::size_t... Dimensions>
struct md_carray {
    using type = T;
};
template <class T, std::size_t First, std::size_t... Rest>
struct md_carray<T, First, Rest...> {
    using type = typename md_carray<T, Rest...>::type[First];
};
template <class T, std::size_t... Dimensions>
using md_carray_t = typename md_carray<T, Dimensions...>::type;

template <class T, std::size_t N, class U, std::size_t... Is>
inline constexpr std::array<T, N> make_sized_array_internal_(U&& first, std::index_sequence<Is...>) {
    return std::array<T, N>{{first[Is]...}};
}
// make_sized_array
template <std::size_t N, class It, class T = remove_rcv_t<decltype(*std::declval<It>())>>
inline constexpr std::array<T, N> make_sized_array(It&& first) {
    return make_sized_array_internal_<T, N>(std::forward<It>(first), std::make_index_sequence<N>{});
}
// make_array
template <class T, std::size_t N>
inline constexpr std::array<std::remove_cv_t<T>, N> make_array(const T (&arr)[N]) {
    return make_sized_array<N>(arr);
}
template <class T, std::size_t N>
inline constexpr std::array<std::remove_cv_t<T>, N> make_array(T(&&arr)[N]) {
    return make_sized_array<N>(std::make_move_iterator(arr));
}
// sub_array
template <std::size_t Pos, std::size_t Cnt = npos, class T, std::size_t M, class N = sub_size<M, Pos, Cnt>>
inline constexpr std::array<std::remove_cv_t<T>, N::value> sub_array(const std::array<T, M>& arr) {
    return make_sized_array<N::value>(arr.begin() + Pos);
}
template <std::size_t Pos, std::size_t Cnt = npos, class T, std::size_t M, class N = sub_size<M, Pos, Cnt>>
inline constexpr std::array<std::remove_cv_t<T>, N::value> sub_array(std::array<T, M>&& arr) {
    return make_sized_array<N::value>(std::make_move_iterator(arr.begin() + Pos));
}
template <std::size_t Pos, std::size_t Cnt = npos, class T, std::size_t M, class N = sub_size<M, Pos, Cnt>>
inline constexpr std::array<std::remove_cv_t<T>, N::value> sub_array(T (&arr)[M]) {
    return make_sized_array<N::value>(arr + Pos);
}
template <std::size_t Pos, std::size_t Cnt = npos, class T, std::size_t M, class N = sub_size<M, Pos, Cnt>>
inline constexpr std::array<std::remove_cv_t<T>, N::value> sub_array(T(&&arr)[M]) {
    return make_sized_array<N::value>(std::make_move_iterator(arr + Pos));
}

// array_size, array_size_v
template <class T>
struct array_size_internal_;
template <class T, std::size_t N>
struct array_size_internal_<T[N]> : std::integral_constant<std::size_t, N> {};
template <class T, std::size_t N>
struct array_size_internal_<std::array<T, N>> : std::integral_constant<std::size_t, N> {};
template <class T>
struct array_size : array_size_internal_<std::remove_cv_t<T>> {};
template <class T>
inline constexpr std::size_t array_size_v = array_size<T>::value;

// array_rank, array_rank_v
template <class T>
struct array_rank;
template <class T>
struct array_rank_internal_ : std::integral_constant<std::size_t, 0> {};
template <class T>
struct array_rank_internal_<T[]> : std::integral_constant<std::size_t, array_rank<T>::value + 1> {};
template <class T, std::size_t N>
struct array_rank_internal_<T[N]> : std::integral_constant<std::size_t, array_rank<T>::value + 1> {};
template <class T, std::size_t N>
struct array_rank_internal_<std::array<T, N>> : std::integral_constant<std::size_t, array_rank<T>::value + 1> {};
template <class T>
struct array_rank : array_rank_internal_<std::remove_cv_t<T>> {};
template <class T>
inline constexpr std::size_t array_rank_v = array_rank<T>::value;

// array_element, array_element_t
template <std::size_t Rank, class T>
struct array_element;
template <std::size_t Rank, class T, class = void>
struct array_element_internal_ {
    using type = T;
};
template <std::size_t Rank, class T, std::size_t N>
struct array_element_internal_<Rank, std::array<T, N>, std::enable_if_t<Rank != 0>> : array_element<Rank - 1, typename std::array<T, N>::value_type> {};
template <std::size_t Rank, class T>
struct array_element_internal_<Rank, T[], std::enable_if_t<Rank != 0>> : array_element<Rank - 1, T> {};
template <std::size_t Rank, class T, std::size_t N>
struct array_element_internal_<Rank, T[N], std::enable_if_t<Rank != 0>> : array_element<Rank - 1, T> {};
template <std::size_t Rank, class T>
struct array_element : array_element_internal_<Rank, std::remove_cv_t<T>> {};
template <std::size_t Rank, class T>
using array_element_t = typename array_element<Rank, T>::type;

// array_element_size, array_element_size_v
template <std::size_t Rank, class T>
struct array_element_size : array_size<array_element_t<Rank, T>> {};
template <std::size_t Rank, class T>
inline constexpr std::size_t array_element_size_v = array_element_size<Rank, T>::value;

namespace details {

template <class T>
inline constexpr decltype(auto) array_access(T& a) noexcept {
    return a;
}
template <class T, std::size_t N, class... Rest>
inline constexpr decltype(auto) array_access(std::array<T, N>& a, std::size_t first_pos, Rest... rest_pos) noexcept {
    return array_access(a[first_pos], rest_pos...);
}
template <class T, std::size_t N, class... Rest>
inline constexpr decltype(auto) array_access(const std::array<T, N>& a, std::size_t first_pos, Rest... rest_pos) noexcept {
    return array_access(a[first_pos], rest_pos...);
}

template <class T>
inline constexpr decltype(auto) array_at(T& a) noexcept {
    return a;
}
template <class T, std::size_t N, class... Rest>
inline constexpr decltype(auto) array_at(std::array<T, N>& a, std::size_t first_pos, Rest... rest_pos) noexcept {
    return array_at(a.at(first_pos), rest_pos...);
}
template <class T, std::size_t N, class... Rest>
inline constexpr decltype(auto) array_at(const std::array<T, N>& a, std::size_t first_pos, Rest... rest_pos) noexcept {
    return array_at(a.at(first_pos), rest_pos...);
}

// array_get
template <std::size_t... Is>
struct array_get {
    template <class T>
    static constexpr T& get(T& a) noexcept {
        return a;
    }
};
template <std::size_t First, std::size_t... Rest>
struct array_get<First, Rest...> {
    template <class T, std::size_t N>
    static constexpr decltype(auto) get(std::array<T, N>& a) noexcept {
        return array_get<Rest...>::template get(std::get<First>(a));
    }
    template <class T, std::size_t N>
    static constexpr decltype(auto) get(const std::array<T, N>& a) noexcept {
        return array_get<Rest...>::template get(std::get<First>(a));
    }
};

} // namespace details

// access
template <class T, std::size_t N, class... Poss>
inline constexpr array_element_t<sizeof...(Poss), std::array<T, N>>& access(std::array<T, N>& a, Poss... poss) noexcept {
    return details::array_access(a, poss...);
}
template <class T, std::size_t N, class... Poss>
inline constexpr const array_element_t<sizeof...(Poss), std::array<T, N>>& access(const std::array<T, N>& a, Poss... poss) noexcept {
    return details::array_access(a, poss...);
}
template <class T, std::size_t N, class... Poss>
inline constexpr array_element_t<sizeof...(Poss), std::array<T, N>>&& access(std::array<T, N>&& a, Poss... poss) noexcept {
    return std::move(access(a, poss...));
}
template <class T, std::size_t N, class... Poss>
inline constexpr const array_element_t<sizeof...(Poss), std::array<T, N>>&& access(const std::array<T, N>&& a, Poss... poss) noexcept {
    return std::move(access(a, poss...));
}

// at
template <class T, std::size_t N, class... Poss>
inline constexpr array_element_t<sizeof...(Poss), std::array<T, N>>& at(std::array<T, N>& a, Poss... poss) noexcept {
    return details::array_at(a, poss...);
}
template <class T, std::size_t N, class... Poss>
inline constexpr const array_element_t<sizeof...(Poss), std::array<T, N>>& at(const std::array<T, N>& a, Poss... poss) noexcept {
    return details::array_at(a, poss...);
}
template <class T, std::size_t N, class... Poss>
inline constexpr array_element_t<sizeof...(Poss), std::array<T, N>>&& at(std::array<T, N>&& a, Poss... poss) noexcept {
    return std::move(at(a, poss...));
}
template <class T, std::size_t N, class... Poss>
inline constexpr const array_element_t<sizeof...(Poss), std::array<T, N>>&& at(const std::array<T, N>&& a, Poss... poss) noexcept {
    return std::move(at(a, poss...));
}

} // namespace esl

namespace std {

// get
template <size_t... Is, class T, std::size_t N, class = std::enable_if_t<sizeof...(Is) != 1>>
inline constexpr ::esl::array_element_t<sizeof...(Is), std::array<T, N>>& get(std::array<T, N>& a) noexcept {
    return ::esl::details::array_get<Is...>::get(a);
}
template <size_t... Is, class T, std::size_t N, class = std::enable_if_t<sizeof...(Is) != 1>>
inline constexpr const ::esl::array_element_t<sizeof...(Is), std::array<T, N>>& get(const std::array<T, N>& a) noexcept {
    return ::esl::details::array_get<Is...>::get(a);
}
template <size_t... Is, class T, std::size_t N, class = std::enable_if_t<sizeof...(Is) != 1>>
inline constexpr ::esl::array_element_t<sizeof...(Is), std::array<T, N>>&& get(std::array<T, N>&& a) noexcept {
    return move(::esl::details::array_get<Is...>::get(a));
}
template <size_t... Is, class T, std::size_t N, class = std::enable_if_t<sizeof...(Is) != 1>>
inline constexpr const ::esl::array_element_t<sizeof...(Is), std::array<T, N>>&& get(const std::array<T, N>&& a) noexcept {
    return move(::esl::details::array_get<Is...>::get(a));
}

} // namespace std

namespace esl {

using std::get;

} // namespace esl

#endif // ESL_ARRAY_HPP
