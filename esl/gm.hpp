#include <array>
#include <ostream>
#include <type_traits>
#include <utility>

namespace esl {
namespace gm {

template <class T, std::size_t N>
struct VecT;

namespace details {

template <class T, std::size_t N>
struct VecBase {
public:
    constexpr T& operator[](std::size_t i) noexcept {
        return reinterpret_cast<T*>(this)[i];
    }

    constexpr const T& operator[](std::size_t i) const noexcept {
        return reinterpret_cast<const T*>(this)[i];
    }
};

} // namespace details

template <class T, std::size_t... Is>
struct SwizzleT {
    constexpr operator VecT<T, sizeof...(Is)>() const noexcept {
        return VecT<T, sizeof...(Is)>(reinterpret_cast<const T*>(this)[Is]...);
    }

    constexpr SwizzleT<T, Is...>& operator=(VecT<T, sizeof...(Is)> v) noexcept {
        Assign<std::make_index_sequence<sizeof...(Is)>>::assign(reinterpret_cast<T*>(this), v);
        return *this;
    }

private:
    template <class U>
    struct Assign;
    template <std::size_t... Js>
    struct Assign<std::index_sequence<Js...>> {
        static constexpr void assign(T* data, const VecT<T, sizeof...(Is)>& v) noexcept {
            (..., (data[Is] = v[Js]));
        }
    };
};

template <class T>
struct VecT<T, 2> : details::VecBase<T, 2> {
    union {
        T data[2];
        struct {
            T x;
            T y;
        };
        SwizzleT<T, 0, 0> xx;
        SwizzleT<T, 0, 1> xy;
        SwizzleT<T, 1, 0> yx;
        SwizzleT<T, 1, 1> yy;
    };

    constexpr VecT() noexcept = default;
    constexpr VecT(T x) noexcept : x(x) {}
    constexpr VecT(T x, T y) noexcept : x(x), y(y) {}
};

template <class T>
struct VecT<T, 3> : details::VecBase<T, 3> {
    union {
        T data[3];
        struct {
            T x;
            T y;
            T z;
        };
        SwizzleT<T, 0, 0> xx;
        SwizzleT<T, 0, 1> xy;
        SwizzleT<T, 1, 0> yx;
        SwizzleT<T, 1, 1> yy;
        SwizzleT<T, 0, 2> xz;
        SwizzleT<T, 1, 2> yz;
        SwizzleT<T, 2, 0> zx;
        SwizzleT<T, 2, 1> zy;
        SwizzleT<T, 2, 2> zz;
        SwizzleT<T, 0, 0, 0> xxx;
        SwizzleT<T, 0, 0, 1> xxy;
        SwizzleT<T, 0, 0, 2> xxz;
        SwizzleT<T, 0, 1, 0> xyx;
        SwizzleT<T, 0, 1, 1> xyy;
        SwizzleT<T, 0, 1, 2> xyz;
        SwizzleT<T, 0, 2, 0> xzx;
        SwizzleT<T, 0, 2, 1> xzy;
        SwizzleT<T, 0, 2, 2> xzz;
        SwizzleT<T, 1, 0, 0> yxx;
        SwizzleT<T, 1, 0, 1> yxy;
        SwizzleT<T, 1, 0, 2> yxz;
        SwizzleT<T, 1, 1, 0> yyx;
        SwizzleT<T, 1, 1, 1> yyy;
        SwizzleT<T, 1, 1, 2> yyz;
        SwizzleT<T, 2, 0, 0> zxx;
        SwizzleT<T, 2, 0, 1> zxy;
        SwizzleT<T, 2, 0, 2> zxz;
        SwizzleT<T, 2, 1, 0> zyx;
        SwizzleT<T, 2, 1, 1> zyy;
        SwizzleT<T, 2, 1, 2> zyz;
        SwizzleT<T, 2, 2, 0> zzx;
        SwizzleT<T, 2, 2, 1> zzy;
        SwizzleT<T, 2, 2, 2> zzz;
    };

