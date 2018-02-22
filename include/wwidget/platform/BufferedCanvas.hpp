#pragma once

#include "../Canvas.hpp"

#include <deque>
#include <mutex>

#include <unordered_map>

namespace wwidget {

struct CanvasBuffers {
	struct SimpleVertex {
		Point    position;
		unsigned layer;
		Color    color;
	};

	struct SimpleBuffer {
		std::vector<SimpleVertex> vertices;
		std::vector<uint32_t>     indices;
	};

	struct TexturedVertex {
		Point    position;
		unsigned layer;
		Point    texcoord;
		Color    color;
	};

	struct TexturedBuffer {
		std::vector<TexturedVertex> vertices;
		std::vector<uint32_t>       indices;
	};

	SimpleBuffer lines;
	SimpleBuffer triangles;
	std::unordered_map<std::shared_ptr<Bitmap>, TexturedBuffer> textured;
};

/// WIP implementation of a buffered canvas
class BufferedCanvas : public Canvas {
public:
	struct Transform {
		float offsetx, offsety;
		float scalex, scaley;

		constexpr
		Transform() :
			Transform(0, 0, 1, 1)
		{}

		constexpr
		Transform(float xscale, float yscale) :
			offsetx(0), offsety(0),
			scalex(xscale), scaley(yscale)
		{}
		constexpr
		Transform(float xoff, float yoff, float xscale, float yscale) :
			offsetx(xoff), offsety(yoff),
			scalex(xscale), scaley(yscale)
		{}
	};

private:
	std::vector<Transform> mTransforms;
	unsigned      mLayer;
	CanvasBuffers mBuffers;
public:
	void pushTransform(Transform const& t);
	void popTransform();

	BufferedCanvas();

	// Poll the pending draw operations
	CanvasBuffers poll();

	// You'll probably want to override these
	virtual void recommendUpload(std::weak_ptr<Bitmap> bm) override;

	// Stuff from the actual canvas

	void pushViewport(float x, float y, float w, float h) override;
	void popViewport() override;

	void pushClipRect(float x, float y, float w, float h) override;
	void popClipRect() override;


	void rect(
		Rect const& area,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) override;

	void rect(
		Rect const& area,
		Rect const& subarea,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()) override;

	void rect(
		Rect const& area,
		Color const& color) override;

	void rects(
		size_t num,
		Rect const* areas,
		Rect const* texareas,
		std::shared_ptr<Bitmap> const& bm,
		Color const& tint = Color::white()
	) override;

	void rect(
		float corner_radius,
		Rect const& area,
		Color const& color) override;

	void box(
		Rect const& area,
		Color const& color) override;

	void box(
		float corner_radius,
		Rect const& area,
		Color const& color) override;

	void polygon( // Solid color
		size_t num, Point const* points, Color const& color
	) override;
	void polygon( // Vertex color
		size_t num, Point const* points, Color const* color
	) override;
	void polygon( // Solid color texture w/ texcoords
		size_t num, Point const* points, Point const* texcoords,
		std::shared_ptr<Bitmap> const& bm, Color const& tint = Color::white()
	) override;
	void linestrip(
		size_t num, Point const* points, Color const& color
	) override;
	void lineloop(
		size_t num, Point const* points, Color const& color
	) override;
};

} // namespace wwidget
