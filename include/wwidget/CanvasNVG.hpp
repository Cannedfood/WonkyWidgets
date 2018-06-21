#pragma once

#include "Attributes.hpp"
#include "Canvas.hpp"

#include <memory>

extern "C" {
	#include <nanovg.h>
} // extern "C"

namespace wwidget {

class Bitmap;

class CanvasNVG final : public Canvas {
	using PFNContextClose = void(*)(NVGcontext*);

	NVGcontext* m_context;
	PFNContextClose m_close_ctxt;
public:
	CanvasNVG(NVGcontext* ctxt, PFNContextClose close_ctxt = nullptr);
	~CanvasNVG();

	// Frame
	Canvas& beginFrame(Size const& frame_size, float dpi) override;
	Canvas& endFrame() override;
	Canvas& cancelFrame() override;

	// State
	Canvas& pushState() override;
	Canvas& popState() override;
	Canvas& resetState() override;

	// Scissor
	Canvas& scissor(Rect const& area) override;
	Canvas& scissorIntersect(Rect const& area) override;
	Canvas& resetScissor() override;

	// Transform
	Canvas& resetTransform() override;
	Canvas& translate(float x, float y) override;
	Canvas& scale    (float x, float y) override;

	// Properties
	Canvas& lineWidth(float f) override;
	Canvas& fillColor(Color const& color) override;
	Canvas& fillColor(std::shared_ptr<Bitmap> const& bm, Color const& color) override;
	Canvas& strokeColor(Color const& color) override;
	Canvas& strokeColor(std::shared_ptr<Bitmap> const& bm, Color const& color) override;

	// Shapes
	Canvas& rect(Rect const& area) override;
	Canvas& rect(Rect const& area, float radius) override;
	Canvas& circle(Point const& center, float f) override;
	Canvas& elipse(Point const& center, float rx, float ry) override;
	Canvas& arc(Point const& center, float radius, float from_angle, float to_angle, bool counter_clockwise = false) override;

	// Path
	Canvas& moveTo(Point const& p) override;
	Canvas& lineTo(Point const& p) override;

	// Text
	Canvas& text(Point const& position, std::string_view txt) override;
	Canvas& textBox(Point const& position, float maxWidth, std::string_view txt) override;

	// Commit
	Canvas& fill() override;
	Canvas& fillPreserve() override; //!< Fill, but don't reset path
	Canvas& stroke() override;
	Canvas& strokePreserve() override; //!< Stroke, but don't reset path
};

} // namespace wwidget