#pragma once

namespace widget {

struct GlyphRect {
	float tx, ty;
	float x, y;
	float w, h;
	float a, b;
};

struct GlyphData : public GlyphRect {
	float advancex;
};

struct FontMetrics {
	float size;
	float dpix, dpiy;
};

using GlyphKerning = float;

} // namespace widget
