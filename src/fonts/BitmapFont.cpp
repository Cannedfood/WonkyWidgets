#include "../../include/widget/fonts/BitmapFont.hpp"

namespace widget {

BitmapFont::BitmapFont() {}
BitmapFont::BitmapFont(ConstructionInfo&& info) :
	mBitmap(std::move(info.bitmap)),
	mWidth(std::move(info.width)),
	mHeight(std::move(info.height)),
	mKerningInformation(std::move(info.kerning)),
	mGlyphData(std::move(info.glyphs)),
	mFontMetrics(std::move(info.fontMetrics))
{}

} // namespace widget
