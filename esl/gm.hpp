#include "macros.hpp"
#include <array>
#include <ostream>
#include <type_traits>
#include <utility>

namespace esl {
namespace gm {

/// VecT
template <class T, std::size_t N>
struct VecT;

using Vec2 = VecT<float, 2>;
using Vec3 = VecT<float, 3>;
using Vec4 = VecT<float, 4>;

template <class T, std::size_t N, std::size_t... Is>
struct SwizzleT;

namespace details {

template <class T, std::size_t N>
struct VecBase {
    constexpr T& operator[](std::size_t i) noexcept {
        return static_cast<VecT<T, N>*>(this)->data[i];
    }

    constexpr const T& operator[](std::size_t i) const noexcept {
        return static_cast<const VecT<T, N>*>(this)->data[i];
    }
};

template <std::size_t... Is>
struct SwizzleIndexArray {
    static constexpr std::array<std::size_t, sizeof...(Is)> value = {Is...};
};

template <class T, std::size_t N, class LIS = std::make_index_sequence<N>, class RIS = std::make_index_sequence<N>>
struct VecOp;
template <class T, std::size_t N, std::size_t... Ls, std::size_t... Rs>
struct VecOp<T, N, std::index_sequence<Ls...>, std::index_sequence<Rs...>> {

    template <std::size_t L, std::size_t R>
    static constexpr void Assign(T (&lhs)[L], const T (&rhs)[R]) noexcept {
        (..., (lhs[Ls] = rhs[Rs]));
    }

    template <std::size_t L, std::size_t R>
    static constexpr VecT<T, N> Add(const T (&lhs)[L], const T (&rhs)[R]) noexcept {
        return VecT<T, N>((lhs[Ls] + rhs[Rs])...);
    }

    template <std::size_t L, std::size_t R>
    static constexpr void AddA(T (&lhs)[L], const T (&rhs)[R]) noexcept {
        (..., (lhs[Ls] += rhs[Rs]));
    }

    template <std::size_t L>
    static constexpr VecT<T, N> Add(const T (&lhs)[L], T rhs) noexcept {
        return VecT<T, N>((lhs[Ls] + rhs)...);
    }

    template <std::size_t L>
    static constexpr void AddA(T (&lhs)[L], T rhs) noexcept {
        (..., (lhs[Ls] += rhs));
    }

    template <std::size_t L, std::size_t R>
    static constexpr VecT<T, N> Sub(const T (&lhs)[L], const T (&rhs)[R]) noexcept {
        return VecT<T, N>((lhs[Ls] - rhs[Rs])...);
    }

    template <std::size_t L, std::size_t R>
    static constexpr void SubA(T (&lhs)[L], const T (&rhs)[R]) noexcept {
        (..., (lhs[Ls] -= rhs[Rs]));
    }

    template <std::size_t L>
    static constexpr VecT<T, N> Sub(const T (&lhs)[L], T rhs) noexcept {
        return VecT<T, N>((lhs[Ls] - rhs)...);
    }

    template <std::size_t L>
    static constexpr void SubA(T (&lhs)[L], T rhs) noexcept {
        (..., (lhs[Ls] -= rhs));
    }

    template <std::size_t L>
    static constexpr VecT<T, N> RSub(const T (&lhs)[L], T rhs) noexcept {
        return VecT<T, N>((rhs - lhs[Ls])...);
    }

    template <std::size_t L, std::size_t R>
    static constexpr VecT<T, N> Mul(const T (&lhs)[L], const T (&rhs)[R]) noexcept {
        return VecT<T, N>((lhs[Ls] * rhs[Rs])...);
    }

    template <std::size_t L, std::size_t R>
    static constexpr void MulA(T (&lhs)[L], const T (&rhs)[R]) noexcept {
        (..., (lhs[Ls] *= rhs[Rs]));
    }

    template <std::size_t L>
    static constexpr VecT<T, N> Mul(const T (&lhs)[L], T rhs) noexcept {
        return VecT<T, N>((lhs[Ls] * rhs)...);
    }

    template <std::size_t L>
    static constexpr void MulA(T (&lhs)[L], T rhs) noexcept {
        (..., (lhs[Ls] *= rhs));
    }

    template <std::size_t L, std::size_t R>
    static constexpr VecT<T, N> Div(const T (&lhs)[L], const T (&rhs)[R]) noexcept {
        return VecT<T, N>((lhs[Ls] / rhs[Rs])...);
    }

    template <std::size_t L, std::size_t R>
    static constexpr void DivA(T (&lhs)[L], const T (&rhs)[R]) noexcept {
        (..., (lhs[Ls] /= rhs[Rs]));
    }

    template <std::size_t L>
    static constexpr VecT<T, N> Div(const T (&lhs)[L], T rhs) noexcept {
        return VecT<T, N>((lhs[Ls] / rhs)...);
    }

    template <std::size_t L>
    static constexpr void DivA(T (&lhs)[L], T rhs) noexcept {
        (..., (lhs[Ls] /= rhs));
    }

    template <std::size_t L>
    static constexpr VecT<T, N> RDiv(const T (&lhs)[L], T rhs) noexcept {
        return VecT<T, N>((rhs / lhs[Ls])...);
    }
};

} // namespace details

template <class T, std::size_t N, std::size_t... Is>
struct SwizzleT {
    T data[N];

    constexpr operator VecT<T, sizeof...(Is)>() const noexcept {
        return VecT<T, sizeof...(Is)>(data[Is]...);
    }

    constexpr SwizzleT& operator=(VecT<T, sizeof...(Is)> v) noexcept {
        details::VecOp<T, sizeof...(Is), std::index_sequence<Is...>>::template Assign(data, v.data);
        return *this;
    }

    constexpr T& operator[](std::size_t i) noexcept {
        return data[details::SwizzleIndexArray<Is...>::value[i]];
    }

    constexpr const T& operator[](std::size_t i) const noexcept {
        return data[details::SwizzleIndexArray<Is...>::value[i]];
    }
};

template <class T>
struct VecT<T, 2> : details::VecBase<T, 2> {
    union {
        T data[2];
        ESL_WARNING_PUSH()
        ESL_WARNING_CLANG(disable, "-Wgnu-anonymous-struct")
        ESL_WARNING_CLANG(disable, "-Wnested-anon-types")
        ESL_WARNING_GCC(disable, "-Wpedantic")
        struct {
            T x;
            T y;
        };
        ESL_WARNING_POP()
        SwizzleT<T, 2, 0, 0> xx;
        SwizzleT<T, 2, 0, 1> xy;
        SwizzleT<T, 2, 1, 0> yx;
        SwizzleT<T, 2, 1, 1> yy;
    };

