#pragma once

#include <cinttypes>

namespace widget {

constexpr inline
uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, float a) noexcept {
	return (uint32_t(r) << 16) |
	       (uint32_t(g) << 8)  |
	       (uint32_t(b) << 0)  |
				 (uint32_t(0xFF * a) << 24);
}

constexpr inline
uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) noexcept {
	return rgba(r, g, b, 1);
}

class Canvas {
public:
	virtual void fillRect   (float x, float y, float w, float h, uint32_t color) = 0;
	virtual void outlineRect(float x, float y, float w, float h, uint32_t color) = 0;

	virtual void fillRRect   (float radius, float degree, float x, float y, float w, float h, uint32_t color) = 0;
	virtual void outlineRRect(float radius, float degree, float x, float y, float w, float h, uint32_t color) = 0;
};

} // namespace widget
