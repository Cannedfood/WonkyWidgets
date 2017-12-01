#pragma once

#include <cinttypes>
#include <memory>
#include <deque>
#include <functional>

#include "Bitmap.hpp"

namespace widget {

class Widget;
class Font;
class Owner;

struct Color {
	float r, g, b, a;

	constexpr inline
	Color(float r, float g, float b, float a = 1) noexcept :
		r(r), g(g), b(b), a(a)
	{}

	constexpr inline
	Color(float f = 0) noexcept :
		Color(f, f, f, 1)
	{}

	constexpr inline
	Color(uint32_t color) noexcept :
		r((color >> 16) / 255.f),
		g((color >> 8)  / 255.f),
		b((color >> 0)  / 255.f),
		a((color >> 24) / 255.f)
	{}

	constexpr inline
	operator uint32_t() const noexcept {
		return (uint32_t(0xFF * r) << 16) |
		       (uint32_t(0xFF * g) << 8)  |
		       (uint32_t(0xFF * b) << 0)  |
					 (uint32_t(0xFF * a) << 24);
	}
};

constexpr inline
Color rgba(uint8_t r, uint8_t g, uint8_t b, float a) noexcept {
	return Color(r / 255.f, g / 255.f, b / 255.f, a);
}
constexpr inline
Color rgb(uint8_t r, uint8_t g, uint8_t b) noexcept {
	return Color(r / 255.f, g / 255.f, b / 255.f, 1);
}

class Canvas {
private:
	struct Cache;

	Cache* mCache;
protected:
	Canvas();
public:
	virtual ~Canvas();

	// Load texture synchronously.
	virtual std::shared_ptr<Bitmap> loadTextureNow(uint8_t const* data, unsigned w, unsigned h, unsigned components) = 0;
	// Load texture synchronously, prefer loadTexture when possible
	virtual std::shared_ptr<Bitmap> loadTextureNow(std::string const& path);
	// Load texture asynchronously. Task is interrupted if the widget is destroyed. Returns true if the texture was when the function exited (e.g. because it was cached).
	virtual bool loadTexture(Owner* task_owner, std::string const& path, std::function<void(std::shared_ptr<Bitmap>&&)>&& to);
	// Load texture asynchronously. Task is interrupted if the widget is destroyed. Returns true if the texture was when the function exited (e.g. because it was cached).
	virtual bool loadTexture(Owner* task_owner, std::string const& path, std::shared_ptr<Bitmap>& to);

	virtual std::shared_ptr<Font> loadFontNow(std::string const& font);
	virtual bool loadFont(Owner* task_owner, std::string const& font, std::function<void(std::shared_ptr<Font>&&)>&& to);
	virtual bool loadFont(Owner* task_owner, std::string const& font, std::shared_ptr<Font>& to);

	virtual void begin(float x, float y, float w, float h);
	virtual void end();

	virtual void pushArea(float x, float y, float w, float h) = 0;
	virtual void popArea() = 0;

	virtual void outlinePoly(float* points, size_t number, uint32_t color) = 0;
	virtual void fillPoly(float* points, size_t number, uint32_t color) = 0;
	virtual void fillPoly(float* points, float* texcoords, size_t number, std::shared_ptr<Bitmap> const& bitmap, uint32_t tint) = 0;

	virtual void fillRects(float* coords, float* texcoords, size_t number, std::shared_ptr<Bitmap> const& bm, uint32_t tint) = 0; //<! Used for font rendering

	virtual void outlineRect(float x, float y, float w, float h, uint32_t color);
	virtual void fillRect   (float x, float y, float w, float h, uint32_t color);
	virtual void fillRect   (float x, float y, float w, float h, std::shared_ptr<Bitmap> const& bitmap, uint32_t tint = ~uint32_t(0), float srcx = 0, float srcy = 0, float srcw = -1, float srch = -1);

	virtual void outlineRRect(float radius, float degree, float x, float y, float w, float h, uint32_t color);
	virtual void fillRRect   (float radius, float degree, float x, float y, float w, float h, uint32_t color);
	virtual void fillRRect   (float radius, float degree, float x, float y, float w, float h, std::shared_ptr<Bitmap> const& bitmap, uint32_t tint = ~uint32_t(0), float srcx = 0, float srcy = 0, float srcw = -1, float srch = -1);
};

} // namespace widget
