#pragma once

#include "GlyphData.hpp"

#include <cstdint>
#include <vector>
#include <memory>

#include <unordered_map>

#include "../Bitmap.hpp"
#include "../Canvas.hpp"

namespace wwidget {

class BitmapFont : public Bitmap {
	std::unordered_map<uint64_t, GlyphKerning> mKerningInformation;
	std::unordered_map<uint32_t, GlyphData>    mGlyphData;
	FontMetrics                                mFontMetrics;

public:
	struct ConstructionInfo {
		std::vector<uint8_t>                       bitmap;
		unsigned                                   width, height;
		std::unordered_map<uint64_t, GlyphKerning> kerning;
		std::unordered_map<uint32_t, GlyphData>    glyphData;
		FontMetrics                                fontMetrics;
	};

	BitmapFont();
	BitmapFont(ConstructionInfo&& info);
	BitmapFont& operator=(ConstructionInfo&& info);

	void render(
		std::string value,
		float& x, float& y,
		std::vector<Rect>& rects,
		std::vector<Rect>& texrects
	);
	void render(
		std::string value,
		std::vector<Rect>& rects,
		std::vector<Rect>& texrects
	);

	inline auto  width()  const { return mHeight; }
	inline auto  height() const { return mWidth; }
	inline auto& metrics() const { return mFontMetrics; }
};

} // namespace wwidget