    constexpr VecT() noexcept = default;
    constexpr VecT(T x) noexcept : x(x) {}
    constexpr VecT(T x, T y) noexcept : x(x), y(y) {}
    constexpr VecT(T x, T y, T z) noexcept : x(x), y(y), z(z) {}
    constexpr VecT(VecT<T, 2> xy) noexcept : VecT(xy.x, xy.y) {}
    constexpr VecT(VecT<T, 2> xy, T z) noexcept : VecT(xy.x, xy.y, z) {}
    constexpr VecT(T x, VecT<T, 2> yz) noexcept : VecT(x, yz.x, yz.z) {}
};

template <class T>
struct VecT<T, 4> : details::VecBase<T, 4> {
    union {
        T data[4];
        struct {
            T x;
            T y;
            T z;
            T w;
        };
        SwizzleT<T, 0, 0> xx;
        SwizzleT<T, 0, 1> xy;
        SwizzleT<T, 1, 0> yx;
        SwizzleT<T, 1, 1> yy;
        SwizzleT<T, 0, 2> xz;
        SwizzleT<T, 1, 2> yz;
        SwizzleT<T, 2, 0> zx;
        SwizzleT<T, 2, 1> zy;
        SwizzleT<T, 2, 2> zz;
        SwizzleT<T, 0, 3> xw;
        SwizzleT<T, 1, 3> yw;
        SwizzleT<T, 2, 3> zw;
        SwizzleT<T, 3, 0> wx;
        SwizzleT<T, 3, 1> wy;
        SwizzleT<T, 3, 2> wz;
        SwizzleT<T, 3, 3> ww;
        SwizzleT<T, 0, 0, 0> xxx;
        SwizzleT<T, 0, 0, 1> xxy;
        SwizzleT<T, 0, 0, 2> xxz;
        SwizzleT<T, 0, 1, 0> xyx;
        SwizzleT<T, 0, 1, 1> xyy;
        SwizzleT<T, 0, 1, 2> xyz;
        SwizzleT<T, 0, 2, 0> xzx;
        SwizzleT<T, 0, 2, 1> xzy;
        SwizzleT<T, 0, 2, 2> xzz;
        SwizzleT<T, 1, 0, 0> yxx;
        SwizzleT<T, 1, 0, 1> yxy;
        SwizzleT<T, 1, 0, 2> yxz;
        SwizzleT<T, 1, 1, 0> yyx;
        SwizzleT<T, 1, 1, 1> yyy;
        SwizzleT<T, 1, 1, 2> yyz;
        SwizzleT<T, 2, 0, 0> zxx;
        SwizzleT<T, 2, 0, 1> zxy;
        SwizzleT<T, 2, 0, 2> zxz;
        SwizzleT<T, 2, 1, 0> zyx;
        SwizzleT<T, 2, 1, 1> zyy;
        SwizzleT<T, 2, 1, 2> zyz;
        SwizzleT<T, 2, 2, 0> zzx;
        SwizzleT<T, 2, 2, 1> zzy;
        SwizzleT<T, 2, 2, 2> zzz;
        SwizzleT<T, 0, 0, 3> xxw;
        SwizzleT<T, 0, 1, 3> xyw;
        SwizzleT<T, 0, 2, 3> xzw;
        SwizzleT<T, 0, 3, 3> xww;
        SwizzleT<T, 1, 0, 3> yxw;
        SwizzleT<T, 1, 1, 3> yyw;
        SwizzleT<T, 1, 2, 3> yzw;
        SwizzleT<T, 1, 3, 3> yww;
        SwizzleT<T, 2, 0, 3> zxw;
        SwizzleT<T, 2, 1, 3> zyw;
        SwizzleT<T, 2, 2, 3> zzw;
        SwizzleT<T, 2, 3, 3> zww;
        SwizzleT<T, 3, 0, 3> wxw;
        SwizzleT<T, 3, 1, 3> wyw;
        SwizzleT<T, 3, 2, 3> wzw;
        SwizzleT<T, 3, 3, 3> www;
        SwizzleT<T, 0, 0, 0, 0> xxxx;
        SwizzleT<T, 0, 0, 0, 1> xxxy;
        SwizzleT<T, 0, 0, 0, 2> xxxz;
        SwizzleT<T, 0, 0, 0, 3> xxxw;
        SwizzleT<T, 0, 0, 1, 0> xxyx;
        SwizzleT<T, 0, 0, 1, 1> xxyy;
        SwizzleT<T, 0, 0, 1, 2> xxyz;
        SwizzleT<T, 0, 0, 1, 3> xxyw;
        SwizzleT<T, 0, 0, 2, 0> xxzx;
        SwizzleT<T, 0, 0, 2, 1> xxzy;
        SwizzleT<T, 0, 0, 2, 2> xxzz;
        SwizzleT<T, 0, 0, 2, 3> xxzw;
        SwizzleT<T, 0, 0, 3, 0> xxwx;
        SwizzleT<T, 0, 0, 3, 1> xxwy;
        SwizzleT<T, 0, 0, 3, 2> xxwz;
        SwizzleT<T, 0, 0, 3, 3> xxww;
        SwizzleT<T, 0, 1, 0, 0> xyxx;
        SwizzleT<T, 0, 1, 0, 1> xyxy;
        SwizzleT<T, 0, 1, 0, 2> xyxz;
        SwizzleT<T, 0, 1, 0, 3> xyxw;
        SwizzleT<T, 0, 1, 1, 0> xyyx;
        SwizzleT<T, 0, 1, 1, 1> xyyy;
        SwizzleT<T, 0, 1, 1, 2> xyyz;
        SwizzleT<T, 0, 1, 1, 3> xyyw;
        SwizzleT<T, 0, 1, 2, 0> xyzx;
        SwizzleT<T, 0, 1, 2, 1> xyzy;
        SwizzleT<T, 0, 1, 2, 2> xyzz;
        SwizzleT<T, 0, 1, 2, 3> xyzw;
        SwizzleT<T, 0, 1, 3, 0> xywx;
        SwizzleT<T, 0, 1, 3, 1> xywy;
        SwizzleT<T, 0, 1, 3, 2> xywz;
        SwizzleT<T, 0, 1, 3, 3> xyww;
        SwizzleT<T, 0, 2, 0, 0> xzxx;
        SwizzleT<T, 0, 2, 0, 1> xzxy;
        SwizzleT<T, 0, 2, 0, 2> xzxz;
        SwizzleT<T, 0, 2, 0, 3> xzxw;
        SwizzleT<T, 0, 2, 1, 0> xzyx;
        SwizzleT<T, 0, 2, 1, 1> xzyy;
        SwizzleT<T, 0, 2, 1, 2> xzyz;
        SwizzleT<T, 0, 2, 1, 3> xzyw;
        SwizzleT<T, 0, 2, 2, 0> xzzx;
        SwizzleT<T, 0, 2, 2, 1> xzzy;
        SwizzleT<T, 0, 2, 2, 2> xzzz;
        SwizzleT<T, 0, 2, 2, 3> xzzw;
        SwizzleT<T, 0, 2, 3, 0> xzwx;
        SwizzleT<T, 0, 2, 3, 1> xzwy;
        SwizzleT<T, 0, 2, 3, 2> xzwz;
        SwizzleT<T, 0, 2, 3, 3> xzww;
        SwizzleT<T, 0, 3, 0, 0> xwxx;
        SwizzleT<T, 0, 3, 0, 1> xwxy;
        SwizzleT<T, 0, 3, 0, 2> xwxz;
        SwizzleT<T, 0, 3, 0, 3> xwxw;
        SwizzleT<T, 0, 3, 1, 0> xwyx;
        SwizzleT<T, 0, 3, 1, 1> xwyy;
        SwizzleT<T, 0, 3, 1, 2> xwyz;
        SwizzleT<T, 0, 3, 1, 3> xwyw;
        SwizzleT<T, 0, 3, 2, 0> xwzx;
        SwizzleT<T, 0, 3, 2, 1> xwzy;
        SwizzleT<T, 0, 3, 2, 2> xwzz;
        SwizzleT<T, 0, 3, 2, 3> xwzw;
        SwizzleT<T, 0, 3, 3, 0> xwwx;
        SwizzleT<T, 0, 3, 3, 1> xwwy;
        SwizzleT<T, 0, 3, 3, 2> xwwz;
        SwizzleT<T, 0, 3, 3, 3> xwww;
        SwizzleT<T, 1, 0, 0, 0> yxxx;
        SwizzleT<T, 1, 0, 0, 1> yxxy;
        SwizzleT<T, 1, 0, 0, 2> yxxz;
        SwizzleT<T, 1, 0, 0, 3> yxxw;
        SwizzleT<T, 1, 0, 1, 0> yxyx;
        SwizzleT<T, 1, 0, 1, 1> yxyy;
        SwizzleT<T, 1, 0, 1, 2> yxyz;
        SwizzleT<T, 1, 0, 1, 3> yxyw;
        SwizzleT<T, 1, 0, 2, 0> yxzx;
        SwizzleT<T, 1, 0, 2, 1> yxzy;
        SwizzleT<T, 1, 0, 2, 2> yxzz;
        SwizzleT<T, 1, 0, 2, 3> yxzw;
        SwizzleT<T, 1, 0, 3, 0> yxwx;
        SwizzleT<T, 1, 0, 3, 1> yxwy;
        SwizzleT<T, 1, 0, 3, 2> yxwz;
        SwizzleT<T, 1, 0, 3, 3> yxww;
        SwizzleT<T, 1, 1, 0, 0> yyxx;
        SwizzleT<T, 1, 1, 0, 1> yyxy;
        SwizzleT<T, 1, 1, 0, 2> yyxz;
        SwizzleT<T, 1, 1, 0, 3> yyxw;
        SwizzleT<T, 1, 1, 1, 0> yyyx;
        SwizzleT<T, 1, 1, 1, 1> yyyy;
        SwizzleT<T, 1, 1, 1, 2> yyyz;
        SwizzleT<T, 1, 1, 1, 3> yyyw;
        SwizzleT<T, 1, 1, 2, 0> yyzx;
        SwizzleT<T, 1, 1, 2, 1> yyzy;
        SwizzleT<T, 1, 1, 2, 2> yyzz;
        SwizzleT<T, 1, 1, 2, 3> yyzw;
        SwizzleT<T, 1, 1, 3, 0> yywx;
        SwizzleT<T, 1, 1, 3, 1> yywy;
        SwizzleT<T, 1, 1, 3, 2> yywz;
        SwizzleT<T, 1, 1, 3, 3> yyww;
        SwizzleT<T, 1, 2, 0, 0> yzxx;
        SwizzleT<T, 1, 2, 0, 1> yzxy;
        SwizzleT<T, 1, 2, 0, 2> yzxz;
        SwizzleT<T, 1, 2, 0, 3> yzxw;
        SwizzleT<T, 1, 2, 1, 0> yzyx;
        SwizzleT<T, 1, 2, 1, 1> yzyy;
        SwizzleT<T, 1, 2, 1, 2> yzyz;
        SwizzleT<T, 1, 2, 1, 3> yzyw;
        SwizzleT<T, 1, 2, 2, 0> yzzx;
        SwizzleT<T, 1, 2, 2, 1> yzzy;
        SwizzleT<T, 1, 2, 2, 2> yzzz;
        SwizzleT<T, 1, 2, 2, 3> yzzw;
        SwizzleT<T, 1, 2, 3, 0> yzwx;
        SwizzleT<T, 1, 2, 3, 1> yzwy;
        SwizzleT<T, 1, 2, 3, 2> yzwz;
        SwizzleT<T, 1, 2, 3, 3> yzww;
        SwizzleT<T, 1, 3, 0, 0> ywxx;
        SwizzleT<T, 1, 3, 0, 1> ywxy;
        SwizzleT<T, 1, 3, 0, 2> ywxz;
        SwizzleT<T, 1, 3, 0, 3> ywxw;
        SwizzleT<T, 1, 3, 1, 0> ywyx;
        SwizzleT<T, 1, 3, 1, 1> ywyy;
        SwizzleT<T, 1, 3, 1, 2> ywyz;
        SwizzleT<T, 1, 3, 1, 3> ywyw;
        SwizzleT<T, 1, 3, 2, 0> ywzx;
        SwizzleT<T, 1, 3, 2, 1> ywzy;
        SwizzleT<T, 1, 3, 2, 2> ywzz;
        SwizzleT<T, 1, 3, 2, 3> ywzw;
        SwizzleT<T, 1, 3, 3, 0> ywwx;
        SwizzleT<T, 1, 3, 3, 1> ywwy;
        SwizzleT<T, 1, 3, 3, 2> ywwz;
        SwizzleT<T, 1, 3, 3, 3> ywww;
        SwizzleT<T, 2, 0, 0, 0> zxxx;
        SwizzleT<T, 2, 0, 0, 1> zxxy;
        SwizzleT<T, 2, 0, 0, 2> zxxz;
        SwizzleT<T, 2, 0, 0, 3> zxxw;
        SwizzleT<T, 2, 0, 1, 0> zxyx;
        SwizzleT<T, 2, 0, 1, 1> zxyy;
        SwizzleT<T, 2, 0, 1, 2> zxyz;
        SwizzleT<T, 2, 0, 1, 3> zxyw;
        SwizzleT<T, 2, 0, 2, 0> zxzx;
        SwizzleT<T, 2, 0, 2, 1> zxzy;
        SwizzleT<T, 2, 0, 2, 2> zxzz;
        SwizzleT<T, 2, 0, 2, 3> zxzw;
        SwizzleT<T, 2, 0, 3, 0> zxwx;
        SwizzleT<T, 2, 0, 3, 1> zxwy;
        SwizzleT<T, 2, 0, 3, 2> zxwz;
        SwizzleT<T, 2, 0, 3, 3> zxww;
        SwizzleT<T, 2, 1, 0, 0> zyxx;
        SwizzleT<T, 2, 1, 0, 1> zyxy;
        SwizzleT<T, 2, 1, 0, 2> zyxz;
        SwizzleT<T, 2, 1, 0, 3> zyxw;
        SwizzleT<T, 2, 1, 1, 0> zyyx;
        SwizzleT<T, 2, 1, 1, 1> zyyy;
        SwizzleT<T, 2, 1, 1, 2> zyyz;
        SwizzleT<T, 2, 1, 1, 3> zyyw;
        SwizzleT<T, 2, 1, 2, 0> zyzx;
        SwizzleT<T, 2, 1, 2, 1> zyzy;
        SwizzleT<T, 2, 1, 2, 2> zyzz;
        SwizzleT<T, 2, 1, 2, 3> zyzw;
        SwizzleT<T, 2, 1, 3, 0> zywx;
        SwizzleT<T, 2, 1, 3, 1> zywy;
        SwizzleT<T, 2, 1, 3, 2> zywz;
        SwizzleT<T, 2, 1, 3, 3> zyww;
        SwizzleT<T, 2, 2, 0, 0> zzxx;
        SwizzleT<T, 2, 2, 0, 1> zzxy;
        SwizzleT<T, 2, 2, 0, 2> zzxz;
        SwizzleT<T, 2, 2, 0, 3> zzxw;
        SwizzleT<T, 2, 2, 1, 0> zzyx;
        SwizzleT<T, 2, 2, 1, 1> zzyy;
        SwizzleT<T, 2, 2, 1, 2> zzyz;
        SwizzleT<T, 2, 2, 1, 3> zzyw;
        SwizzleT<T, 2, 2, 2, 0> zzzx;
        SwizzleT<T, 2, 2, 2, 1> zzzy;
        SwizzleT<T, 2, 2, 2, 2> zzzz;
        SwizzleT<T, 2, 2, 2, 3> zzzw;
        SwizzleT<T, 2, 2, 3, 0> zzwx;
        SwizzleT<T, 2, 2, 3, 1> zzwy;
        SwizzleT<T, 2, 2, 3, 2> zzwz;
        SwizzleT<T, 2, 2, 3, 3> zzww;
        SwizzleT<T, 2, 3, 0, 0> zwxx;
        SwizzleT<T, 2, 3, 0, 1> zwxy;
        SwizzleT<T, 2, 3, 0, 2> zwxz;
        SwizzleT<T, 2, 3, 0, 3> zwxw;
        SwizzleT<T, 2, 3, 1, 0> zwyx;
        SwizzleT<T, 2, 3, 1, 1> zwyy;
        SwizzleT<T, 2, 3, 1, 2> zwyz;
        SwizzleT<T, 2, 3, 1, 3> zwyw;
        SwizzleT<T, 2, 3, 2, 0> zwzx;
        SwizzleT<T, 2, 3, 2, 1> zwzy;
        SwizzleT<T, 2, 3, 2, 2> zwzz;
        SwizzleT<T, 2, 3, 2, 3> zwzw;
        SwizzleT<T, 2, 3, 3, 0> zwwx;
        SwizzleT<T, 2, 3, 3, 1> zwwy;
        SwizzleT<T, 2, 3, 3, 2> zwwz;
        SwizzleT<T, 2, 3, 3, 3> zwww;
        SwizzleT<T, 3, 0, 0, 0> wxxx;
        SwizzleT<T, 3, 0, 0, 1> wxxy;
        SwizzleT<T, 3, 0, 0, 2> wxxz;
        SwizzleT<T, 3, 0, 0, 3> wxxw;
        SwizzleT<T, 3, 0, 1, 0> wxyx;
        SwizzleT<T, 3, 0, 1, 1> wxyy;
        SwizzleT<T, 3, 0, 1, 2> wxyz;
        SwizzleT<T, 3, 0, 1, 3> wxyw;
        SwizzleT<T, 3, 0, 2, 0> wxzx;
        SwizzleT<T, 3, 0, 2, 1> wxzy;
        SwizzleT<T, 3, 0, 2, 2> wxzz;
        SwizzleT<T, 3, 0, 2, 3> wxzw;
        SwizzleT<T, 3, 0, 3, 0> wxwx;
        SwizzleT<T, 3, 0, 3, 1> wxwy;
        SwizzleT<T, 3, 0, 3, 2> wxwz;
        SwizzleT<T, 3, 0, 3, 3> wxww;
        SwizzleT<T, 3, 1, 0, 0> wyxx;
        SwizzleT<T, 3, 1, 0, 1> wyxy;
        SwizzleT<T, 3, 1, 0, 2> wyxz;
        SwizzleT<T, 3, 1, 0, 3> wyxw;
        SwizzleT<T, 3, 1, 1, 0> wyyx;
        SwizzleT<T, 3, 1, 1, 1> wyyy;
        SwizzleT<T, 3, 1, 1, 2> wyyz;
        SwizzleT<T, 3, 1, 1, 3> wyyw;
        SwizzleT<T, 3, 1, 2, 0> wyzx;
        SwizzleT<T, 3, 1, 2, 1> wyzy;
        SwizzleT<T, 3, 1, 2, 2> wyzz;
        SwizzleT<T, 3, 1, 2, 3> wyzw;
        SwizzleT<T, 3, 1, 3, 0> wywx;
        SwizzleT<T, 3, 1, 3, 1> wywy;
        SwizzleT<T, 3, 1, 3, 2> wywz;
        SwizzleT<T, 3, 1, 3, 3> wyww;
        SwizzleT<T, 3, 2, 0, 0> wzxx;
        SwizzleT<T, 3, 2, 0, 1> wzxy;
        SwizzleT<T, 3, 2, 0, 2> wzxz;
        SwizzleT<T, 3, 2, 0, 3> wzxw;
        SwizzleT<T, 3, 2, 1, 0> wzyx;
        SwizzleT<T, 3, 2, 1, 1> wzyy;
        SwizzleT<T, 3, 2, 1, 2> wzyz;
        SwizzleT<T, 3, 2, 1, 3> wzyw;
        SwizzleT<T, 3, 2, 2, 0> wzzx;
        SwizzleT<T, 3, 2, 2, 1> wzzy;
        SwizzleT<T, 3, 2, 2, 2> wzzz;
        SwizzleT<T, 3, 2, 2, 3> wzzw;
        SwizzleT<T, 3, 2, 3, 0> wzwx;
        SwizzleT<T, 3, 2, 3, 1> wzwy;
        SwizzleT<T, 3, 2, 3, 2> wzwz;
        SwizzleT<T, 3, 2, 3, 3> wzww;
        SwizzleT<T, 3, 3, 0, 0> wwxx;
        SwizzleT<T, 3, 3, 0, 1> wwxy;
        SwizzleT<T, 3, 3, 0, 2> wwxz;
        SwizzleT<T, 3, 3, 0, 3> wwxw;
        SwizzleT<T, 3, 3, 1, 0> wwyx;
        SwizzleT<T, 3, 3, 1, 1> wwyy;
        SwizzleT<T, 3, 3, 1, 2> wwyz;
        SwizzleT<T, 3, 3, 1, 3> wwyw;
        SwizzleT<T, 3, 3, 2, 0> wwzx;
        SwizzleT<T, 3, 3, 2, 1> wwzy;
        SwizzleT<T, 3, 3, 2, 2> wwzz;
        SwizzleT<T, 3, 3, 2, 3> wwzw;
        SwizzleT<T, 3, 3, 3, 0> wwwx;
        SwizzleT<T, 3, 3, 3, 1> wwwy;
        SwizzleT<T, 3, 3, 3, 2> wwwz;
        SwizzleT<T, 3, 3, 3, 3> wwww;
    };

