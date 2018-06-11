#include "base64.hpp"
//#include <immintrin.h>

namespace esl {

std::size_t base64_encode(const void* in, std::size_t size, char* out,
		const base64_option& option) noexcept {
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

#define ESL_BASE64_DECODE_ERROR_BREAK_() --in; break
#define ESL_BASE64_DECODE_CHECK_AB_(x) if (x > 0x3F) { ESL_BASE64_DECODE_ERROR_BREAK_(); }
#define ESL_BASE64_DECODE_CHECK_AB_B_(x) if (x & 0xF) { ++in; break; }
#define ESL_BASE64_DECODE_CHECK_ABC_C_(x) if (x & 3) { ++in; break; }
std::pair<std::size_t, std::size_t> base64_try_decode(const char* in, std::size_t size, void* out,
		const base64_option& option) noexcept {
	unsigned char* const out_b_base = static_cast<unsigned char*>(out);
	unsigned char* out_b = out_b_base;
	const char* const in_base = in;
	const char* const in_end = in + size;
	while (in < in_end) {
		// [00aaaaaa][00bbbbbb][00cccccc][00dddddd] -> [aaaaaabb][bbbbcccc][ccdddddd]
		const auto a = option.decode(*in++);
		ESL_BASE64_DECODE_CHECK_AB_(a); // break if X---
		if (in == in_end) { // a---
			++in;
			break;
		}
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

} // namespace esl