    constexpr VecT() noexcept = default;
    constexpr VecT(T x, T y) noexcept : x(x), y(y) {}
    constexpr explicit VecT(T x) noexcept : VecT(x, x) {}
};

template <class T>
struct VecT<T, 3> : details::VecBase<T, 3> {
    union {
        T data[3];
        ESL_WARNING_PUSH()
        ESL_WARNING_CLANG(disable, "-Wgnu-anonymous-struct")
        ESL_WARNING_CLANG(disable, "-Wnested-anon-types")
        ESL_WARNING_GCC(disable, "-Wpedantic")
        struct {
            T x;
            T y;
            T z;
        };
        ESL_WARNING_POP()
        SwizzleT<T, 3, 0, 0> xx;
        SwizzleT<T, 3, 0, 1> xy;
        SwizzleT<T, 3, 1, 0> yx;
        SwizzleT<T, 3, 1, 1> yy;
        SwizzleT<T, 3, 0, 2> xz;
        SwizzleT<T, 3, 1, 2> yz;
        SwizzleT<T, 3, 2, 0> zx;
        SwizzleT<T, 3, 2, 1> zy;
        SwizzleT<T, 3, 2, 2> zz;
        SwizzleT<T, 3, 0, 0, 0> xxx;
        SwizzleT<T, 3, 0, 0, 1> xxy;
        SwizzleT<T, 3, 0, 0, 2> xxz;
        SwizzleT<T, 3, 0, 1, 0> xyx;
        SwizzleT<T, 3, 0, 1, 1> xyy;
        SwizzleT<T, 3, 0, 1, 2> xyz;
        SwizzleT<T, 3, 0, 2, 0> xzx;
        SwizzleT<T, 3, 0, 2, 1> xzy;
        SwizzleT<T, 3, 0, 2, 2> xzz;
        SwizzleT<T, 3, 1, 0, 0> yxx;
        SwizzleT<T, 3, 1, 0, 1> yxy;
        SwizzleT<T, 3, 1, 0, 2> yxz;
        SwizzleT<T, 3, 1, 1, 0> yyx;
        SwizzleT<T, 3, 1, 1, 1> yyy;
        SwizzleT<T, 3, 1, 1, 2> yyz;
        SwizzleT<T, 3, 2, 0, 0> zxx;
        SwizzleT<T, 3, 2, 0, 1> zxy;
        SwizzleT<T, 3, 2, 0, 2> zxz;
        SwizzleT<T, 3, 2, 1, 0> zyx;
        SwizzleT<T, 3, 2, 1, 1> zyy;
        SwizzleT<T, 3, 2, 1, 2> zyz;
        SwizzleT<T, 3, 2, 2, 0> zzx;
        SwizzleT<T, 3, 2, 2, 1> zzy;
        SwizzleT<T, 3, 2, 2, 2> zzz;
    };

    constexpr VecT() noexcept = default;
    constexpr VecT(T x, T y, T z) noexcept : x(x), y(y), z(z) {}
    constexpr explicit VecT(T x) noexcept : VecT(x, x, x) {}

