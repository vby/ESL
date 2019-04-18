
#ifndef ESL_SPAN_HPP
#define ESL_SPAN_HPP

#include "exception.hpp"
#include "macros.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace esl {

namespace details {

static constexpr std::size_t span_npos = std::string::npos;

// span_storage
template <class pointer, std::size_t N>
class span_storage {
public:
    constexpr span_storage(pointer data = nullptr) noexcept : data_(data) {}

    constexpr pointer data() const noexcept {
        return data_;
    }
    constexpr std::size_t size() const noexcept {
        return N;
    }

private:
    pointer data_;
};
template <class pointer>
class span_storage<pointer, span_npos> {
public:
    constexpr span_storage() noexcept : span_storage(nullptr, 0) {}
    constexpr span_storage(pointer data, std::size_t size) noexcept : data_(data), size_(size) {}

    constexpr pointer data() const noexcept {
        return data_;
    }
    constexpr std::size_t size() const noexcept {
        return size_;
    }

private:
    pointer data_;
    std::size_t size_;
};

// span_sub_size
template <std::size_t N, std::size_t Pos, std::size_t Cnt, bool = Pos <= N>
class span_subspan_size;
template <std::size_t Pos, std::size_t Cnt>
class span_subspan_size<span_npos, Pos, Cnt, true> : public std::integral_constant<std::size_t, span_npos> {};
template <std::size_t N, std::size_t Pos, std::size_t Cnt>
class span_subspan_size<N, Pos, Cnt, true> : public std::integral_constant<std::size_t, std::min(N - Pos, Cnt)> {};

} // namespace details

using details::span_npos;

template <class T, std::size_t N = span_npos>
class span {
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = std::add_const_t<T>&;
    using pointer = T*;
    using const_pointer = std::add_const_t<T>*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    static constexpr size_type npos = span_npos;

public:
    template <bool Dep = true, class = std::enable_if_t<Dep && (N == 0 || N == npos)>>
    constexpr span() noexcept : storage_() {}

    // Dynamic
    constexpr span(pointer data, size_type size) noexcept : storage_(data, size) {}

    template <class T1, size_type M>
    constexpr span(const span<T1, M>& s) noexcept : storage_(s.data(), s.size()) {}

    // Fixed
    constexpr span(pointer data) noexcept : storage_(data) {}

    template <class T1, class = std::enable_if_t<!std::is_same<T, T1>::value && N != npos>>
    constexpr span(const span<T1, N>& s) noexcept : storage_(s.data()) {}

    constexpr span(const span&) noexcept = default;

    ~span() noexcept = default;

    constexpr span& operator=(const span&) noexcept = default;

    // Element access

    // Exceptions: std::out_of_range if N == npos && pos >= size()
    template <size_type Pos>
    constexpr reference at() const noexcept(N != npos);

    // Exceptions: std::out_of_range if pos >= size()
    constexpr reference at(size_type pos) const {
        this->at_with_location(pos, "esl::span::at");
    }

    constexpr reference operator[](size_type pos) const noexcept {
        return data()[pos];
    }

    constexpr reference front() const noexcept {
        return *begin();
    }

    constexpr reference back() const noexcept {
        return *rbegin();
    }

    constexpr pointer data() const noexcept {
        return storage_.data();
    }

    // Exceptions: std::out_of_range if N == npos && pos > size()
    template <size_type Pos, size_type Cnt = npos, size_type Size = details::span_subspan_size<N, Pos, Cnt>::value>
    constexpr span<T, Size> subspan() const noexcept(N != npos);

    // Exceptions: std::out_of_range if pos > size()
    constexpr span<T> subspan(size_type pos, size_type count = span_npos) const {
        return this->subspan_with_location(pos, count, "esl::span::subspan");
    }

    // Capacity

    constexpr size_type size() const noexcept {
        return storage_.size();
    }

    constexpr size_type max_size() const noexcept {
        return std::numeric_limits<std::size_t>::max() / sizeof(value_type);
    }

    constexpr bool empty() const noexcept {
        return size() == 0;
    }

    // Modifers

    constexpr void swap(span& s) noexcept {
        std::swap(*this, s);
    }

    // Iterators
    constexpr iterator begin() const noexcept {
        return data();
    }

    constexpr iterator end() const noexcept {
        return data() + size();
    }

    constexpr const_iterator cbegin() const noexcept {
        return data();
    }

    constexpr const_iterator cend() const noexcept {
        return data() + size();
    }

    constexpr reverse_iterator rbegin() const noexcept {
        return end();
    }

