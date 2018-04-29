#pragma once

#include <cinttypes>
#include <memory>
#include <deque>
#include <functional>

#include "Bitmap.hpp"

#include "Attributes.hpp"

namespace wwidget {

class Widget;
class Font;
class Owner;

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
		size_t num_points, Point const* points, Color const& color
	) = 0;
	virtual void polygon( // Vertex color
		size_t num_points, Point const* points, Color const* color
	) = 0;
	virtual void polygon( // Solid color texture w/ texcoords
		size_t num_points, Point const* points, Point const* texcoords,
		std::shared_ptr<Bitmap> const& bm, Color const& tint = Color::white()
	) = 0;
	virtual void lines(
		size_t num_points, Point const* points, Color const& color
	) = 0;
	virtual void linestrip(
		size_t num_points, Point const* points, Color const& color
	) = 0;
	virtual void lineloop(
		size_t num_points, Point const* points, Color const& color
	) = 0;
};

} // namespace wwidget
