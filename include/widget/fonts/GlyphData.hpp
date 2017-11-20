#pragma once

namespace widget {

struct GlyphRect {
	float tx0, ty0, tx1, ty1;
	float x0, y0, x1, y1;
};

struct GlyphData : public GlyphRect {
	float xadvance;
};

struct FontMetrics {
	float size;
	float dpix, dpiy;
};

using GlyphKerning = float;

} // namespace widget
