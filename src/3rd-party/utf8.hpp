#pragma once

#include <cinttypes>
#include <stdexcept>

#ifndef STX_UTF8_DECL
#define STX_UTF8_DECL static
#endif

namespace stx {

STX_UTF8_DECL
unsigned utf32to8(uint32_t codepoint, char* out, char** end = nullptr);
STX_UTF8_DECL
uint32_t utf8to32(const char* in, const char** out = nullptr);

} // namespace stx

// =============================================================
// == Implementation =============================================
// =============================================================

namespace stx {

namespace detail {

enum utf8_constants : uint32_t {
	utf8_1byte_max = 0x00007F,
	utf8_2byte_max = 0x0007FF,
	utf8_3byte_max = 0x00FFFF,
	utf8_4byte_max = 0x10FFFF,

	utf8_2byte_mask = 0x1F,
	utf8_3byte_mask = 0x0F,
	utf8_4byte_mask = 0x07,
	utf8_cbyte_mask = 0x3F,

	utf8_2byte_pfx = 0xC0,
	utf8_3byte_pfx = 0xE0,
	utf8_4byte_pfx = 0xF0,
	utf8_cbyte_pfx = 0x80
};

constexpr static
unsigned utf8_num_bytes[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,

	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};

} // namespace detail

STX_UTF8_DECL
unsigned utf32to8(uint32_t codepoint, char* out, char** end) {
	using namespace detail;

	if(codepoint <= utf8_1byte_max) {
		out[0] = codepoint;
		if(end) *end = out + 1;
		return 1;
	}
	else if(codepoint <= utf8_2byte_max) {
		out[0] = ((codepoint >> 6) & utf8_2byte_mask) | utf8_2byte_pfx;
		out[1] = ((codepoint >> 0) & utf8_cbyte_mask) | utf8_cbyte_pfx;
		if(end) *end = out + 2;
		return 2;
	}
	else if(codepoint <= utf8_3byte_max) {
		out[0] = ((codepoint >> 12) & utf8_3byte_mask) | utf8_3byte_pfx;
		out[1] = ((codepoint >> 6 ) & utf8_cbyte_mask) | utf8_cbyte_pfx;
		out[2] = ((codepoint >> 0 ) & utf8_cbyte_mask) | utf8_cbyte_pfx;
		if(end) *end = out + 3;
		return 3;
	}
	else if(codepoint <= utf8_4byte_max) {
		out[0] = ((codepoint >> 18) & utf8_4byte_mask) | utf8_4byte_pfx;
		out[1] = ((codepoint >> 12) & utf8_cbyte_mask) | utf8_cbyte_pfx;
		out[2] = ((codepoint >> 6 ) & utf8_cbyte_mask) | utf8_cbyte_pfx;
		out[3] = ((codepoint >> 0 ) & utf8_cbyte_mask) | utf8_cbyte_pfx;
		if(end) *end = out + 4;
		return 4;
	}
	else {
		throw std::runtime_error("Codepoint too large!");
	}

	throw std::runtime_error("Not yet implemented");
}

STX_UTF8_DECL
uint32_t utf8to32(const char* in, const char** out) {
	using namespace detail;

	unsigned num_bytes = utf8_num_bytes[(unsigned char) *in];

	if(out) *out = in + num_bytes;

	switch (num_bytes) {
		case 1: return (uint32_t) *in;
		case 2:
			return
				(uint32_t(in[0]) & utf8_2byte_mask) << 6 |
				(uint32_t(in[1]) & utf8_cbyte_mask) << 0;
		case 3:
			return
				(uint32_t(in[0]) & utf8_3byte_mask) << 12 |
				(uint32_t(in[1]) & utf8_cbyte_mask) << 6  |
				(uint32_t(in[2]) & utf8_cbyte_mask) << 0;
		case 4:
			return
				(uint32_t(in[0]) & utf8_4byte_mask) << 18 |
				(uint32_t(in[1]) & utf8_cbyte_mask) << 12 |
				(uint32_t(in[2]) & utf8_cbyte_mask) << 6  |
				(uint32_t(in[3]) & utf8_cbyte_mask) << 0;
	}

	throw std::runtime_error("Not fully implemented");
}

} // namespace stx
