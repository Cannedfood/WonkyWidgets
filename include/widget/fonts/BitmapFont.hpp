#pragma once

#include "GlyphData.hpp"

#include <cstdint>
#include <vector>
#include <memory>

#include <unordered_map>

namespace widget {

class Bitmap;

class BitmapFont {
	std::vector<uint8_t>                       mBitmap;
	unsigned                                   mWidth, mHeight;
	std::unordered_map<uint64_t, GlyphKerning> mKerningInformation;
	std::unordered_map<uint32_t, GlyphData>    mGlyphData;
	FontMetrics                                mFontMetrics;

public:
	struct ConstructionInfo {
		std::vector<uint8_t>                       bitmap;
		unsigned                                   width, height;
		std::unordered_map<uint64_t, GlyphKerning> kerning;
		std::unordered_map<uint32_t, GlyphData>    glyphs;
		FontMetrics                                fontMetrics;
	};

	BitmapFont();
	BitmapFont(ConstructionInfo&& info);
};

} // namespace widget
