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
	stbtt_fontinfo fontInfo;
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
	std::vector<stbtt_packedchar> packedChars(127 - 32);

	Timer t;

	unsigned const padding = 2;
	bool const powersOfTwo = false;
	// Optimized for DejaVuSansMono
	unsigned preferredSize = (unsigned) (size * 5.2 + padding * 11);
	if(powersOfTwo)
		preferredSize = 0x100000000L >> __builtin_clz(preferredSize); // Next power of two

	int success;
	do {
		info.width  = info.height = preferredSize;
		info.bitmap.resize(info.width * info.height);

		success = stbtt_PackBegin(
			&spc,
			info.bitmap.data(), info.width, info.height, info.width,
			padding,
			nullptr
		);
		if(!success) throw std::runtime_error("Failed rendering bitmap: Couldn't begin packing.");

		if(size < 8) {
			stbtt_PackSetOversampling(&spc, 8, 8);
		}

		success = stbtt_PackFontRange(
			&spc,
			mImpl->fontData.data(), 0, size,
			33, packedChars.size(), packedChars.data()
		);
		stbtt_PackEnd(&spc);

		info.glyphData.resize(packedChars.size());
		for(size_t i = 0; i < packedChars.size(); i++) {
			auto& a = packedChars[i];
			auto& b = info.glyphData[i];
		}

		if(powersOfTwo) {
			preferredSize <<= 1;
		}
		else {
			preferredSize += size / 2;
		}
	} while(!(success) && ++tries < 100);

	if(!success) throw std::runtime_error("Failed packing bitmap font after trying 20 sizes");

	// printf("Took %zu retries (%fms). Size is %u(%f)\n", tries, t.millis(), preferredSize, info.fontMetrics.size);
  //
	// t.reset();
	// stbi_write_bmp("test.bmp", info.width, info.height, 1, info.bitmap.data());
	// printf("Saving took %fms\n", t.millis());

	// TODO: get font metrics
	// int ascend, descend, lineGap;
	// stbtt_GetFontVMetrics(&mImpl->fontInfo, &ascend, &descend, &lineGap);

	to = BitmapFont(std::move(info));
}

} // namespace widget

#endif // ifdef WIDGET_USE_FREETYPE
