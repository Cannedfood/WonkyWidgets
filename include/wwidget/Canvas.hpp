#pragma once

#include <cinttypes>
#include <memory>
#include <deque>
#include <functional>

#include "Bitmap.hpp"

namespace wwidget {

class Widget;
class Font;
class Owner;

struct Color {
	union {
		struct {
			float r, g, b, a;
		};
		float rgba[4];
	};

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

	constexpr static Color white() noexcept { return {1, 1, 1, 1}; }
	constexpr static Color black() noexcept { return {0, 0, 0, 1}; }
	constexpr static Color eyecancer1() noexcept { return {1, 0, 1, 1}; }
	constexpr static Color eyecancer2() noexcept { return {0, 1, 0, 1}; }
};

constexpr inline
Color rgba(uint8_t r, uint8_t g, uint8_t b, float a) noexcept {
	return Color(r / 255.f, g / 255.f, b / 255.f, a);
}
constexpr inline
Color rgb(uint8_t r, uint8_t g, uint8_t b) noexcept {
	return Color(r / 255.f, g / 255.f, b / 255.f, 1);
}

struct Point {
	union {
		struct { float x, y; };
		float xy[2];
	};

	constexpr
	Point() : x(0), y(0) {}

	constexpr
	Point(float x, float y) : x(x), y(y) {}
};

struct Rect {
	union {
		struct {
			Point min;
			Point max;
		};
		struct {
			float x0, y0, x1, y1;
		};
	};

	constexpr
	Rect(float x, float y, float w, float h) :
		x0(x), y0(y), x1(x + w), y1(y + h)
	{}

	constexpr
	Rect(float w, float h) :
		Rect(0, 0, w, h)
	{}

	constexpr
	Rect() :
		x0(0), y0(0), x1(0), y1(0)
	{}

	constexpr static inline
	Rect absolute(float x0, float y0, float x1, float y1) {
		Rect result;
		result.x0 = x0;
		result.y0 = y0;
		result.x1 = x1;
		result.y1 = y1;
		return result;
	}
};

class Canvas {
protected:
	inline std::shared_ptr<void>& proxyRef(std::shared_ptr<Bitmap> const& b) {
		return b->mRendererProxy;
	}
public:
	virtual void pushViewport(float x, float y, float w, float h) = 0;
	virtual void popViewport() = 0;

	virtual void pushClipRect(float x, float y, float w, float h) = 0;
	virtual void popClipRect() = 0;

	virtual void recommendUpload(std::weak_ptr<Bitmap> bm) = 0;

	virtual void rect(
		Rect const& area,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) = 0;

	virtual void rect(
		Rect const& area,
		Rect const& subarea,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) = 0;

	virtual void rect(
		Rect const& area,
		Color const& color) = 0;

	virtual void rects(
		size_t num,
		Rect const* areas,
		Rect const* texareas,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()
	) = 0;

	virtual void rect(
		float corner_radius,
		Rect const& area,
		Color const& color) = 0;

	virtual void box(
		Rect const& area,
		Color const& color) = 0;

	virtual void box(
		float corner_radius,
		Rect const& area,
		Color const& color) = 0;

	virtual void polygon( // Solid color
		size_t num, Point const* points, Color const& color
	) = 0;
	virtual void polygon( // Vertex color
		size_t num, Point const* points, Color const* color
	) = 0;
	virtual void polygon( // Solid color texture
		size_t num, Point const* points,
		std::shared_ptr<Bitmap> const& bm, Color const& tint = Color::white()
	) = 0;
	virtual void polygon( // Solid color texture w/ texcoords
		size_t num, Point const* points, Point const* texcoords,
		std::shared_ptr<Bitmap> const& bm, Color const& tint = Color::white()
	) = 0;
	virtual void linestrip(
		size_t num, Point const* points, Color const& color
	) = 0;
	virtual void lineloop(
		size_t num, Point const* points, Color const& color
	) = 0;
};

} // namespace wwidget