    constexpr VecT(VecT<T, 2> xy, T z) noexcept : VecT(xy[0], xy[1], z) {}
    constexpr VecT(T x, VecT<T, 2> yz) noexcept : VecT(x, yz[0], yz[1]) {}
};

template <class T>
struct VecT<T, 4> : details::VecBase<T, 4> {
    union {
        T data[4];
        ESL_WARNING_PUSH()
        ESL_WARNING_CLANG(disable, "-Wgnu-anonymous-struct")
        ESL_WARNING_CLANG(disable, "-Wnested-anon-types")
        ESL_WARNING_GCC(disable, "-Wpedantic")
        struct {
            T x;
            T y;
            T z;
            T w;
        };
        ESL_WARNING_POP()
        SwizzleT<T, 4, 0, 0> xx;
        SwizzleT<T, 4, 0, 1> xy;
        SwizzleT<T, 4, 1, 0> yx;
        SwizzleT<T, 4, 1, 1> yy;
        SwizzleT<T, 4, 0, 2> xz;
        SwizzleT<T, 4, 1, 2> yz;
        SwizzleT<T, 4, 2, 0> zx;
        SwizzleT<T, 4, 2, 1> zy;
        SwizzleT<T, 4, 2, 2> zz;
        SwizzleT<T, 4, 0, 3> xw;
        SwizzleT<T, 4, 1, 3> yw;
        SwizzleT<T, 4, 2, 3> zw;
        SwizzleT<T, 4, 3, 0> wx;
        SwizzleT<T, 4, 3, 1> wy;
        SwizzleT<T, 4, 3, 2> wz;
        SwizzleT<T, 4, 3, 3> ww;
        SwizzleT<T, 4, 0, 0, 0> xxx;
        SwizzleT<T, 4, 0, 0, 1> xxy;
        SwizzleT<T, 4, 0, 0, 2> xxz;
        SwizzleT<T, 4, 0, 1, 0> xyx;
        SwizzleT<T, 4, 0, 1, 1> xyy;
        SwizzleT<T, 4, 0, 1, 2> xyz;
        SwizzleT<T, 4, 0, 2, 0> xzx;
        SwizzleT<T, 4, 0, 2, 1> xzy;
        SwizzleT<T, 4, 0, 2, 2> xzz;
        SwizzleT<T, 4, 1, 0, 0> yxx;
        SwizzleT<T, 4, 1, 0, 1> yxy;
        SwizzleT<T, 4, 1, 0, 2> yxz;
        SwizzleT<T, 4, 1, 1, 0> yyx;
        SwizzleT<T, 4, 1, 1, 1> yyy;
        SwizzleT<T, 4, 1, 1, 2> yyz;
        SwizzleT<T, 4, 2, 0, 0> zxx;
        SwizzleT<T, 4, 2, 0, 1> zxy;
        SwizzleT<T, 4, 2, 0, 2> zxz;
        SwizzleT<T, 4, 2, 1, 0> zyx;
        SwizzleT<T, 4, 2, 1, 1> zyy;
        SwizzleT<T, 4, 2, 1, 2> zyz;
        SwizzleT<T, 4, 2, 2, 0> zzx;
        SwizzleT<T, 4, 2, 2, 1> zzy;
        SwizzleT<T, 4, 2, 2, 2> zzz;
        SwizzleT<T, 4, 0, 0, 3> xxw;
        SwizzleT<T, 4, 0, 1, 3> xyw;
        SwizzleT<T, 4, 0, 2, 3> xzw;
        SwizzleT<T, 4, 0, 3, 3> xww;
        SwizzleT<T, 4, 1, 0, 3> yxw;
        SwizzleT<T, 4, 1, 1, 3> yyw;
        SwizzleT<T, 4, 1, 2, 3> yzw;
        SwizzleT<T, 4, 1, 3, 3> yww;
        SwizzleT<T, 4, 2, 0, 3> zxw;
        SwizzleT<T, 4, 2, 1, 3> zyw;
        SwizzleT<T, 4, 2, 2, 3> zzw;
        SwizzleT<T, 4, 2, 3, 3> zww;
        SwizzleT<T, 4, 3, 0, 3> wxw;
        SwizzleT<T, 4, 3, 1, 3> wyw;
        SwizzleT<T, 4, 3, 2, 3> wzw;
        SwizzleT<T, 4, 3, 3, 3> www;
        SwizzleT<T, 4, 0, 0, 0, 0> xxxx;
        SwizzleT<T, 4, 0, 0, 0, 1> xxxy;
        SwizzleT<T, 4, 0, 0, 0, 2> xxxz;
        SwizzleT<T, 4, 0, 0, 0, 3> xxxw;
        SwizzleT<T, 4, 0, 0, 1, 0> xxyx;
        SwizzleT<T, 4, 0, 0, 1, 1> xxyy;
        SwizzleT<T, 4, 0, 0, 1, 2> xxyz;
        SwizzleT<T, 4, 0, 0, 1, 3> xxyw;
        SwizzleT<T, 4, 0, 0, 2, 0> xxzx;
        SwizzleT<T, 4, 0, 0, 2, 1> xxzy;
        SwizzleT<T, 4, 0, 0, 2, 2> xxzz;
        SwizzleT<T, 4, 0, 0, 2, 3> xxzw;
        SwizzleT<T, 4, 0, 0, 3, 0> xxwx;
        SwizzleT<T, 4, 0, 0, 3, 1> xxwy;
        SwizzleT<T, 4, 0, 0, 3, 2> xxwz;
        SwizzleT<T, 4, 0, 0, 3, 3> xxww;
        SwizzleT<T, 4, 0, 1, 0, 0> xyxx;
        SwizzleT<T, 4, 0, 1, 0, 1> xyxy;
        SwizzleT<T, 4, 0, 1, 0, 2> xyxz;
        SwizzleT<T, 4, 0, 1, 0, 3> xyxw;
        SwizzleT<T, 4, 0, 1, 1, 0> xyyx;
        SwizzleT<T, 4, 0, 1, 1, 1> xyyy;
        SwizzleT<T, 4, 0, 1, 1, 2> xyyz;
        SwizzleT<T, 4, 0, 1, 1, 3> xyyw;
        SwizzleT<T, 4, 0, 1, 2, 0> xyzx;
        SwizzleT<T, 4, 0, 1, 2, 1> xyzy;
        SwizzleT<T, 4, 0, 1, 2, 2> xyzz;
        SwizzleT<T, 4, 0, 1, 2, 3> xyzw;
        SwizzleT<T, 4, 0, 1, 3, 0> xywx;
        SwizzleT<T, 4, 0, 1, 3, 1> xywy;
        SwizzleT<T, 4, 0, 1, 3, 2> xywz;
        SwizzleT<T, 4, 0, 1, 3, 3> xyww;
        SwizzleT<T, 4, 0, 2, 0, 0> xzxx;
        SwizzleT<T, 4, 0, 2, 0, 1> xzxy;
        SwizzleT<T, 4, 0, 2, 0, 2> xzxz;
        SwizzleT<T, 4, 0, 2, 0, 3> xzxw;
        SwizzleT<T, 4, 0, 2, 1, 0> xzyx;
        SwizzleT<T, 4, 0, 2, 1, 1> xzyy;
        SwizzleT<T, 4, 0, 2, 1, 2> xzyz;
        SwizzleT<T, 4, 0, 2, 1, 3> xzyw;
        SwizzleT<T, 4, 0, 2, 2, 0> xzzx;
        SwizzleT<T, 4, 0, 2, 2, 1> xzzy;
        SwizzleT<T, 4, 0, 2, 2, 2> xzzz;
        SwizzleT<T, 4, 0, 2, 2, 3> xzzw;
        SwizzleT<T, 4, 0, 2, 3, 0> xzwx;
        SwizzleT<T, 4, 0, 2, 3, 1> xzwy;
        SwizzleT<T, 4, 0, 2, 3, 2> xzwz;
        SwizzleT<T, 4, 0, 2, 3, 3> xzww;
        SwizzleT<T, 4, 0, 3, 0, 0> xwxx;
        SwizzleT<T, 4, 0, 3, 0, 1> xwxy;
        SwizzleT<T, 4, 0, 3, 0, 2> xwxz;
        SwizzleT<T, 4, 0, 3, 0, 3> xwxw;
        SwizzleT<T, 4, 0, 3, 1, 0> xwyx;
        SwizzleT<T, 4, 0, 3, 1, 1> xwyy;
        SwizzleT<T, 4, 0, 3, 1, 2> xwyz;
        SwizzleT<T, 4, 0, 3, 1, 3> xwyw;
        SwizzleT<T, 4, 0, 3, 2, 0> xwzx;
        SwizzleT<T, 4, 0, 3, 2, 1> xwzy;
        SwizzleT<T, 4, 0, 3, 2, 2> xwzz;
        SwizzleT<T, 4, 0, 3, 2, 3> xwzw;
        SwizzleT<T, 4, 0, 3, 3, 0> xwwx;
        SwizzleT<T, 4, 0, 3, 3, 1> xwwy;
        SwizzleT<T, 4, 0, 3, 3, 2> xwwz;
        SwizzleT<T, 4, 0, 3, 3, 3> xwww;
        SwizzleT<T, 4, 1, 0, 0, 0> yxxx;
        SwizzleT<T, 4, 1, 0, 0, 1> yxxy;
        SwizzleT<T, 4, 1, 0, 0, 2> yxxz;
        SwizzleT<T, 4, 1, 0, 0, 3> yxxw;
        SwizzleT<T, 4, 1, 0, 1, 0> yxyx;
        SwizzleT<T, 4, 1, 0, 1, 1> yxyy;
        SwizzleT<T, 4, 1, 0, 1, 2> yxyz;
        SwizzleT<T, 4, 1, 0, 1, 3> yxyw;
        SwizzleT<T, 4, 1, 0, 2, 0> yxzx;
        SwizzleT<T, 4, 1, 0, 2, 1> yxzy;
        SwizzleT<T, 4, 1, 0, 2, 2> yxzz;
        SwizzleT<T, 4, 1, 0, 2, 3> yxzw;
        SwizzleT<T, 4, 1, 0, 3, 0> yxwx;
        SwizzleT<T, 4, 1, 0, 3, 1> yxwy;
        SwizzleT<T, 4, 1, 0, 3, 2> yxwz;
        SwizzleT<T, 4, 1, 0, 3, 3> yxww;
        SwizzleT<T, 4, 1, 1, 0, 0> yyxx;
        SwizzleT<T, 4, 1, 1, 0, 1> yyxy;
        SwizzleT<T, 4, 1, 1, 0, 2> yyxz;
        SwizzleT<T, 4, 1, 1, 0, 3> yyxw;
        SwizzleT<T, 4, 1, 1, 1, 0> yyyx;
        SwizzleT<T, 4, 1, 1, 1, 1> yyyy;
        SwizzleT<T, 4, 1, 1, 1, 2> yyyz;
        SwizzleT<T, 4, 1, 1, 1, 3> yyyw;
        SwizzleT<T, 4, 1, 1, 2, 0> yyzx;
        SwizzleT<T, 4, 1, 1, 2, 1> yyzy;
        SwizzleT<T, 4, 1, 1, 2, 2> yyzz;
        SwizzleT<T, 4, 1, 1, 2, 3> yyzw;
        SwizzleT<T, 4, 1, 1, 3, 0> yywx;
        SwizzleT<T, 4, 1, 1, 3, 1> yywy;
        SwizzleT<T, 4, 1, 1, 3, 2> yywz;
        SwizzleT<T, 4, 1, 1, 3, 3> yyww;
        SwizzleT<T, 4, 1, 2, 0, 0> yzxx;
        SwizzleT<T, 4, 1, 2, 0, 1> yzxy;
        SwizzleT<T, 4, 1, 2, 0, 2> yzxz;
        SwizzleT<T, 4, 1, 2, 0, 3> yzxw;
        SwizzleT<T, 4, 1, 2, 1, 0> yzyx;
        SwizzleT<T, 4, 1, 2, 1, 1> yzyy;
        SwizzleT<T, 4, 1, 2, 1, 2> yzyz;
        SwizzleT<T, 4, 1, 2, 1, 3> yzyw;
        SwizzleT<T, 4, 1, 2, 2, 0> yzzx;
        SwizzleT<T, 4, 1, 2, 2, 1> yzzy;
        SwizzleT<T, 4, 1, 2, 2, 2> yzzz;
        SwizzleT<T, 4, 1, 2, 2, 3> yzzw;
        SwizzleT<T, 4, 1, 2, 3, 0> yzwx;
        SwizzleT<T, 4, 1, 2, 3, 1> yzwy;
        SwizzleT<T, 4, 1, 2, 3, 2> yzwz;
        SwizzleT<T, 4, 1, 2, 3, 3> yzww;
        SwizzleT<T, 4, 1, 3, 0, 0> ywxx;
        SwizzleT<T, 4, 1, 3, 0, 1> ywxy;
        SwizzleT<T, 4, 1, 3, 0, 2> ywxz;
        SwizzleT<T, 4, 1, 3, 0, 3> ywxw;
        SwizzleT<T, 4, 1, 3, 1, 0> ywyx;
        SwizzleT<T, 4, 1, 3, 1, 1> ywyy;
        SwizzleT<T, 4, 1, 3, 1, 2> ywyz;
        SwizzleT<T, 4, 1, 3, 1, 3> ywyw;
        SwizzleT<T, 4, 1, 3, 2, 0> ywzx;
        SwizzleT<T, 4, 1, 3, 2, 1> ywzy;
        SwizzleT<T, 4, 1, 3, 2, 2> ywzz;
        SwizzleT<T, 4, 1, 3, 2, 3> ywzw;
        SwizzleT<T, 4, 1, 3, 3, 0> ywwx;
        SwizzleT<T, 4, 1, 3, 3, 1> ywwy;
        SwizzleT<T, 4, 1, 3, 3, 2> ywwz;
        SwizzleT<T, 4, 1, 3, 3, 3> ywww;
        SwizzleT<T, 4, 2, 0, 0, 0> zxxx;
        SwizzleT<T, 4, 2, 0, 0, 1> zxxy;
        SwizzleT<T, 4, 2, 0, 0, 2> zxxz;
        SwizzleT<T, 4, 2, 0, 0, 3> zxxw;
        SwizzleT<T, 4, 2, 0, 1, 0> zxyx;
        SwizzleT<T, 4, 2, 0, 1, 1> zxyy;
        SwizzleT<T, 4, 2, 0, 1, 2> zxyz;
        SwizzleT<T, 4, 2, 0, 1, 3> zxyw;
        SwizzleT<T, 4, 2, 0, 2, 0> zxzx;
        SwizzleT<T, 4, 2, 0, 2, 1> zxzy;
        SwizzleT<T, 4, 2, 0, 2, 2> zxzz;
        SwizzleT<T, 4, 2, 0, 2, 3> zxzw;
        SwizzleT<T, 4, 2, 0, 3, 0> zxwx;
        SwizzleT<T, 4, 2, 0, 3, 1> zxwy;
        SwizzleT<T, 4, 2, 0, 3, 2> zxwz;
        SwizzleT<T, 4, 2, 0, 3, 3> zxww;
        SwizzleT<T, 4, 2, 1, 0, 0> zyxx;
        SwizzleT<T, 4, 2, 1, 0, 1> zyxy;
        SwizzleT<T, 4, 2, 1, 0, 2> zyxz;
        SwizzleT<T, 4, 2, 1, 0, 3> zyxw;
        SwizzleT<T, 4, 2, 1, 1, 0> zyyx;
        SwizzleT<T, 4, 2, 1, 1, 1> zyyy;
        SwizzleT<T, 4, 2, 1, 1, 2> zyyz;
        SwizzleT<T, 4, 2, 1, 1, 3> zyyw;
        SwizzleT<T, 4, 2, 1, 2, 0> zyzx;
        SwizzleT<T, 4, 2, 1, 2, 1> zyzy;
        SwizzleT<T, 4, 2, 1, 2, 2> zyzz;
        SwizzleT<T, 4, 2, 1, 2, 3> zyzw;
        SwizzleT<T, 4, 2, 1, 3, 0> zywx;
        SwizzleT<T, 4, 2, 1, 3, 1> zywy;
        SwizzleT<T, 4, 2, 1, 3, 2> zywz;
        SwizzleT<T, 4, 2, 1, 3, 3> zyww;
        SwizzleT<T, 4, 2, 2, 0, 0> zzxx;
        SwizzleT<T, 4, 2, 2, 0, 1> zzxy;
        SwizzleT<T, 4, 2, 2, 0, 2> zzxz;
        SwizzleT<T, 4, 2, 2, 0, 3> zzxw;
        SwizzleT<T, 4, 2, 2, 1, 0> zzyx;
        SwizzleT<T, 4, 2, 2, 1, 1> zzyy;
        SwizzleT<T, 4, 2, 2, 1, 2> zzyz;
        SwizzleT<T, 4, 2, 2, 1, 3> zzyw;
        SwizzleT<T, 4, 2, 2, 2, 0> zzzx;
        SwizzleT<T, 4, 2, 2, 2, 1> zzzy;
        SwizzleT<T, 4, 2, 2, 2, 2> zzzz;
        SwizzleT<T, 4, 2, 2, 2, 3> zzzw;
        SwizzleT<T, 4, 2, 2, 3, 0> zzwx;
        SwizzleT<T, 4, 2, 2, 3, 1> zzwy;
        SwizzleT<T, 4, 2, 2, 3, 2> zzwz;
        SwizzleT<T, 4, 2, 2, 3, 3> zzww;
        SwizzleT<T, 4, 2, 3, 0, 0> zwxx;
        SwizzleT<T, 4, 2, 3, 0, 1> zwxy;
        SwizzleT<T, 4, 2, 3, 0, 2> zwxz;
        SwizzleT<T, 4, 2, 3, 0, 3> zwxw;
        SwizzleT<T, 4, 2, 3, 1, 0> zwyx;
        SwizzleT<T, 4, 2, 3, 1, 1> zwyy;
        SwizzleT<T, 4, 2, 3, 1, 2> zwyz;
        SwizzleT<T, 4, 2, 3, 1, 3> zwyw;
        SwizzleT<T, 4, 2, 3, 2, 0> zwzx;
        SwizzleT<T, 4, 2, 3, 2, 1> zwzy;
        SwizzleT<T, 4, 2, 3, 2, 2> zwzz;
        SwizzleT<T, 4, 2, 3, 2, 3> zwzw;
        SwizzleT<T, 4, 2, 3, 3, 0> zwwx;
        SwizzleT<T, 4, 2, 3, 3, 1> zwwy;
        SwizzleT<T, 4, 2, 3, 3, 2> zwwz;
        SwizzleT<T, 4, 2, 3, 3, 3> zwww;
        SwizzleT<T, 4, 3, 0, 0, 0> wxxx;
        SwizzleT<T, 4, 3, 0, 0, 1> wxxy;
        SwizzleT<T, 4, 3, 0, 0, 2> wxxz;
        SwizzleT<T, 4, 3, 0, 0, 3> wxxw;
        SwizzleT<T, 4, 3, 0, 1, 0> wxyx;
        SwizzleT<T, 4, 3, 0, 1, 1> wxyy;
        SwizzleT<T, 4, 3, 0, 1, 2> wxyz;
        SwizzleT<T, 4, 3, 0, 1, 3> wxyw;
        SwizzleT<T, 4, 3, 0, 2, 0> wxzx;
        SwizzleT<T, 4, 3, 0, 2, 1> wxzy;
        SwizzleT<T, 4, 3, 0, 2, 2> wxzz;
        SwizzleT<T, 4, 3, 0, 2, 3> wxzw;
        SwizzleT<T, 4, 3, 0, 3, 0> wxwx;
        SwizzleT<T, 4, 3, 0, 3, 1> wxwy;
        SwizzleT<T, 4, 3, 0, 3, 2> wxwz;
        SwizzleT<T, 4, 3, 0, 3, 3> wxww;
        SwizzleT<T, 4, 3, 1, 0, 0> wyxx;
        SwizzleT<T, 4, 3, 1, 0, 1> wyxy;
        SwizzleT<T, 4, 3, 1, 0, 2> wyxz;
        SwizzleT<T, 4, 3, 1, 0, 3> wyxw;
        SwizzleT<T, 4, 3, 1, 1, 0> wyyx;
        SwizzleT<T, 4, 3, 1, 1, 1> wyyy;
        SwizzleT<T, 4, 3, 1, 1, 2> wyyz;
        SwizzleT<T, 4, 3, 1, 1, 3> wyyw;
        SwizzleT<T, 4, 3, 1, 2, 0> wyzx;
        SwizzleT<T, 4, 3, 1, 2, 1> wyzy;
        SwizzleT<T, 4, 3, 1, 2, 2> wyzz;
        SwizzleT<T, 4, 3, 1, 2, 3> wyzw;
        SwizzleT<T, 4, 3, 1, 3, 0> wywx;
        SwizzleT<T, 4, 3, 1, 3, 1> wywy;
        SwizzleT<T, 4, 3, 1, 3, 2> wywz;
        SwizzleT<T, 4, 3, 1, 3, 3> wyww;
        SwizzleT<T, 4, 3, 2, 0, 0> wzxx;
        SwizzleT<T, 4, 3, 2, 0, 1> wzxy;
        SwizzleT<T, 4, 3, 2, 0, 2> wzxz;
        SwizzleT<T, 4, 3, 2, 0, 3> wzxw;
        SwizzleT<T, 4, 3, 2, 1, 0> wzyx;
        SwizzleT<T, 4, 3, 2, 1, 1> wzyy;
        SwizzleT<T, 4, 3, 2, 1, 2> wzyz;
        SwizzleT<T, 4, 3, 2, 1, 3> wzyw;
        SwizzleT<T, 4, 3, 2, 2, 0> wzzx;
        SwizzleT<T, 4, 3, 2, 2, 1> wzzy;
        SwizzleT<T, 4, 3, 2, 2, 2> wzzz;
        SwizzleT<T, 4, 3, 2, 2, 3> wzzw;
        SwizzleT<T, 4, 3, 2, 3, 0> wzwx;
        SwizzleT<T, 4, 3, 2, 3, 1> wzwy;
        SwizzleT<T, 4, 3, 2, 3, 2> wzwz;
        SwizzleT<T, 4, 3, 2, 3, 3> wzww;
        SwizzleT<T, 4, 3, 3, 0, 0> wwxx;
        SwizzleT<T, 4, 3, 3, 0, 1> wwxy;
        SwizzleT<T, 4, 3, 3, 0, 2> wwxz;
        SwizzleT<T, 4, 3, 3, 0, 3> wwxw;
        SwizzleT<T, 4, 3, 3, 1, 0> wwyx;
        SwizzleT<T, 4, 3, 3, 1, 1> wwyy;
        SwizzleT<T, 4, 3, 3, 1, 2> wwyz;
        SwizzleT<T, 4, 3, 3, 1, 3> wwyw;
        SwizzleT<T, 4, 3, 3, 2, 0> wwzx;
        SwizzleT<T, 4, 3, 3, 2, 1> wwzy;
        SwizzleT<T, 4, 3, 3, 2, 2> wwzz;
        SwizzleT<T, 4, 3, 3, 2, 3> wwzw;
        SwizzleT<T, 4, 3, 3, 3, 0> wwwx;
        SwizzleT<T, 4, 3, 3, 3, 1> wwwy;
        SwizzleT<T, 4, 3, 3, 3, 2> wwwz;
        SwizzleT<T, 4, 3, 3, 3, 3> wwww;
    };

