#pragma once

#include "../Canvas.hpp"
#include "../async/Queue.hpp"

#include <GL/gl.h>
#include <cmath>

namespace wwidget {

class OpenGL1_Canvas final : public Canvas {
	TaskQueue mQueue;

	void bindBitmap(std::shared_ptr<Bitmap> const& b);

	std::vector<Offset> mOffsets;
	std::vector<Rect>   mClipRects;
	void updateMatrix();
public:
	void pushViewport(float x, float y, float w, float h) override;
	void popViewport() override;
	void pushClipRect(float x, float y, float w, float h) override;
	void popClipRect() override;
	void recommendUpload(std::weak_ptr<Bitmap> bm) override;
	void rect(
		Rect const& area,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) override;

	void rect(
		Rect const& area,
		Rect const& texarea,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) override;

	void rect(
		Rect const& area,
		Color const& color) override;

	void rect(
		float corner_radius,
		Rect const& area,
		Color const& color) override;

	void rects(
		size_t num,
		Rect const* areas,
		Rect const* texareas,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) override;

	void box(
		Rect const& area,
		Color const& color) override;

	void box(
		float corner_radius,
		Rect const& area,
		Color const& color) override;

	void polygon( // Solid color
		size_t num, Point const* points, Color const& color) override;
	void polygon( // Vertex color
		size_t num, Point const* points, Color const* color) override;
	void polygon( // Solid color texture w/ texcoords
		size_t num, Point const* points, Point const* texcoords,
		std::shared_ptr<Bitmap> const& bm, Color const& tint = Color::white()) override;
	void lines(
		size_t num_points, Point const* points, Color const& color) override;
	void linestrip(
		size_t num, Point const* points, Color const& color) override;
	void lineloop(
		size_t num, Point const* points, Color const& color) override;
};

} // namespace wwidget