    constexpr reverse_iterator rend() const noexcept {
        return begin();
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return cend();
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return cbegin();
    }

protected:
    constexpr reference at_with_location(size_type pos, const char* location) const {
        ESL_TRHOW_OUT_OF_RANGE_IF(pos, >=, this->size(), location);
        return data()[pos];
    }

    constexpr span<T> subspan_with_location(size_type pos, size_type count, const char* location) const {
        ESL_TRHOW_OUT_OF_RANGE_IF(pos, >, this->size(), location);
        size_type rcount = size() - pos;
        return span<T>{data() + pos, std::min(rcount, count)};
    }

private:
    details::span_storage<pointer, N> storage_;
};

namespace details {

template <class T, std::size_t N>
struct span_helper {
    using Span = span<T, N>;

    template <std::size_t Pos, class = std::enable_if_t<(Pos < N)>>
    static constexpr typename Span::reference at(const Span& sp) noexcept {
        return sp[Pos];
    }

    template <std::size_t Pos, std::size_t Cnt, std::size_t Size>
    static constexpr span<T, Size> subspan(const Span& sp) noexcept {
        return span<T, Size>(&sp[Pos]);
    }
};

template <class T>
struct span_helper<T, span_npos> {
    using Span = span<T>;

    template <std::size_t Pos>
    static constexpr typename Span::reference at(const Span& sp) {
        return sp.at(Pos);
    }

    template <std::size_t Pos, std::size_t Cnt, std::size_t Size>
    static constexpr span<T, Size> subspan(const Span& sp) {
        return sp.subspan(Pos, Cnt);
    }
};

} // namespace details

// span::at
template <class T, std::size_t N>
template <std::size_t Pos>
inline constexpr auto span<T, N>::at() const noexcept(N != npos) -> reference {
    return details::span_helper<T, N>::template at<Pos>(*this);
}

// span::subspan
template <class T, std::size_t N>
template <std::size_t Pos, std::size_t Cnt, std::size_t Size>
inline constexpr span<T, Size> span<T, N>::subspan() const noexcept(N != npos) {
    return details::span_helper<T, N>::template subspan<Pos, Cnt, Size>(*this);
}

// Comparison
template <class T, std::size_t N, std::size_t M>
inline constexpr bool operator==(const span<T, N>& lhs, const span<T, M>& rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <class T, std::size_t N, std::size_t M>
inline constexpr bool operator!=(const span<T, N>& lhs, const span<T, M>& rhs) {
    return !(lhs == rhs);
}

// Traits
template <class T>
class is_span : public std::false_type {};
template <class T, std::size_t N>
class is_span<span<T, N>> : public std::true_type {};

template <class T>
class is_fixed_span : std::false_type {};
template <class T, std::size_t N>
class is_fixed_span<span<T, N>> : public std::integral_constant<bool, N == span_npos> {};

// make_span
template <class T, std::size_t N>
inline constexpr span<T, N> make_span(T (&arr)[N]) noexcept {
    return span<T, N>{arr};
}
template <class T, std::size_t N>
inline constexpr span<T, N> make_span(std::array<T, N>& arr) noexcept {
    return span<T, N>{arr.data()};
}
template <class T, std::size_t N>
inline constexpr span<const T, N> make_span(const std::array<T, N>& arr) noexcept {
    return span<const T, N>{arr.data()};
}
template <class T>
inline constexpr span<T> make_span(T* data, std::size_t count) noexcept {
    return span<T>{data, count};
}
template <class T>
inline constexpr span<T> make_span(T* first, T* last) noexcept {
    return span<T>{first, last - first};
}
template <class T>
inline constexpr span<T> make_span(std::vector<T>& vec) noexcept {
    return span<T>{vec.data(), vec.size()};
}
template <class T>
inline constexpr span<const T> make_span(const std::vector<T>& vec) noexcept {
    return span<const T>{vec.data(), vec.size()};
}

} // namespace esl

// std::tuple_size<fixed-span>, std::get<I>(fixed-span)
namespace std {

ESL_WARNING_PUSH()

ESL_WARNING_CLANG(disable, "-Wmismatched-tags")

// std::tuple_size
template <class T, size_t N>
class tuple_size<esl::span<T, N>> : public integral_constant<size_t, N> {};
template <class T>
class tuple_size<esl::span<T, esl::span_npos>> {};

ESL_WARNING_POP()

// std::get
template <size_t I, class T, size_t N, class = std::enable_if<N != esl::span_npos>>
inline constexpr typename esl::span<T, N>::reference get(const esl::span<T, N>& span) noexcept {
    return span.template at<I>();
}

} // namespace std

#endif //ESL_SPAN_HPP