    constexpr VecT() noexcept = default;
    constexpr VecT(T x, T y, T z, T w) noexcept : x(x), y(y), z(z), w(w) {}
    constexpr explicit VecT(T x) noexcept : VecT(x, x, x, x) {}

    constexpr VecT(VecT<T, 2> xy, T z, T w) noexcept : VecT(xy[0], xy[1], z, w) {}
    constexpr VecT(T x, VecT<T, 2> yz, T w) noexcept : VecT(x, yz[0], yz[1], w) {}
    constexpr VecT(T x, T y, VecT<T, 2> zw) noexcept : VecT(x, y, zw[0], zw[1]) {}
    constexpr VecT(VecT<T, 2> xy, VecT<T, 2> zw) noexcept : VecT(xy[0], xy[1], zw[0], zw[1]) {}
    constexpr VecT(VecT<T, 3> xyz, T w) noexcept : VecT(xyz[0], xyz[1], xyz[2], w) {}
    constexpr VecT(T x, VecT<T, 3> yzw) noexcept : VecT(x, yzw[0], yzw[1], yzw[2]) {}
};

static_assert(sizeof(gm::Vec2) == sizeof(float) * 2);
static_assert(sizeof(gm::Vec3) == sizeof(float) * 3);
static_assert(sizeof(gm::Vec4) == sizeof(float) * 4);

/// Operators

// Vec == Vec
template <class T, std::size_t N>
inline constexpr bool operator==(const VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    return reinterpret_cast<const std::array<T, N>&>(lhs) == reinterpret_cast<const std::array<T, N>&>(rhs);
}

// Vec == Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr bool operator==(const VecT<T, sizeof...(Is)>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return lhs == VecT<T, sizeof...(Is)>(rhs);
}

// Swizzle == Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr bool operator==(const SwizzleT<T, N, Is...>& lhs, const VecT<T, sizeof...(Is)>& rhs) noexcept {
    return rhs == lhs;
}

