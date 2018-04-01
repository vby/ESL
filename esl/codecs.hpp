#ifndef ESL_CODECS_HPP
#define ESL_CODECS_HPP

#include "utility.hpp"

#include <string_view>
//#include <immintrin.h>

namespace esl {

class base64 {
public:
	static constexpr char encode_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	static constexpr char pad = '=';

	static constexpr auto decode_codes = transpose_integer_array<unsigned char, 256>(encode_chars, {{'=', 64}});

	static constexpr std::size_t encode_size(std::size_t size, bool padding=true) noexcept {
		if (padding) {
			return (size + 2) / 3 * 4;
		} else {
			const auto r = size % 3;
			return size / 3 * 4 + (r << 1) - (r >> 1);
		}
	}

	// without '\0'
	static std::size_t encode(const void* in, std::size_t size, char* out, bool padding=true) noexcept {
		const unsigned char* in_b = static_cast<const unsigned char*>(in);
		char* const out_base = out;
		while (size > 2) {
			// [aaaaaabb][bbbbcccc][ccdddddd] -> [00aaaaaa][00bbbbbb][00cccccc][00dddddd]
			*out++ = encode_chars[in_b[0] >> 2];
			*out++ = encode_chars[((in_b[0] & 0x3) << 4) | (in_b[1] >> 4)];
			*out++ = encode_chars[((in_b[1] & 0xF) << 2) | (in_b[2] >> 6)];
			*out++ = encode_chars[in_b[2] & 0x3F];
			in_b += 3;
			size -= 3;
		}
		if (size != 0) {
			*out++ = encode_chars[in_b[0] >> 2];
			if (size == 1) {
				*out++ = encode_chars[(in_b[0] & 0x3) << 4];
				if (padding) {
					*out++ = pad;
					*out++ = pad;
				}
			} else {
				*out++ = encode_chars[((in_b[0] & 0x3) << 4) | (in_b[1] >> 4)];
				*out++ = encode_chars[((in_b[1] & 0xF) << 2)];
				if (padding) {
					*out++ = pad;
				}
			}
		}
		return out - out_base;
	}

	static std::string encode(const void* in, std::size_t size, bool padding=true) noexcept {
		std::string s;
		s.resize(encode_size(size, padding));
		encode(in, size, s.data(), padding);
		return s;
	}

	static constexpr std::size_t decode_max_size(std::size_t size) noexcept {
		const auto r = size % 4;
		return (size / 4) * 3 + r - (r / 2);
	}

	#define ESL_BASE64_DECODE_ERROR_BREAK_(in) --in; break
	#define ESL_BASE64_DECODE_CHECK_AB_(in, x) if (x > 0x3F) { ESL_BASE64_DECODE_ERROR_BREAK_(in); }
	#define ESL_BASE64_DECODE_CHECK_AB_B_(in, x) if (x & 0xF) { ESL_BASE64_DECODE_ERROR_BREAK_(in); }
	#define ESL_BASE64_DECODE_CHECK_ABC_C_(in, x) if (x & 3) { ESL_BASE64_DECODE_ERROR_BREAK_(in); }

	static std::pair<std::size_t, std::size_t> decode(const char* in, std::size_t size, void* out) noexcept {
		unsigned char* const out_b_base = static_cast<unsigned char*>(out);
		unsigned char* out_b = out_b_base;
		const unsigned char* in_c = reinterpret_cast<const unsigned char*>(in);
		const unsigned char* const in_c_base = in_c;
		const unsigned char* const in_c_end = in_c_base + size;
		while (in_c + 1 < in_c_end) {
			// [00aaaaaa][00bbbbbb][00cccccc][00dddddd] -> [aaaaaabb][bbbbcccc][ccdddddd]
			const auto a = decode_codes[*in_c++];
			ESL_BASE64_DECODE_CHECK_AB_(in_c, a); // break if X---

			const auto b = decode_codes[*in_c++];
			ESL_BASE64_DECODE_CHECK_AB_(in_c, b); // break if aX--

			*out_b++ = (a << 2) | (b >> 4);
			if (in_c == in_c_end) { // ab => ab==
				ESL_BASE64_DECODE_CHECK_AB_B_(in_c, b);
				break; //Succ
			}
			const auto c = decode_codes[*in_c++];
			if (c > 0x40) { // abX-
				ESL_BASE64_DECODE_ERROR_BREAK_(in_c);
			}
			if (in_c == in_c_end) {
				if (c < 0x40) { // abc => abc=
					*out_b++ = (b << 4) | (c >> 2);
					ESL_BASE64_DECODE_CHECK_ABC_C_(in_c, c);
				} else { // ab= => ab==
					ESL_BASE64_DECODE_CHECK_AB_B_(in_c, b);
				}
				break; //Succ
			}
			const auto d = decode_codes[*in_c++];
			if (c < 0x40) {
				*out_b++ = (b << 4) | (c >> 2);
				if (d < 0x40) { // abcd
					*out_b++ = (c << 6) | d;
				} else if (d == 0x40) { // abc=
					ESL_BASE64_DECODE_CHECK_ABC_C_(in_c, c);
				} else { // abcX
					ESL_BASE64_DECODE_ERROR_BREAK_(in_c);
				}
			} else { // ab=-
				if (d == 0x40) { // ab==
					ESL_BASE64_DECODE_CHECK_AB_B_(in_c, b);
				} else { // ab=X
					ESL_BASE64_DECODE_ERROR_BREAK_(in_c);
				}
			}
		}
		return std::make_pair(out_b - out_b_base, in_c - in_c_base);
	}

	#undef ESL_BASE64_DECODE_ERROR_BREAK_
	#undef ESL_BASE64_DECODE_CHECK_AB_
	#undef ESL_BASE64_DECODE_CHECK_AB_B_
	#undef ESL_BASE64_DECODE_CHECK_ABC_C_

	static std::pair<std::string, std::size_t> decode(const char* in, std::size_t size) noexcept {
		std::string s;
		s.resize(decode_max_size(size));
		auto r = decode(in, size, s.data());
		s.resize(r.first);
		return std::make_pair(std::move(s), r.second);
	}

	static std::pair<std::string, std::size_t> decode(const std::string_view& sv) noexcept {
		return decode(sv.data(), sv.size());
	}
};

} // namespace esl

#endif //ESL_CODECS_HPP