    constexpr VecT() noexcept = default;
    constexpr VecT(T x) noexcept : x(x) {}
    constexpr VecT(T x, T y) noexcept : x(x), y(y) {}
    constexpr VecT(T x, T y, T z) noexcept : x(x), y(y), z(z) {}
    constexpr VecT(T x, T y, T z, T w) noexcept : x(x), y(y), z(z), w(w) {}
    constexpr VecT(VecT<T, 2> xy) noexcept : VecT(xy.x, xy.y) {}
    constexpr VecT(VecT<T, 2> xy, T z) noexcept : VecT(xy.x, xy.y, z) {}
    constexpr VecT(VecT<T, 2> xy, T z, T w) noexcept : VecT(xy.x, xy.y, z, w) {}
    constexpr VecT(T x, VecT<T, 2> yz) noexcept : VecT(x, yz.y, yz.z) {}
    constexpr VecT(T x, VecT<T, 2> yz, T w) noexcept : VecT(x, yz.y, yz.z, w) {}
    constexpr VecT(T x, T y, VecT<T, 2> zw) noexcept : VecT(x, y, zw.z, zw.w) {}
    constexpr VecT(VecT<T, 3> xyz) noexcept : VecT(xyz.x, xyz.y, xyz.z) {}
    constexpr VecT(VecT<T, 3> xyz, T w) noexcept : VecT(xyz.x, xyz.y, xyz.z, w) {}
    constexpr VecT(T x, VecT<T, 3> yzw) noexcept : VecT(x, yzw.y, yzw.z, yzw.w) {}
};

template <class T, std::size_t N>
inline constexpr bool operator==(const VecT<T, N>& lhs, const VecT<T, N>& rhs) noexcept {
    return reinterpret_cast<const std::array<T, N>&>(lhs) == reinterpret_cast<const std::array<T, N>&>(rhs);
}

template <class T, std::size_t... Is>
inline constexpr bool operator==(const VecT<T, sizeof...(Is)>& v, const SwizzleT<T, Is...>& s) noexcept {
    return v == VecT<T, sizeof...(Is)>(s);
}

template <class T, std::size_t... Is>
inline constexpr bool operator==(const SwizzleT<T, Is...>& s, const VecT<T, sizeof...(Is)>& v) noexcept {
    return v == s;
}

template <class CharT, class Traits, class T, std::size_t N>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const VecT<T, N>& v) {
    os << '(' << v[0];
    for (std::size_t i = 1; i < N; ++i) {
        os << ',' << v[i];
    }
    return os << ')';
}

template <class CharT, class Traits, class T, std::size_t... Is>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const SwizzleT<T, Is...>& v) {
    return os << VecT<T, sizeof...(Is)>(v);
}

using Vec2 = VecT<float, 2>;
using Vec3 = VecT<float, 3>;
using Vec4 = VecT<float, 4>;

template <class T, std::size_t N>
struct MatT : details::VecBase<VecT<T, N>, N> {
    union {
        VecT<T, N> data[N];
    };
};

template <class CharT, class Traits, class T, std::size_t N>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const MatT<T, N>& m) {
    os << '|' << m[0][0];
    for (std::size_t j = 1; j < N; ++j) {
        os << ' ' << m[0][j];
    }
    for (std::size_t i = 1; i < N; ++i) {
        os << '|' << m[i][0];
        for (std::size_t j = 1; j < N; ++j) {
            os << ' ' << m[i][j];
        }
    }
    return os << '|';
}

using Mat2 = MatT<float, 2>;
using Mat3 = MatT<float, 3>;
using Mat4 = MatT<float, 4>;

} // namespace gm
} // namespace esl