// Vec != Vec
template <class T, std::size_t N>
inline constexpr bool operator!=(const VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    return !(lhs == rhs);
}

// Vec != Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr bool operator!=(const VecT<T, sizeof...(Is)>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return !(lhs == rhs);
}

// Swizzle != Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr bool operator!=(const SwizzleT<T, N, Is...>& lhs, const VecT<T, sizeof...(Is)>& rhs) noexcept {
    return !(lhs == rhs);
}

// +Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N> operator+(const VecT<T, N>& lhs) noexcept {
    return lhs;
}

// +Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator+(const SwizzleT<T, N, Is...>& lhs) noexcept {
    return lhs;
}

// -Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N> operator-(const VecT<T, N>& lhs) noexcept {
    return details::VecOp<T, N>::RSub(lhs.data, 0);
}

// -Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator-(const SwizzleT<T, N, Is...>& lhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::RSub(lhs.data, 0);
}

// ++Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N>& operator++(VecT<T, N>& lhs) noexcept {
    details::VecOp<T, N>::AddA(lhs.data, 1);
    return lhs;
}

// Vec++
template <class T, std::size_t N>
inline constexpr VecT<T, N> operator++(VecT<T, N>& lhs, int) noexcept {
    auto prev = lhs;
    details::VecOp<T, N>::AddA(lhs.data, 1);
    return prev;
}

