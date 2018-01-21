#include "../../include/wwidget/fonts/BitmapFont.hpp"

#include "../3rd-party/utf8.hpp"

extern "C" {
	#include <memory.h>
}

namespace wwidget {

BitmapFont::BitmapFont() {}
BitmapFont::BitmapFont(ConstructionInfo&& info) :
	BitmapFont()
{
	*this = std::move(info);
}

BitmapFont& BitmapFont::operator=(ConstructionInfo&& info) {
	std::unique_ptr<uint8_t[], void(*)(void*)> data(
		(uint8_t*)malloc(info.bitmap.size()), &::free
	);
	std::copy(info.bitmap.begin(), info.bitmap.end(), data.get());
	init(std::move(data), info.width, info.height, Format::ALPHA);
	mKerningInformation = std::move(info.kerning);
	mGlyphData = std::move(info.glyphData);
	mFontMetrics = std::move(info.fontMetrics);
	return *this;
}

void BitmapFont::render(
	std::string value,
	float& xref, float& yref,
	std::vector<Rect>& rects,
	std::vector<Rect>& texrects)
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

			if(codepoint != ' ') {
				float posx0 = x + gdata.x0;
				float posy0 = y + gdata.y0;
				float posx1 = x + gdata.x1;
				float posy1 = y + gdata.y1;

				texrects.emplace_back(Rect::absolute(
					gdata.tx0, gdata.ty0,
					gdata.tx1, gdata.ty1
				));
				rects.emplace_back(Rect::absolute(
					posx0, posy0, posx1, posy1
				));
			}

			x += gdata.xadvance;
		}
	}

	xref = x;
	yref = y;
}

void BitmapFont::render(
	std::string value,
	std::vector<Rect>& rects,
	std::vector<Rect>& texrects)
{
	float x = 0, y = 0;
	render(value, x, y, rects, texrects);
}

} // namespace wwidget
