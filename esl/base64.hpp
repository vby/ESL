#ifndef ESL_BASE64_HPP
#define ESL_BASE64_HPP

#include "array.hpp"
#include "utility.hpp"

namespace esl {

// base64_alphabet_(std|urlsafe)
inline constexpr const char* base64_alphabet_std = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
inline constexpr const char* base64_alphabet_urlsafe = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

class base64_option {
private:
    std::array<char, 64> alphabet_;
    char pad_;
    std::array<unsigned char, 256> alphabet_invert_;

public:
    constexpr base64_option(const char* alphabet, char pad = '=') noexcept
        : alphabet_(make_sized_array<64>(alphabet)), pad_(pad), alphabet_invert_(invert_integer_array<unsigned char, 256, 64>(alphabet, {{pad, 64}})) {}
    constexpr base64_option(const char* alphabet, bool padding) noexcept : base64_option(alphabet, padding ? '=' : '\0') {}

    // b [0, 64)
    constexpr char encode(unsigned char b) const noexcept {
        return alphabet_[b];
    }

    constexpr bool padding() const noexcept {
        return pad_ != '\0';
    }

    constexpr char pad() const noexcept {
        return pad_;
    }

    // c [0, 256)
    constexpr unsigned char decode(char c) const noexcept {
        return alphabet_invert_[static_cast<unsigned char>(c)];
    }
};

inline constexpr base64_option base64_std{base64_alphabet_std};
inline constexpr base64_option base64_std_npad{base64_alphabet_std, false};
inline constexpr base64_option base64_urlsafe{base64_alphabet_urlsafe};
inline constexpr base64_option base64_urlsafe_npad{base64_alphabet_std, false};

// base64_encode_size
// return: Exact encode size
inline constexpr std::size_t base64_encode_size(std::size_t size, bool padding) noexcept {
    if (padding) {
        return (size + 2) / 3 * 4;
    } else {
        const auto r = size % 3;
        return size / 3 * 4 + (r << 1) - (r >> 1);
    }
}

// base64_encode
// out: at least `base64_encode_size' size
// return: encode size
std::size_t base64_encode(const void* in, std::size_t size, char* out, const base64_option& option = base64_std) noexcept;

// string
inline std::string base64_encode(const void* data, std::size_t size, const base64_option& option = base64_std) {
    std::string s;
    s.resize(base64_encode_size(size, option.padding()));
    base64_encode(data, size, s.data(), option);
    return s;
}
// ostream
template <class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>& base64_encode(std::basic_ostream<CharT, Traits>& os, const void* data, std::size_t size,
                                                        const base64_option& option = base64_std) {
    const unsigned char* bytes = static_cast<const unsigned char*>(data);
    constexpr std::size_t chunk_size = 3 * 128;
    char buf[base64_encode_size(chunk_size, true)];
    for (std::size_t i = 0; i < size; i += chunk_size) {
        os.write(buf, base64_encode(bytes + i, std::min(size - i, chunk_size), buf, option));
    }
    return os;
}

// base64_decode_max_size
// return: decode size if input has padding
inline constexpr std::size_t base64_decode_max_size(std::size_t size) noexcept {
    const auto r = size % 4;
    return (size / 4) * 3 + r - (r / 2);
}

// base64_decode_error
class base64_decode_error : std::logic_error {
private:
    std::size_t pos_;

public:
    base64_decode_error(std::size_t pos) : logic_error("decode error"), pos_(pos) {}

    std::size_t position() const noexcept {
        return pos_;
    }
};

// base64_try_decode
// return: output size and input decoded size
std::pair<std::size_t, std::size_t> base64_try_decode(const char* in, std::size_t size, void* out, const base64_option& option = base64_std) noexcept;

// return string
inline std::pair<std::string, std::size_t> base64_try_decode(const char* in, std::size_t size, const base64_option& option = base64_std) {
    std::string s;
    s.resize(base64_decode_max_size(size));
    auto r = base64_try_decode(in, size, s.data(), option);
    s.resize(r.first);
    return {std::move(s), r.second};
}

// base64_decode
inline std::size_t base64_decode(const char* in, std::size_t size, char* out, const base64_option& option = base64_std) {
    auto r = base64_try_decode(in, size, out, option);
    if (r.second != size) {
        throw base64_decode_error(r.second);
    }
    return r.first;
}
// return string
inline std::string base64_decode(const char* in, std::size_t size, const base64_option& option = base64_std) {
    std::string s;
    s.resize(base64_decode_max_size(size));
    s.resize(base64_decode(in, size, s.data(), option));
    return s;
}
// ostream
template <class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>& base64_decode(std::basic_ostream<CharT, Traits>& os, const char* in, std::size_t size,
                                                        const base64_option& option = base64_std) {
    constexpr std::size_t chunk_size = 4 * 128;
    char buf[base64_decode_max_size(chunk_size)];
    const char* const in_end = in + size;
    for (; in < in_end; in += chunk_size) {
        const auto n = std::min(static_cast<std::size_t>(in_end - in), chunk_size);
        os.write(buf, base64_decode(in, n, buf, option));
    }
    return os;
}

} // namespace esl

#endif //ESL_BASE64_HPP
