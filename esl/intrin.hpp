#ifndef ESL_INTRIN_HPP
#define ESL_INTRIN_HPP

#include "limits.hpp"
#include "macros.hpp"

#include <type_traits>

namespace esl {

// load16le
ESL_ATTR_FORCEINLINE constexpr std::uint16_t load16le(unsigned char a, unsigned char b) noexcept {
    return static_cast<std::uint16_t>(a) | (static_cast<std::uint16_t>(b) << 8);
}
ESL_ATTR_FORCEINLINE constexpr std::uint16_t load16le(const unsigned char* bs) noexcept {
    return load16le(bs[0], bs[1]);
}

// load32le
ESL_ATTR_FORCEINLINE constexpr std::uint32_t load32le(unsigned char a, unsigned char b, unsigned char c, unsigned char d) noexcept {
    return static_cast<std::uint32_t>(a) | (static_cast<std::uint32_t>(b) << 8) | (static_cast<std::uint32_t>(c) << 16) | (static_cast<std::uint32_t>(d) << 24);
}
ESL_ATTR_FORCEINLINE constexpr std::uint32_t load32le(const unsigned char* bs) noexcept {
    return load32le(bs[0], bs[1], bs[2], bs[3]);
}

// load64le
ESL_ATTR_FORCEINLINE constexpr std::uint64_t load64le(const unsigned char* bs) noexcept {
    return static_cast<std::uint64_t>(load32le(bs)) | (static_cast<std::uint64_t>(load32le(bs + 4)) << 32);
}

// store16le
ESL_ATTR_FORCEINLINE constexpr void store16le(std::uint16_t u16, unsigned char* bs) noexcept {
    bs[0] = static_cast<unsigned char>(u16 & 0xFF);
    bs[1] = static_cast<unsigned char>((u16 >> 8) & 0xFF);
}

// store32le
ESL_ATTR_FORCEINLINE constexpr void store32le(std::uint32_t u32, unsigned char* bs) noexcept {
    bs[0] = static_cast<unsigned char>(u32 & 0xFF);
    bs[1] = static_cast<unsigned char>((u32 >> 8) & 0xFF);
    bs[2] = static_cast<unsigned char>((u32 >> 16) & 0xFF);
    bs[3] = static_cast<unsigned char>((u32 >> 24) & 0xFF);
}

// store64le
ESL_ATTR_FORCEINLINE constexpr void store64le(std::uint64_t u64, unsigned char* bs) noexcept {
    store32le(static_cast<std::uint32_t>(u64 & 0xFFFFFFFF), bs);
    store32le(static_cast<std::uint32_t>(u64 >> 32), bs + 4);
}

// rotl
template <class T>
ESL_ATTR_FORCEINLINE constexpr T rotl(T x, unsigned char s) noexcept {
    return ((x << s) | (x >> (sizeof(T) * 8 - s)));
}

// rotr
template <class T, class = std::enable_if_t<std::is_unsigned_v<T>>>
ESL_ATTR_FORCEINLINE constexpr T rotr(T x, unsigned char s) noexcept {
    return ((x >> s) | (x << (sizeof(T) * 8 - s)));
}
// clz
template <class T, class = std::enable_if_t<std::is_unsigned_v<T>>>
ESL_ATTR_FORCEINLINE unsigned char clz(T x) noexcept {
    if constexpr (sizeof(T) == 4) {
#ifdef ESL_COMPILER_MSVC
        return __lzcnt(x);
#else
        return static_cast<unsigned char>(__builtin_clz(x));
#endif
    } else if constexpr (sizeof(T) == 8) {
#ifdef ESL_COMPILER_MSVC
        return __lzcnt64(x);
#else
        return static_cast<unsigned char>(__builtin_clzll(x));
#endif
    }
}

} // namespace esl

#endif // ESL_INTRIN_HPP
