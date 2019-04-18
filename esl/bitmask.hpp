#ifndef ESL_BITMASK_HPP
#define ESL_BITMASK_HPP

#include <type_traits>

namespace esl {

template <class T>
struct is_bitmask_enum : std::false_type {};

template <class T>
inline constexpr bool is_bitmask_enum_v = is_bitmask_enum<T>::value;

// bitmask_enum

class bitmask_enum_base {};
template <class T>
class bitmask_enum : public bitmask_enum_base {
public:
    using value_type = T;
    using enum_type = std::remove_reference_t<T>;
    using enum_underlying_type = std::underlying_type_t<enum_type>;

private:
    T e_;

public:
    constexpr bitmask_enum(T e) noexcept : e_(e) {}

    constexpr operator value_type() noexcept {
        return e_;
    }

    constexpr operator bool() noexcept {
        return static_cast<enum_underlying_type>(e_);
    }
};

namespace details {

// bitmask_enum_traits
template <class T, class Enable = void>
struct bitmask_enum_traits : std::false_type {
    using enum_type = void;
    using underlying_type = void;
    using bitmask_enum_type = void;
};
template <class T>
struct bitmask_enum_traits<T, std::enable_if_t<is_bitmask_enum_v<T>>> : std::true_type {
    using enum_type = T;
    using underlying_type = std::underlying_type_t<enum_type>;
    using bitmask_enum_type = bitmask_enum<enum_type>;
};
template <class T>
struct bitmask_enum_traits<T, std::enable_if_t<std::is_base_of_v<bitmask_enum_base, T>>> : std::true_type {
    using enum_type = typename T::enum_type;
    using underlying_type = typename T::enum_underlying_type;
    using bitmask_enum_type = bitmask_enum<enum_type>;
};

template <class T>
inline constexpr bool is_bitmask_enum_v = bitmask_enum_traits<T>::value;

template <class T, class U>
inline constexpr bool is_same_bitmask_enum_v =
    is_bitmask_enum_v<T>&& std::is_same_v<typename bitmask_enum_traits<T>::enum_type, typename bitmask_enum_traits<U>::enum_type>;

template <class T>
constexpr decltype(auto) bitmask_enum_to_underlying(T v) noexcept {
    return static_cast<typename bitmask_enum_traits<T>::underlying_type>(v);
}
template <class T, class U>
constexpr decltype(auto) bitmask_enum_to_enum(U v) noexcept {
    return static_cast<typename bitmask_enum_traits<T>::enum_type>(v);
}
template <class T, class U>
constexpr typename bitmask_enum_traits<T>::bitmask_enum_type bitmask_enum_to_bitmask_enum(U v) noexcept {
    return bitmask_enum_to_enum<T>(v);
}

} // namespace details

} // namespace esl

/// & | ^ ~
template <class T, class U, class = std::enable_if_t<esl::details::is_same_bitmask_enum_v<T, U>>>
inline constexpr decltype(auto) operator&(T lhs, U rhs) {
    auto r = esl::details::bitmask_enum_to_underlying(lhs) & esl::details::bitmask_enum_to_underlying(rhs);
    return esl::details::bitmask_enum_to_bitmask_enum<T>(r);
}
template <class T, class U, class = std::enable_if_t<esl::details::is_same_bitmask_enum_v<T, U>>>
inline constexpr decltype(auto) operator|(T lhs, U rhs) {
    auto r = esl::details::bitmask_enum_to_underlying(lhs) | esl::details::bitmask_enum_to_underlying(rhs);
    return esl::details::bitmask_enum_to_bitmask_enum<T>(r);
}
template <class T, class U, class = std::enable_if_t<esl::details::is_same_bitmask_enum_v<T, U>>>
inline constexpr decltype(auto) operator^(T lhs, U rhs) {
    auto r = esl::details::bitmask_enum_to_underlying(lhs) ^ esl::details::bitmask_enum_to_underlying(rhs);
    return esl::details::bitmask_enum_to_bitmask_enum<T>(r);
}
template <class T, class = std::enable_if_t<esl::details::is_bitmask_enum_v<T>>>
inline constexpr decltype(auto) operator~(T v) {
    auto r = ~esl::details::bitmask_enum_to_underlying(v);
    return esl::details::bitmask_enum_to_bitmask_enum<T>(r);
}
/// &= |= ^=
template <class T, class U, class = std::enable_if_t<esl::is_bitmask_enum_v<T> && esl::details::is_bitmask_enum_v<U>>>
inline constexpr esl::bitmask_enum<T&> operator&=(T& lhs, U rhs) {
    return lhs = lhs & rhs;
}
template <class T, class U, class = std::enable_if_t<esl::is_bitmask_enum_v<T> && esl::details::is_bitmask_enum_v<U>>>
inline constexpr esl::bitmask_enum<T&> operator&=(esl::bitmask_enum<T&> lhs, U rhs) {
    lhs = lhs & rhs;
    return lhs;
}
template <class T, class U, class = std::enable_if_t<esl::is_bitmask_enum_v<T> && esl::details::is_bitmask_enum_v<U>>>
inline constexpr esl::bitmask_enum<T&> operator|=(T& lhs, U rhs) {
    return lhs = lhs | rhs;
}
template <class T, class U, class = std::enable_if_t<esl::is_bitmask_enum_v<T> && esl::details::is_bitmask_enum_v<U>>>
inline constexpr esl::bitmask_enum<T&> operator|=(esl::bitmask_enum<T&> lhs, U rhs) {
    lhs = lhs | rhs;
    return lhs;
}
template <class T, class U, class = std::enable_if_t<esl::is_bitmask_enum_v<T> && esl::details::is_bitmask_enum_v<U>>>
inline constexpr esl::bitmask_enum<T&> operator^=(T& lhs, U rhs) {
    return lhs = lhs ^ rhs;
}
template <class T, class U, class = std::enable_if_t<esl::is_bitmask_enum_v<T> && esl::details::is_bitmask_enum_v<U>>>
inline constexpr esl::bitmask_enum<T&> operator^=(esl::bitmask_enum<T&> lhs, U rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

#endif // ESL_BITMASK_HPP