// ++Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr SwizzleT<T, N, Is...>& operator++(SwizzleT<T, N, Is...>& lhs) noexcept {
    details::VecOp<T, N, std::index_sequence<Is...>>::AddA(lhs.data, 1);
    return lhs;
}

// Swizzle++
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator++(SwizzleT<T, N, Is...>& lhs, int) noexcept {
    VecT<T, N> prev = lhs;
    details::VecOp<T, N, std::index_sequence<Is...>>::AddA(lhs.data, 1);
    return prev;
}

// Vec + Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N> operator+(const VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N>::Add(lhs.data, rhs.data);
}

// Vec += Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N>& operator+=(VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    details::VecOp<T, N>::AddA(lhs.data, rhs.data);
    return lhs;
}

// Vec + Scalar
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N> operator+(const VecT<T, N>& lhs, U rhs) noexcept {
    return details::VecOp<T, N>::Add(lhs.data, rhs);
}

// Scalar + Vec
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N> operator+(U lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N>::Add(rhs.data, lhs);
}

// Vec += Scalar
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N>& operator+=(VecT<T, N>& lhs, U rhs) noexcept {
    details::VecOp<T, N>::AddA(lhs.data, rhs);
    return lhs;
}

// Vec + Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator+(const VecT<T, N>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return details::VecOp<T, N, std::make_index_sequence<N>, std::index_sequence<Is...>>::Add(lhs.data, rhs.data);
}

// Vec += Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N>& operator+=(VecT<T, N>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    details::VecOp<T, N, std::make_index_sequence<N>, std::index_sequence<Is...>>::AddA(lhs.data, rhs.data);
    return lhs;
}

// Swizzle + Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator+(const SwizzleT<T, N, Is...>& lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Add(lhs.data, rhs.data);
}

// Swizzle += Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr SwizzleT<T, N, Is...>& operator+=(SwizzleT<T, N, Is...>& lhs, const VecT<T, N>& rhs) noexcept {
    details::VecOp<T, N, std::index_sequence<Is...>>::AddA(lhs.data, rhs.data);
    return lhs;
}

// Swizzle + Scalar
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr VecT<T, N> operator+(const SwizzleT<T, N, Is...>& lhs, U rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Add(lhs.data, rhs);
}

// Scalar + Swizzle
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr VecT<T, N> operator+(U lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Add(rhs.data, lhs);
}

// Swizzle += Scalar
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr SwizzleT<T, N, Is...>& operator+=(SwizzleT<T, N, Is...>& lhs, U rhs) noexcept {
    details::VecOp<T, N, std::index_sequence<Is...>>::AddA(lhs.data, rhs);
    return lhs;
}

// Vec - Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N> operator-(const VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N>::Sub(lhs.data, rhs.data);
}

// Vec -= Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N>& operator-=(VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    details::VecOp<T, N>::SubA(lhs.data, rhs.data);
    return lhs;
}

// Vec - Scalar
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N> operator-(const VecT<T, N>& lhs, U rhs) noexcept {
    return details::VecOp<T, N>::Sub(lhs.data, rhs);
}

// Scalar - Vec
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N> operator-(U lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N>::RSub(rhs.data, lhs);
}

// Vec -= Scalar
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N>& operator-=(VecT<T, N>& lhs, U rhs) noexcept {
    details::VecOp<T, N>::SubA(lhs.data, rhs);
    return lhs;
}

// Vec - Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator-(const VecT<T, N>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return details::VecOp<T, N, std::make_index_sequence<N>, std::index_sequence<Is...>>::Sub(lhs.data, rhs.data);
}

// Vec -= Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N>& operator-=(VecT<T, N>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    details::VecOp<T, N, std::make_index_sequence<N>, std::index_sequence<Is...>>::SubA(lhs.data, rhs.data);
    return lhs;
}

// Swizzle - Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator-(const SwizzleT<T, N, Is...>& lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Sub(lhs.data, rhs.data);
}

// Swizzle -= Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr SwizzleT<T, N, Is...>& operator-=(SwizzleT<T, N, Is...>& lhs, const VecT<T, N>& rhs) noexcept {
    details::VecOp<T, N, std::index_sequence<Is...>>::SubA(lhs.data, rhs.data);
    return lhs;
}

// Swizzle - Scalar
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr VecT<T, N> operator-(const SwizzleT<T, N, Is...>& lhs, U rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Sub(lhs.data, rhs);
}

// Scalar - Swizzle
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr VecT<T, N> operator-(U lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::RSub(rhs.data, lhs);
}

// Swizzle -= Scalar
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr SwizzleT<T, N, Is...>& operator-=(SwizzleT<T, N, Is...>& lhs, U rhs) noexcept {
    details::VecOp<T, N, std::index_sequence<Is...>>::SubA(lhs.data, rhs);
    return lhs;
}

// Vec * Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N> operator*(const VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N>::Mul(lhs.data, rhs.data);
}

// Vec *= Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N>& operator*=(VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    details::VecOp<T, N>::MulA(lhs.data, rhs.data);
    return lhs;
}

// Vec * Scalar
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N> operator*(const VecT<T, N>& lhs, U rhs) noexcept {
    return details::VecOp<T, N>::Mul(lhs.data, rhs);
}

// Scalar * Vec
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N> operator*(U lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N>::Mul(rhs.data, lhs);
}

// Vec *= Scalar
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N>& operator*=(VecT<T, N>& lhs, U rhs) noexcept {
    details::VecOp<T, N>::MulA(lhs.data, rhs);
    return lhs;
}

// Vec * Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator*(const VecT<T, N>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return details::VecOp<T, N, std::make_index_sequence<N>, std::index_sequence<Is...>>::Mul(lhs.data, rhs.data);
}

// Vec *= Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N>& operator*=(VecT<T, N>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    details::VecOp<T, N, std::make_index_sequence<N>, std::index_sequence<Is...>>::MulA(lhs.data, rhs.data);
    return lhs;
}

// Swizzle * Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator*(const SwizzleT<T, N, Is...>& lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Mul(lhs.data, rhs.data);
}

