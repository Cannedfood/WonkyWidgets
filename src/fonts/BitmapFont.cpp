#include "../../include/widget/fonts/BitmapFont.hpp"

#include "../3rd-party/utf8.hpp"

namespace widget {

BitmapFont::BitmapFont() {}
BitmapFont::BitmapFont(ConstructionInfo&& info) :
	mBitmap(std::move(info.bitmap)),
	mWidth(std::move(info.width)),
	mHeight(std::move(info.height)),
	mKerningInformation(std::move(info.kerning)),
	mGlyphData(std::move(info.glyphData)),
	mFontMetrics(std::move(info.fontMetrics))
{}

void BitmapFont::render(
	std::string value,
	float& xref, float& yref,
	std::vector<float>& rects,
	std::vector<float>& texrects)
{
	float x = xref, y = yref;
	const char* s = value.c_str();
	while(uint32_t codepoint = stx::utf8to32(s, &s)) {
		if(codepoint == (uint32_t)('\n')) {
			x = xref;
			y = y + metrics().lineHeight;
			continue;
		}
		auto iter = mGlyphData.find(codepoint);
		if(iter != mGlyphData.end()) {
			auto& gdata = iter->second;

			float posx0 = x + gdata.x0;
			float posy0 = y + gdata.y0;
			float posx1 = x + gdata.x1;
			float posy1 = y + gdata.y1;
			rects.reserve(8);
			texrects.reserve(8);

	#define POINT(TX, TY, PX, PY) \
			texrects.emplace_back(TX); \
			texrects.emplace_back(TY); \
			rects.emplace_back(PX); \
			rects.emplace_back(PY); \

			POINT(gdata.tx0, gdata.ty1, posx0, posy1);
			POINT(gdata.tx1, gdata.ty1, posx1, posy1);
			POINT(gdata.tx1, gdata.ty0, posx1, posy0);
			POINT(gdata.tx0, gdata.ty0, posx0, posy0);

	#undef POINT

			x += gdata.xadvance;
		}
	}

	xref = x;
	yref = y;
}

void BitmapFont::render(
	std::string value,
	std::vector<float>& rects,
	std::vector<float>& texrects)
{
	float x = 0, y = 0;
	render(value, x, y, rects, texrects);
}

} // namespace widget
