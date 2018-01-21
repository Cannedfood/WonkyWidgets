#pragma once

namespace wwidget {

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

	float baseline;
	float lineHeight;
	float lineGap;
	float ascend;
	float descend;
};

using GlyphKerning = float;

} // namespace wwidget
