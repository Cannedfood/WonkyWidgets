#ifndef WIDGET_USE_FREETYPE

#include "../../include/widget/fonts/FontDescription.hpp"

#include "../../include/widget/Error.hpp"

#include "../../include/widget/debug/Timer.hpp"

#define STB_RECT_PACK_IMPLEMENTATION 1
#define STBRP_STATIC 1
#include "../3rd-party/stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION 1
#define STBTT_STATIC 1
#include "../3rd-party/stb_truetype.h"

#include "../3rd-party/stb_image.h" // Implementation already in contained in Canvas.hpp

#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include "../3rd-party/stb_image_write.h"

#include <iostream>
#include <fstream>

namespace widget {

struct FontDescription::ImplData {
	stbtt_fontinfo       fontInfo;
	std::vector<uint8_t> fontData;
};

FontDescription::FontDescription() :
	mImpl(nullptr)
{}
FontDescription::FontDescription(std::string const& path) :
	FontDescription()
{
	load(path);
}
FontDescription::FontDescription(std::istream& data) :
	FontDescription()
{
	load(data);
}
FontDescription::~FontDescription() {
	free();
}

void FontDescription::load(std::string const& path) {
	free();

	mImpl = new ImplData;

	std::ifstream stream(path, std::ios::binary);
	if(!stream)
			throw exceptions::FailedLoadingFile(path, "Couldn't open file");
	stream.unsetf(std::ios::skipws);

	// Determine stream size
	stream.seekg(0, std::ios::end);
	mImpl->fontData.resize(stream.tellg());
	if(stream.fail() || stream.eof())
		throw exceptions::FailedLoadingFile(path, "Couldn't get file size.");

	// Load data
	stream.seekg(0, std::ios::beg);
	stream.read((char*) mImpl->fontData.data(), mImpl->fontData.size());
	if(stream.fail() || stream.eof())
		throw exceptions::FailedLoadingFile(path, "Couldn't load file as a whole.");

	stbtt_InitFont(&mImpl->fontInfo, mImpl->fontData.data(), 0);
}
void FontDescription::load(std::istream& data) {
	free();

	mImpl = new ImplData;

	data.unsetf(std::ios::skipws);
	mImpl->fontData.assign(std::istreambuf_iterator<char>(data), std::istreambuf_iterator<char>());
	if(data.fail() || data.bad())
			throw exceptions::FailedLoadingFile("stream");

	stbtt_InitFont(&mImpl->fontInfo, mImpl->fontData.data(), 0);
}
void FontDescription::load(void* data, size_t length) {
	free();

	mImpl = new ImplData;

	mImpl->fontData.assign(
		(uint8_t*) data,
		((uint8_t*) data) + length
	);

	stbtt_InitFont(&mImpl->fontInfo, (unsigned char*) data, 0);
}

void FontDescription::free() {
	if(mImpl) {
		delete mImpl;
		mImpl = nullptr;
	}
}

void FontDescription::render(float dpix, float dpiy, float size, BitmapFont& to, bool cache) {
	// TODO: implement caching
	// TODO: set font metrics

	BitmapFont::ConstructionInfo info;
	info.fontMetrics.dpix = dpix;
	info.fontMetrics.dpiy = dpiy;
	info.fontMetrics.size = size;

	size_t tries = 0;
	stbtt_pack_context spc;
	constexpr unsigned range_begin = 32;
	constexpr unsigned range_end   = 127;
	std::vector<stbtt_packedchar> packedChars(range_end - range_begin);

	Timer t;

	unsigned const padding = 1;
	bool const powersOfTwo = true;
	// TODO: make the 5.2 update depending on historical results
	// Optimized for DejaVuSansMono
	unsigned preferredSize = (unsigned) (size * 5.2 + padding * 11);
	if(powersOfTwo) {
		preferredSize = unsigned(0x100000000L >> __builtin_clz(preferredSize)); // Next power of two
	}

	int success;
	do {
		info.width = info.height = preferredSize;
		info.bitmap.resize(info.width * info.height);

		success = stbtt_PackBegin(
			&spc,
			info.bitmap.data(), info.width, info.height, info.width,
			padding,
			nullptr
		);
		if(!success) throw std::runtime_error("Failed rendering bitmap: Couldn't begin packing.");

		success = stbtt_PackFontRange(
			&spc,
			mImpl->fontData.data(), 0, size,
			range_begin, packedChars.size(), packedChars.data()
		);
		stbtt_PackEnd(&spc);

		if(powersOfTwo) {
			preferredSize <<= 1;
		}
		else {
			preferredSize += size / 2;
		}
	} while(!(success) && ++tries < 500);

	if(!success) throw std::runtime_error("Failed packing bitmap font after trying 500 sizes");

	info.glyphData.reserve(packedChars.size());
	for(size_t i = 0; i < packedChars.size(); i++) {
		auto& a = packedChars[i];
		auto& b = info.glyphData[i + range_begin];
		b.xadvance = a.xadvance;
		b.tx0 = a.x0;
		b.ty0 = a.y0;
		b.tx1 = a.x1;
		b.ty1 = a.y1;
		b.x0  = a.xoff; b.y0  = a.yoff;
		b.x1  = a.xoff2; b.y1  = a.yoff2;
		// printf("%c = (%3.f %3.f|%3.f %3.f) [%3.f %3.f|%3.f %3.f] %ux%u\n",
		// 	(char) (i + range_begin),
		// 	b.x0, b.y0, b.x1, b.y1,
		// 	b.tx0, b.ty0, b.tx1, b.ty1,
		// 	info.width, info.height
		// );
	}
	// printf("Took %zu retries (%fms). Size is %ux%u, area is %u(%f)\n", tries, t.millis(), info.width, info.height, info.width * info.height, info.fontMetrics.size);
  //
	// t.reset();
	// stbi_write_bmp("test.bmp", info.width, info.height, 1, info.bitmap.data());
	// printf("Saving took %fms\n", t.millis());

	// TODO: get font metrics
	int ascend, descend, lineGap;
	stbtt_GetFontVMetrics(&mImpl->fontInfo, &ascend, &descend, &lineGap);

	float scale = stbtt_ScaleForPixelHeight(&mImpl->fontInfo, size);
	info.fontMetrics.ascend     = scale * ascend;
	info.fontMetrics.descend    = scale * descend;
	info.fontMetrics.lineGap    = scale * lineGap;
	info.fontMetrics.lineHeight = scale * (ascend + descend + lineGap);

	to = BitmapFont(std::move(info));
}

} // namespace widget

#endif // ifdef WIDGET_USE_FREETYPE
