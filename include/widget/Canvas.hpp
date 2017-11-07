#pragma once

#include <cinttypes>
#include <memory>
#include <deque>
#include <functional>

#include "Bitmap.hpp"

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
private:
	class Cache;

	Cache* mCache;
protected:
	Canvas();
public:
	virtual ~Canvas();

	// Load texture synchronously
	virtual std::shared_ptr<Bitmap> loadTextureNow(uint8_t* data, unsigned w, unsigned h, unsigned components) = 0;
	// Load texture synchronously, prefer loadTexture when possible
	virtual std::shared_ptr<Bitmap> loadTextureNow(std::string const& path);

	virtual void pushArea(float x, float y, float w, float h) = 0;
	virtual void popArea() = 0;

	virtual void outlinePoly(float* points, size_t number, uint32_t color) = 0;
	virtual void fillPoly(float* points, size_t number, uint32_t color) = 0;
	virtual void fillPoly(float* points, float* texcoords, size_t number, Bitmap* bitmap, uint32_t tint) = 0;

	virtual void outlineRect(float x, float y, float w, float h, uint32_t color);
	virtual void fillRect   (float x, float y, float w, float h, uint32_t color);
	virtual void fillRect   (float x, float y, float w, float h, Bitmap* bitmap, uint32_t tint = ~uint32_t(0), float srcx = 0, float srcy = 0, float srcw = -1, float srch = -1);

	virtual void outlineRRect(float radius, float degree, float x, float y, float w, float h, uint32_t color);
	virtual void fillRRect   (float radius, float degree, float x, float y, float w, float h, uint32_t color);
	virtual void fillRRect   (float radius, float degree, float x, float y, float w, float h, Bitmap* bitmap, uint32_t tint = ~uint32_t(0), float srcx = 0, float srcy = 0, float srcw = -1, float srch = -1);
};

} // namespace widget
