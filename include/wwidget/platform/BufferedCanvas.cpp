#include "BufferedCanvas.hpp"

namespace wwidget {

static inline
Point transform(BufferedCanvas::Transform const& t, Point const& v) {
	return {
		t.offsetx + t.scalex * v.x,
		t.offsety + t.scaley * v.y
	};
}
static inline
BufferedCanvas::Transform transform(
	BufferedCanvas::Transform const& a,
	BufferedCanvas::Transform const& b)
{
	return {
		a.offsetx + a.scalex * b.offsetx,
		a.offsety + a.scaley * b.offsety,
		a.scalex * b.scalex,
		a.scaley * b.scaley,
	};
}

BufferedCanvas::BufferedCanvas()
{
	mTransforms.emplace_back(); // We need
}

CanvasBuffers BufferedCanvas::poll()
{
	return std::move(mBuffers);
}

void BufferedCanvas::pushTransform(Transform const& t)
{
	mTransforms.push_back(transform(mTransforms.back(), t));
}
void BufferedCanvas::popTransform()
{
	mTransforms.pop_back();
}

void BufferedCanvas::pushViewport(float x, float y, float w, float h)
{
	pushTransform({
		x, y, 1.f / w, 1.f / h
	});
}
void BufferedCanvas::popViewport()
{
	popTransform();
}

void BufferedCanvas::pushClipRect(float x, float y, float w, float h)
{
	pushTransform({x, y, 1, 1});
}
void BufferedCanvas::popClipRect()
{
	popTransform();
}

void BufferedCanvas::recommendUpload(std::weak_ptr<Bitmap> bm)
{
	// Ignored.
}

void BufferedCanvas::rect(
	Rect const& area,
	std::shared_ptr<Bitmap> const& bm,
	Color const& tint)
{
	rect(area, Rect(bm->width(), bm->height()), bm, tint);
}

void BufferedCanvas::rect(
	Rect const& area,
	Rect const& subarea,
	std::shared_ptr<Bitmap> const& bm,
	Color const& tint)
{
	Point points[4] = {
		{ area.min.x, area.max.y },
		{ area.max.x, area.max.y },
		{ area.max.x, area.min.y },
		{ area.min.x, area.min.y }
	};
	Point texcoords[4] = {
		{ subarea.min.x, subarea.max.y },
		{ subarea.max.x, subarea.max.y },
		{ subarea.max.x, subarea.min.y },
		{ subarea.min.x, subarea.min.y }
	};
	polygon(4, points, texcoords, bm, tint);
}

void BufferedCanvas::rect(
	Rect const& area,
	Color const& color)
{
	Point points[4] = {
		{ area.min.x, area.max.y },
		{ area.max.x, area.max.y },
		{ area.max.x, area.min.y },
		{ area.min.x, area.min.y }
	};
	polygon(4, points, color);
}

void BufferedCanvas::rects(
	size_t num,
	Rect const* areas,
	Rect const* texareas,
	std::shared_ptr<Bitmap> const& bm,
	Color const& tint)
{
	for(size_t i = 0; i < num; i++) {
		rect(areas[i], texareas[i], bm, tint);
	}
}

void BufferedCanvas::rect(
	float corner_radius,
	Rect const& area,
	Color const& color)
{
	// TODO: Rounded rects
	rect(area, color);
}

void BufferedCanvas::box(
	Rect const& area,
	Color const& color)
{
	Point points[4] = {
		{ area.min.x, area.max.y },
		{ area.max.x, area.max.y },
		{ area.max.x, area.min.y },
		{ area.min.x, area.min.y }
	};
	lineloop(4, points, color);
}

void BufferedCanvas::box(
	float corner_radius,
	Rect const& area,
	Color const& color)
{
	// TODO: Round corners
	box(area, color);
}

void BufferedCanvas::polygon( // Solid color
	size_t num, Point const* points, Color const& color)
{
	auto&    buf = mBuffers.triangles;
	uint32_t idx = buf.vertices.size();
	buf.vertices.reserve(num);
	for(size_t i = 0; i < num; i++) {
		buf.vertices.push_back({
			transform(mTransforms.back(), points[i]),
			color
		});
	}
	buf.indices.reserve(3 * (num - 1));
	for(size_t i = 1; i < num - 1; i++) {
		buf.indices.emplace_back(idx + 0 + 0);
		buf.indices.emplace_back(idx + i + 0);
		buf.indices.emplace_back(idx + i + 1);
	}
	// TODO: Check
}
void BufferedCanvas::polygon( // Vertex color
	size_t num, Point const* points, Color const* color)
{
	auto&    buf = mBuffers.triangles;
	uint32_t idx = buf.vertices.size();
	buf.vertices.reserve(num);
	for(size_t i = 0; i < num; i++) {
		buf.vertices.push_back({
			transform(mTransforms.back(), points[i]),
			color[i]
		});
	}
	buf.indices.reserve(3 * (num - 1));
	for(size_t i = 1; i < num - 1; i++) {
		buf.indices.emplace_back(idx + 0 + 0);
		buf.indices.emplace_back(idx + i + 0);
		buf.indices.emplace_back(idx + i + 1);
	}
	// TODO: Check
}
void BufferedCanvas::polygon( // Solid color texture w/ texcoords
	size_t num, Point const* points, Point const* texcoords,
	std::shared_ptr<Bitmap> const& bm, Color const& tint)
{
	auto&    buf = mBuffers.textured[bm];
	uint32_t idx = buf.vertices.size();
	buf.vertices.reserve(num);
	for(size_t i = 0; i < num; i++) {
		buf.vertices.push_back({
			transform(mTransforms.back(), points[i]),
			texcoords[i],
			tint
		});
	}
	buf.indices.reserve(3 * (num - 1));
	for(size_t i = 1; i < num - 1; i++) {
		buf.indices.emplace_back(idx + 0 + 0);
		buf.indices.emplace_back(idx + i + 0);
		buf.indices.emplace_back(idx + i + 1);
	}
	// TODO: Check
}
void BufferedCanvas::linestrip(
	size_t num, Point const* points, Color const& color)
{
	auto& buf = mBuffers.lines;
	uint32_t idx = buf.vertices.size();
	buf.vertices.reserve(num);
	for(uint32_t i = 0; i < num; i++) {
		buf.vertices.push_back({
			transform(mTransforms.back(), points[i]),
			color
		});
	}
	buf.indices.reserve(num * 2);
	for(uint32_t i = 0; i < num - 1; i++) {
		buf.indices.emplace_back(idx + i + 0);
		buf.indices.emplace_back(idx + i + 1);
	}
	// TODO: Check
}
void BufferedCanvas::lineloop(
	size_t num, Point const* points, Color const& color)
{
	auto& buf = mBuffers.lines;
	uint32_t idx = buf.vertices.size();
	buf.vertices.reserve(num);
	for(uint32_t i = 0; i < num; i++) {
		buf.vertices.push_back({
			transform(mTransforms.back(), points[i]),
			color
		});
	}
	buf.indices.reserve(num * 2 + 2);
	for(uint32_t i = 0; i < num - 1; i++) {
		buf.indices.emplace_back(idx + i + 0);
		buf.indices.emplace_back(idx + i + 1);
	}
	buf.indices.emplace_back(idx + num - 1);
	buf.indices.emplace_back(idx + 0);
	// TODO: Check
}

} // namespace wwidget