// Swizzle *= Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr SwizzleT<T, N, Is...>& operator*=(SwizzleT<T, N, Is...>& lhs, const VecT<T, N>& rhs) noexcept {
    details::VecOp<T, N, std::index_sequence<Is...>>::MulA(lhs.data, rhs.data);
    return lhs;
}

// Swizzle * Scalar
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr VecT<T, N> operator*(const SwizzleT<T, N, Is...>& lhs, U rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Mul(lhs.data, rhs);
}

// Scalar * Swizzle
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr VecT<T, N> operator*(U lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Mul(rhs.data, lhs);
}

// Swizzle *= Scalar
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr SwizzleT<T, N, Is...>& operator*=(SwizzleT<T, N, Is...>& lhs, U rhs) noexcept {
    details::VecOp<T, N, std::index_sequence<Is...>>::MulA(lhs.data, rhs);
    return lhs;
}

// Vec / Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N> operator/(const VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N>::Div(lhs.data, rhs.data);
}

// Vec /= Vec
template <class T, std::size_t N>
inline constexpr VecT<T, N>& operator/=(VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    details::VecOp<T, N>::DivA(lhs.data, rhs.data);
    return lhs;
}

// Vec / Scalar
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N> operator/(const VecT<T, N>& lhs, U rhs) noexcept {
    return details::VecOp<T, N>::Div(lhs.data, rhs);
}

// Scalar / Vec
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N> operator/(U lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N>::Div(lhs, rhs.data);
}

// Vec /= Scalar
template <class T, std::size_t N, class U>
inline constexpr VecT<T, N>& operator/=(VecT<T, N>& lhs, U rhs) noexcept {
    details::VecOp<T, N>::DivA(lhs.data, rhs);
    return lhs;
}

// Vec / Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator/(const VecT<T, N>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return details::VecOp<T, N, std::make_index_sequence<N>, std::index_sequence<Is...>>::Div(lhs.data, rhs.data);
}

// Vec /= Swizzle
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N>& operator/=(VecT<T, N>& lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    details::VecOp<T, N, std::make_index_sequence<N>, std::index_sequence<Is...>>::DivA(lhs.data, rhs.data);
    return lhs;
}

// Swizzle / Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr VecT<T, N> operator/(const SwizzleT<T, N, Is...>& lhs, const VecT<T, N>& rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Div(lhs.data, rhs.data);
}

// Swizzle /= Vec
template <class T, std::size_t N, std::size_t... Is>
inline constexpr SwizzleT<T, N, Is...>& operator/=(SwizzleT<T, N, Is...>& lhs, const VecT<T, N>& rhs) noexcept {
    details::VecOp<T, N, std::index_sequence<Is...>>::DivA(lhs.data, rhs.data);
    return lhs;
}

// Swizzle / Scalar
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr VecT<T, N> operator/(const SwizzleT<T, N, Is...>& lhs, U rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::Div(lhs.data, rhs);
}

// Scalar / Swizzle
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr VecT<T, N> operator/(U lhs, const SwizzleT<T, N, Is...>& rhs) noexcept {
    return details::VecOp<T, N, std::index_sequence<Is...>>::RDiv(rhs.data, lhs);
}

// Swizzle /= Scalar
template <class T, std::size_t N, std::size_t... Is, class U>
inline constexpr SwizzleT<T, N, Is...>& operator/=(SwizzleT<T, N, Is...>& lhs, U rhs) noexcept {
    details::VecOp<T, N, std::index_sequence<Is...>>::DivA(lhs.data, rhs);
    return lhs;
}

template <class CharT, class Traits, class T, std::size_t N>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const VecT<T, N>& v) {
    os << '(' << v[0];
    for (std::size_t i = 1; i < N; ++i) {
        os << ',' << v[i];
    }
    return os << ')';
}

template <class CharT, class Traits, class T, std::size_t N, std::size_t... Is>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const SwizzleT<T, N, Is...>& v) {
    return os << VecT<T, sizeof...(Is)>(v);
}

/// MatT
template <class T, std::size_t C, std::size_t R = C>
struct MatT;

using Mat2 = MatT<float, 2>;
using Mat3 = MatT<float, 3>;
using Mat4 = MatT<float, 4>;

template <class T>
struct MatT<T, 2, 2> : details::VecBase<VecT<T, 2>, 2> {
    union {
        VecT<T, 2> data[2];
        ESL_WARNING_PUSH()
        ESL_WARNING_CLANG(disable, "-Wgnu-anonymous-struct")
        ESL_WARNING_CLANG(disable, "-Wnested-anon-types")
        ESL_WARNING_GCC(disable, "-Wpedantic")
        struct {
            VecT<T, 2> x;
            VecT<T, 2> y;
        };
        ESL_WARNING_POP()
    };

    constexpr MatT() noexcept = default;
    constexpr MatT(T xx, T xy, T yx, T yy) noexcept : x(xx, xy), y(yx, yy) {}
    constexpr explicit MatT(T x) noexcept : MatT(x, 0, 0, x) {}

    constexpr MatT(VecT<T, 2> x, VecT<T, 2> y) noexcept : x(x), y(y) {}
    constexpr MatT(T xx, T xy, VecT<T, 2> y) noexcept : x(xx, xy), y(y) {}
    constexpr MatT(VecT<T, 2> x, T yx, T yy) noexcept : x(x), y(yx, yy) {}
};

template <class T>
struct MatT<T, 3, 3> : details::VecBase<VecT<T, 3>, 3> {
    union {
        VecT<T, 3> data[3];
        ESL_WARNING_PUSH()
        ESL_WARNING_CLANG(disable, "-Wgnu-anonymous-struct")
        ESL_WARNING_CLANG(disable, "-Wnested-anon-types")
        ESL_WARNING_GCC(disable, "-Wpedantic")
        struct {
            VecT<T, 3> x;
            VecT<T, 3> y;
            VecT<T, 3> z;
        };
        ESL_WARNING_POP()
    };

    constexpr MatT() noexcept = default;
    constexpr MatT(T xx, T xy, T xz, T yx, T yy, T yz, T zx, T zy, T zz) noexcept : x(xx, xy, xz), y(yx, yy, yz), z(zx, zy, zz) {}
    constexpr explicit MatT(T x) noexcept : MatT(x, 0, 0, 0, x, 0, 0, 0, x) {}
    constexpr explicit MatT(MatT<T, 2> m2) noexcept : MatT(m2.x.x, m2.x.y, 0, m2.y.x, m2.y.y, 0, 0, 0, 1) {}

    constexpr MatT(VecT<T, 3> x, VecT<T, 3> y, VecT<T, 3> z) noexcept : x(x), y(y), z(z) {}
    constexpr MatT(T xx, T xy, T xz, VecT<T, 3> y, VecT<T, 3> z) noexcept : x(xx, xy, xz), y(y), z(z) {}
    constexpr MatT(VecT<T, 3> x, T yx, T yy, T yz, VecT<T, 3> z) noexcept : x(x), y(yx, yy, yz), z(z) {}
    constexpr MatT(VecT<T, 3> x, VecT<T, 3> y, T zx, T zy, T zz) noexcept : x(x), y(y), z(zx, zy, zz) {}
    constexpr MatT(T xx, T xy, T xz, T yx, T yy, T yz, VecT<T, 3> z) noexcept : x(xx, xy, xz), y(yx, yy, yz), z(z) {}
    constexpr MatT(T xx, T xy, T xz, VecT<T, 3> y, T zx, T zy, T zz) noexcept : x(xx, xy, xz), y(y), z(zx, zy, zz) {}
    constexpr MatT(VecT<T, 3> x, T yx, T yy, T yz, T zx, T zy, T zz) noexcept : x(x), y(yx, yy, yz), z(zx, zy, zz) {}
};

