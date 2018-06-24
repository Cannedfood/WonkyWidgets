#include "CanvasNVG.hpp"

#include "Bitmap.hpp"

#include <cassert>

namespace wwidget {

CanvasNVG::CanvasNVG(NVGcontext* ctxt, PFNContextClose close_ctxt) :
	m_context(ctxt),
	m_close_ctxt(close_ctxt)
{
	assert(0 <= nvgCreateFont(m_context, "mono", "/usr/share/fonts/TTF/DejaVuSansMono.ttf"));
	assert(0 <= nvgCreateFont(m_context, "sans", "/usr/share/fonts/TTF/DejaVuSans.ttf"));
	assert(0 <= nvgCreateFont(m_context, "icon", "/usr/share/fonts/noto/NotoSansSymbols2-Regular.ttf"));
}
CanvasNVG::~CanvasNVG() {
	if(m_close_ctxt) {
		m_close_ctxt(m_context);
	}
}

int CanvasNVG::getHandle(std::shared_ptr<Bitmap> const& bm) {
	if(bm->mRendererProxy) {
		return (int)(size_t)bm->mRendererProxy.get();
	}
	else {
		int texture = nvgCreateImageRGBA(
			m_context,
			bm->width(), bm->height(),
			NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY,
			bm->toRGBA().data()
		);
		assert(texture >= 0);
		bm->mRendererProxy = {
			(void*)(size_t)texture,
			[this](void* vp) {
				nvgDeleteImage(m_context, (int)(size_t)vp);
			}
		};
		return texture;
	}
}

// Frame
Canvas& CanvasNVG::beginFrame(Size const& frame_size, float dpi) {
	nvgBeginFrame(m_context, frame_size.x, frame_size.y, dpi);
	return *this;
}
Canvas& CanvasNVG::endFrame() {
	nvgEndFrame(m_context);
	return *this;
}
Canvas& CanvasNVG::cancelFrame() {
	nvgCancelFrame(m_context);
	return *this;
}

// State
Canvas& CanvasNVG::pushState() {
	nvgSave(m_context);
	return *this;
}
Canvas& CanvasNVG::popState() {
	nvgRestore(m_context);
	return *this;
}
Canvas& CanvasNVG::resetState() {
	nvgReset(m_context);
	return *this;
}

// Scissor
Canvas& CanvasNVG::scissor(Rect const& area) {
	nvgScissor(m_context, area.min.x, area.min.y, area.width(), area.height());
	return *this;
}
Canvas& CanvasNVG::scissorIntersect(Rect const& area) {
	nvgIntersectScissor(m_context, area.min.x, area.min.y, area.width(), area.height());
	return *this;
}
Canvas& CanvasNVG::resetScissor() {
	nvgResetScissor(m_context);
	return *this;
}

// Transform
Canvas& CanvasNVG::resetTransform() {
	nvgResetTransform(m_context);
	return *this;
}
Canvas& CanvasNVG::translate(float x, float y) {
	nvgTranslate(m_context, x, y);
	return *this;
}
Canvas& CanvasNVG::scale    (float x, float y) {
	nvgScale(m_context, x, y);
	return *this;
}

// Properties
Canvas& CanvasNVG::lineWidth(float f) {
	nvgStrokeWidth(m_context, f);
	return *this;
}
Canvas& CanvasNVG::fillColor(Color const& color) {
	nvgFillColor(m_context, nvgRGBAf(color.r, color.g, color.b, color.a));
	return *this;
}
Canvas& CanvasNVG::fillTexture(Rect const& to, std::shared_ptr<Bitmap> const& bm, Color const& tint) {
	nvgFillPaint(m_context,
		nvgImagePattern(m_context,
			to.min.x, to.min.y, // Translation
			to.width(), to.height(), // Scale
			0, // rotation
			getHandle(bm), // image
			1 // alpha
		)
	);
	return *this;
}
Canvas& CanvasNVG::strokeColor(Color const& color) {
	nvgStrokeColor(m_context, nvgRGBAf(color.r, color.g, color.b, color.a));
	return *this;
}
Canvas& CanvasNVG::strokeTexture(Rect const& to, std::shared_ptr<Bitmap> const& bm, Color const& tint) {
	nvgStrokePaint(m_context,
		nvgImagePattern(m_context,
			to.min.x, to.min.y, // Translation
			to.width() / bm->width(), to.height() / bm->height(), // Scale
			0, // rotation
			getHandle(bm), // image
			1 // alpha
		)
	);
	return *this;
}

// Shapes
Canvas& CanvasNVG::rect(Rect const& area) {
	nvgRect(m_context, area.min.x, area.min.y, area.width(), area.height());
	return *this;
}
Canvas& CanvasNVG::rect(Rect const& area, float radius) {
	nvgRoundedRect(m_context, area.min.x, area.min.y, area.width(), area.height(), radius);
	return *this;
}
Canvas& CanvasNVG::circle(Point const& center, float r) {
	nvgCircle(m_context, center.x, center.y, r);
	return *this;
}
Canvas& CanvasNVG::elipse(Point const& center, float rx, float ry) {
	nvgEllipse(m_context, center.x, center.y, rx, ry);
	return *this;
}
Canvas& CanvasNVG::arc(
	Point const& center, float radius,
	float from_angle, float to_angle,
	bool counter_clockwise)
{
	nvgArc(m_context,
		center.x, center.y, radius,
		from_angle, to_angle,
		counter_clockwise ? NVG_CCW : NVG_CW
	);
	return *this;
}

// Path
Canvas& CanvasNVG::moveTo(Point const& p) {
	nvgMoveTo(m_context, p.x, p.y);
	return *this;
}
Canvas& CanvasNVG::lineTo(Point const& p) {
	nvgLineTo(m_context, p.x, p.y);
	return *this;
}

// Text
Canvas& CanvasNVG::registerFont(const char* name, const char* path) {
	nvgCreateFont(m_context, name, path);
	return *this;
}

Canvas& CanvasNVG::font(const char* name) {
	nvgFontFace(m_context, *name ? name : "sans");
	return *this;
}
Canvas& CanvasNVG::fontSize(float f) {
	nvgFontSize(m_context, f != 0 ? f : 18);
	return *this;
}
Canvas& CanvasNVG::fontBlur(float f) {
	nvgFontBlur(m_context, f);
	return *this;
}
Canvas& CanvasNVG::fontLetterSpacing(float f) {
	nvgTextLetterSpacing(m_context, f);
	return *this;
}
Canvas& CanvasNVG::fontLineHeight(float f) {
	nvgTextLineHeight(m_context, f);
	return *this;
}

Canvas& CanvasNVG::text(Point const& position, std::string_view txt) {
	nvgTextAlign(m_context, NVG_ALIGN_LEFT);
	nvgText(m_context, position.x, position.y, txt.data(), txt.data() + txt.size());
	return *this;
}
Canvas& CanvasNVG::textBox(Point const& position, float maxWidth, std::string_view txt) {
	nvgTextAlign(m_context, NVG_ALIGN_LEFT);
	nvgTextBox(m_context, position.x, position.y, maxWidth, txt.data(), txt.data() + txt.size());
	return *this;
}

Rect CanvasNVG::textBounds(Point const& position, std::string_view txt) {
	float bounds[4];
	nvgTextBounds(m_context, position.x, position.y, txt.data(), txt.data() + txt.size(), bounds);
	return Rect::absolute(
		bounds[0], bounds[1],
		bounds[2], bounds[3]
	);
}
Rect CanvasNVG::textBoxBounds(Point const& position, float maxWidth, std::string_view txt) {
	float bounds[4];
	nvgTextBoxBounds(m_context, position.x, position.y, maxWidth, txt.data(), txt.data() + txt.size(), bounds);
	return Rect::absolute(
		bounds[0], bounds[1],
		bounds[2], bounds[3]
	);
}
FontMetrics CanvasNVG::fontMetrics() {
	FontMetrics metrics;
	nvgTextMetrics(m_context, &metrics.ascend, &metrics.descend, &metrics.line_height);
	return metrics;
}

// Commit
Canvas& CanvasNVG::fill() {
	nvgFill(m_context);
	nvgBeginPath(m_context);
	return *this;
}
Canvas& CanvasNVG::fillPreserve() {
	nvgFill(m_context);
	return *this;
}
Canvas& CanvasNVG::stroke() {
	nvgStroke(m_context);
	nvgBeginPath(m_context);
	return *this;
}
Canvas& CanvasNVG::strokePreserve() {
	nvgStroke(m_context);
	return *this;
}

} // namespace wwidget