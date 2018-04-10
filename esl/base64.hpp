#ifndef ESL_BASE64_HPP
#define ESL_BASE64_HPP

#include "array.hpp"
#include "utility.hpp"

#include <string_view>
//#include <immintrin.h>

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
	constexpr base64_option(const char* alphabet, char pad = '=') noexcept: alphabet_(make_sized_array<64>(alphabet)), pad_(pad),
		alphabet_invert_(invert_integer_array<unsigned char, 256, 64>(alphabet, {{pad, 64}})) {}
	constexpr base64_option(const char* alphabet, bool padding) noexcept: base64_option(alphabet, padding ? '=' : '\0') {}

	// b [0, 64)
	constexpr char encode(unsigned char b) const noexcept { return alphabet_[b]; }

	constexpr bool padding() const noexcept { return pad_ != '\0'; }

	constexpr char pad() const noexcept { return pad_; }

	// c [0, 128)
	constexpr unsigned char decode(char c) const noexcept { return alphabet_invert_[static_cast<unsigned char>(c)]; }
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
inline constexpr std::size_t base64_encode(const void* in, std::size_t size, char* out,
		const base64_option& option = base64_std) noexcept {
	const unsigned char* in_b = static_cast<const unsigned char*>(in);
	char* const out_base = out;
	while (size > 2) {
		// [aaaaaabb][bbbbcccc][ccdddddd] -> [00aaaaaa][00bbbbbb][00cccccc][00dddddd]
		*out++ = option.encode(in_b[0] >> 2);
		*out++ = option.encode(((in_b[0] & 0x3) << 4) | (in_b[1] >> 4));
		*out++ = option.encode(((in_b[1] & 0xF) << 2) | (in_b[2] >> 6));
		*out++ = option.encode(in_b[2] & 0x3F);
		in_b += 3;
		size -= 3;
	}
	if (size != 0) {
		*out++ = option.encode(in_b[0] >> 2);
		if (size == 1) {
			*out++ = option.encode((in_b[0] & 0x3) << 4);
			if (option.padding()) {
				*out++ = option.pad();
				*out++ = option.pad();
			}
		} else {
			*out++ = option.encode(((in_b[0] & 0x3) << 4) | (in_b[1] >> 4));
			*out++ = option.encode(((in_b[1] & 0xF) << 2));
			if (option.padding()) {
				*out++ = option.pad();
			}
		}
	}
	return out - out_base;
}
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
	constexpr std::size_t chunk_size = 3 * 256;
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
class base64_decode_error: std::logic_error {
private:
	std::size_t pos_;

public:
	base64_decode_error(std::size_t pos): logic_error("decode error"), pos_(pos) {}

	std::size_t position() const noexcept { return pos_; }
};

// base64_try_decode
// return: output size and input decoded size
#define ESL_BASE64_DECODE_ERROR_BREAK_() --in; break
#define ESL_BASE64_DECODE_CHECK_AB_(x) if (x > 0x3F) { ESL_BASE64_DECODE_ERROR_BREAK_(); }
#define ESL_BASE64_DECODE_CHECK_AB_B_(x) if (x & 0xF) { ++in; break; }
#define ESL_BASE64_DECODE_CHECK_ABC_C_(x) if (x & 3) { ++in; break; }
inline constexpr std::pair<std::size_t, std::size_t> base64_try_decode(const char* in, std::size_t size, void* out,
		const base64_option& option = base64_std) noexcept {
	unsigned char* const out_b_base = static_cast<unsigned char*>(out);
	unsigned char* out_b = out_b_base;
	const char* const in_base = in;
	const char* const in_end = in + size;
	while (in + 1 < in_end) {
		// [00aaaaaa][00bbbbbb][00cccccc][00dddddd] -> [aaaaaabb][bbbbcccc][ccdddddd]
		const auto a = option.decode(*in++);
		ESL_BASE64_DECODE_CHECK_AB_(a); // break if X---

		const auto b = option.decode(*in++);
		ESL_BASE64_DECODE_CHECK_AB_(b); // break if aX--

		*out_b++ = (a << 2) | (b >> 4);
		if (in == in_end) { // ab => ab==
			ESL_BASE64_DECODE_CHECK_AB_B_(b);
			break; //Succ
		}
		const auto c = option.decode(*in++);
		if (c > 0x40) { // abX-
			ESL_BASE64_DECODE_ERROR_BREAK_();
		}
		if (in == in_end) {
			if (c < 0x40) { // abc => abc=
				*out_b++ = (b << 4) | (c >> 2);
				ESL_BASE64_DECODE_CHECK_ABC_C_(c);
			} else { // ab= => ab==
				ESL_BASE64_DECODE_CHECK_AB_B_(b);
			}
			break; //Succ
		}
		const auto d = option.decode(*in++);
		if (c < 0x40) {
			*out_b++ = (b << 4) | (c >> 2);
			if (d < 0x40) { // abcd
				*out_b++ = (c << 6) | d;
			} else if (d == 0x40) { // abc=
				ESL_BASE64_DECODE_CHECK_ABC_C_(c);
			} else { // abcX
				ESL_BASE64_DECODE_ERROR_BREAK_();
			}
		} else { // ab=-
			if (d == 0x40) { // ab==
				ESL_BASE64_DECODE_CHECK_AB_B_(b);
			} else { // ab=X
				ESL_BASE64_DECODE_ERROR_BREAK_();
			}
		}
	}
	return {out_b - out_b_base, in - in_base};
}
#undef ESL_BASE64_DECODE_ERROR_BREAK_
#undef ESL_BASE64_DECODE_CHECK_AB_
#undef ESL_BASE64_DECODE_CHECK_AB_B_
#undef ESL_BASE64_DECODE_CHECK_ABC_C_
// return string
inline std::pair<std::string, std::size_t> base64_try_decode(const char* in, std::size_t size,
		const base64_option& option = base64_std) {
	std::string s;
	s.resize(base64_decode_max_size(size));
	auto r = base64_try_decode(in, size, s.data(), option);
	s.resize(r.first);
	return {std::move(s), r.second};
}
// return string from string_view
inline std::pair<std::string, std::size_t> base64_try_decode(std::string_view sv, const base64_option& option = base64_std) {
	return base64_try_decode(sv.data(), sv.size(), option);
}

// base64_decode
inline constexpr std::size_t base64_decode(const char* in, std::size_t size, char* out, const base64_option& option = base64_std) {
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
// return string from string_view
inline std::string base64_decode(std::string_view sv, const base64_option& option = base64_std) {
	return base64_decode(sv.data(), sv.size(), option);
}
// ostream
template <class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>& base64_decode(std::basic_ostream<CharT, Traits>& os, std::string_view sv,
		const base64_option& option = base64_std) {
	constexpr std::size_t chunk_size = 4 * 256;
	char buf[base64_decode_max_size(chunk_size)];
	const char* in = sv.data();
	const char* const in_end = in + sv.size();
	for (; in < in_end; in += chunk_size) {
		auto in_size = std::min(static_cast<std::size_t>(in_end - in), chunk_size);
		os.write(buf, base64_decode(in, in_size, buf, option));
	}
	return os;
}

} // namespace esl

#endif //ESL_BASE64_HPP

