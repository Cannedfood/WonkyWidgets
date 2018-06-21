#pragma once

#include <cinttypes>
#include <memory>
#include <deque>
#include <functional>

#include "Attributes.hpp"

namespace wwidget {

class Bitmap;
class Font;

class Canvas {
public:
	// Frame
	virtual Canvas& beginFrame(Size const& frame_size, float dpi) = 0;
	virtual Canvas& endFrame() = 0;
	virtual Canvas& cancelFrame() = 0;

	// State
	virtual Canvas& pushState() = 0;
	virtual Canvas& popState() = 0;
	virtual Canvas& resetState() = 0;

	// Scissor
	virtual Canvas& scissor(Rect const& area) = 0;
	virtual Canvas& scissorIntersect(Rect const& area) = 0;
	virtual Canvas& resetScissor() = 0;

	// Transform
	virtual Canvas& resetTransform() = 0;
	virtual Canvas& translate(float x, float y) = 0;
	virtual Canvas& scale    (float x, float y) = 0;

	// Properties
	virtual Canvas& lineWidth(float f) = 0;
	virtual Canvas& fillColor(Color const& color) = 0;
	virtual Canvas& fillColor(std::shared_ptr<Bitmap> const& bm, Color const& color) = 0;
	virtual Canvas& strokeColor(Color const& color) = 0;
	virtual Canvas& strokeColor(std::shared_ptr<Bitmap> const& bm, Color const& color) = 0;

	// Shapes
	virtual Canvas& rect(Rect const& area) = 0;
	virtual Canvas& rect(Rect const& area, float radius) = 0;
	virtual Canvas& circle(Point const& center, float f) = 0;
	virtual Canvas& elipse(Point const& center, float rx, float ry) = 0;
	virtual Canvas& arc(Point const& center, float radius, float from_angle, float to_angle, bool counter_clockwise = false) = 0;

	// Path
	virtual Canvas& moveTo(Point const& p) = 0;
	virtual Canvas& lineTo(Point const& p) = 0;

	// Text
	virtual Canvas& text(Point const& position, std::string_view txt) = 0;
	virtual Canvas& textBox(Point const& position, float maxWidth, std::string_view txt) = 0;

	// Commit
	virtual Canvas& fill() = 0;
	virtual Canvas& fillPreserve() = 0; //!< Fill, but don't reset path
	virtual Canvas& stroke() = 0;
	virtual Canvas& strokePreserve() = 0; //!< Stroke, but don't reset path
};

} // namespace wwidget
