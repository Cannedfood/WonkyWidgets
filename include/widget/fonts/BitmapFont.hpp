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
	std::vector<GlyphData>                     mGlyphData;
	uint32_t                                   mGlyphOffset;
	FontMetrics                                mFontMetrics;

public:
	struct ConstructionInfo {
		std::vector<uint8_t>                       bitmap;
		unsigned                                   width, height;
		std::unordered_map<uint64_t, GlyphKerning> kerning;
		std::vector<GlyphData>                     glyphData;
		uint32_t                                   glyphOffset;
		FontMetrics                                fontMetrics;
	};

	BitmapFont();
	BitmapFont(ConstructionInfo&& info);

	inline auto bitmap() const { return mBitmap; }
	inline auto width()  const { return mHeight; }
	inline auto height() const { return mWidth; }

	void render(float& cursor_x, float& cursor_y);
};

} // namespace widget