template <class T>
struct MatT<T, 4, 4> : details::VecBase<VecT<T, 4>, 4> {
    union {
        VecT<T, 4> data[4];
        ESL_WARNING_PUSH()
        ESL_WARNING_CLANG(disable, "-Wgnu-anonymous-struct")
        ESL_WARNING_CLANG(disable, "-Wnested-anon-types")
        ESL_WARNING_GCC(disable, "-Wpedantic")
        struct {
            VecT<T, 4> x;
            VecT<T, 4> y;
            VecT<T, 4> z;
            VecT<T, 4> w;
        };
        ESL_WARNING_POP()
    };

    constexpr MatT() noexcept = default;
    constexpr MatT(T xx, T xy, T xz, T xw, T yx, T yy, T yz, T yw, T zx, T zy, T zz, T zw, T wx, T wy, T wz, T ww) noexcept
        : x(xx, xy, xz, xw), y(yx, yy, yz, yw), z(zx, zy, zz, zw), w(wx, wy, wz, ww) {}
    constexpr explicit MatT(T x) noexcept : MatT(x, 0, 0, 0, 0, x, 0, 0, 0, 0, x, 0, 0, 0, 0, x) {}
    constexpr explicit MatT(MatT<T, 2> m2) noexcept : MatT(m2.x.x, m2.x.y, 0, 0, m2.y.x, m2.y.y, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) {}
    constexpr explicit MatT(MatT<T, 3> m3) noexcept : MatT(m3.x.x, m3.x.y, m3.x.z, 0, m3.y.x, m3.y.y, m3.y.z, 0, m3.z.x, m3.z.y, m3.z.z, 0, 0, 0, 0, 1) {}

    constexpr MatT(VecT<T, 4> x, VecT<T, 4> y, VecT<T, 4> z, VecT<T, 4> w) noexcept : x(x), y(y), z(z), w(w) {}
    constexpr MatT(T xx, T xy, T xz, T xw, VecT<T, 4> y, VecT<T, 4> z, VecT<T, 4> w) noexcept : x(xx, xy, xz, xw), y(y), z(z), w(w) {}
    constexpr MatT(VecT<T, 4> x, T yx, T yy, T yz, T yw, VecT<T, 4> z, VecT<T, 4> w) noexcept : x(x), y(yx, yy, yz, yw), z(z), w(w) {}
    constexpr MatT(VecT<T, 4> x, VecT<T, 4> y, T zx, T zy, T zz, T zw, VecT<T, 4> w) noexcept : x(x), y(y), z(zx, zy, zz, zw), w(w) {}
    constexpr MatT(VecT<T, 4> x, VecT<T, 4> y, VecT<T, 4> z, T wx, T wy, T wz, T ww) noexcept : x(x), y(y), z(z), w(wx, wy, wz, ww) {}
    constexpr MatT(T xx, T xy, T xz, T xw, T yx, T yy, T yz, T yw, VecT<T, 4> z, VecT<T, 4> w) noexcept : x(xx, xy, xz, xw), y(yx, yy, yz, yw), z(z), w(w) {}
    constexpr MatT(T xx, T xy, T xz, T xw, VecT<T, 4> y, T zx, T zy, T zz, T zw, VecT<T, 4> w) noexcept : x(xx, xy, xz, xw), y(y), z(zx, zy, zz, zw), w(w) {}
    constexpr MatT(T xx, T xy, T xz, T xw, VecT<T, 4> y, VecT<T, 4> z, T wx, T wy, T wz, T ww) noexcept : x(xx, xy, xz, xw), y(y), z(z), w(wx, wy, wz, ww) {}
    constexpr MatT(VecT<T, 4> x, T yx, T yy, T yz, T yw, T zx, T zy, T zz, T zw, VecT<T, 4> w) noexcept : x(x), y(yx, yy, yz, yw), z(zx, zy, zz, zw), w(w) {}
    constexpr MatT(VecT<T, 4> x, T yx, T yy, T yz, T yw, VecT<T, 4> z, T wx, T wy, T wz, T ww) noexcept : x(x), y(yx, yy, yz, yw), z(z), w(wx, wy, wz, ww) {}
    constexpr MatT(VecT<T, 4> x, VecT<T, 4> y, T zx, T zy, T zz, T zw, T wx, T wy, T wz, T ww) noexcept : x(x), y(y), z(zx, zy, zz, zw), w(wx, wy, wz, ww) {}
    constexpr MatT(T xx, T xy, T xz, T xw, T yx, T yy, T yz, T yw, T zx, T zy, T zz, T zw, VecT<T, 4> w) noexcept
        : x(xx, xy, xz, xw), y(yx, yy, yz, yw), z(zx, zy, zz, zw), w(w) {}
    constexpr MatT(T xx, T xy, T xz, T xw, T yx, T yy, T yz, T yw, VecT<T, 4> z, T wx, T wy, T wz, T ww) noexcept
        : x(xx, xy, xz, xw), y(yx, yy, yz, yw), z(z), w(wx, wy, wz, ww) {}
    constexpr MatT(T xx, T xy, T xz, T xw, VecT<T, 4> y, T zx, T zy, T zz, T zw, T wx, T wy, T wz, T ww) noexcept
        : x(xx, xy, xz, xw), y(y), z(zx, zy, zz, zw), w(wx, wy, wz, ww) {}
    constexpr MatT(VecT<T, 4> x, T yx, T yy, T yz, T yw, T zx, T zy, T zz, T zw, T wx, T wy, T wz, T ww) noexcept
        : x(x), y(yx, yy, yz, yw), z(zx, zy, zz, zw), w(wx, wy, wz, ww) {}
};

static_assert(sizeof(Mat2) == sizeof(float) * 4);
static_assert(sizeof(Mat3) == sizeof(float) * 9);
static_assert(sizeof(Mat4) == sizeof(float) * 16);

template <class T, std::size_t C, std::size_t R>
inline constexpr bool operator==(const MatT<T, C, R>& lhs, const MatT<T, C, R>& rhs) noexcept {
    return reinterpret_cast<const std::array<T, C * R>&>(lhs) == reinterpret_cast<const std::array<T, C * R>&>(rhs);
}

template <class CharT, class Traits, class T, std::size_t C, std::size_t R>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const MatT<T, C, R>& m) {
    os << '|' << m[0][0];
    for (std::size_t j = 1; j < R; ++j) {
        os << ' ' << m[0][j];
    }
    for (std::size_t i = 1; i < C; ++i) {
        os << '|' << m[i][0];
        for (std::size_t j = 1; j < R; ++j) {
            os << ' ' << m[i][j];
        }
    }
    return os << '|';
}

} // namespace gm
} // namespace esl
