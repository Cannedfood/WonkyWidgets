#ifndef WIDGET_USE_FREETYPE

#include "../../include/widget/fonts/FontDescription.hpp"

#include "../../include/widget/Error.hpp"

#define STB_RECT_PACK_IMPLEMENTATION 1
#define STBRP_STATIC 1
#include "../3rd-party/stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION 1
#define STBTT_STATIC 1
#include "../3rd-party/stb_truetype.h"

// #define STB_IMAGE_IMPLEMENTATION 1
// #include "../3rd-party/stb_image.h"

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
	BitmapFont::ConstructionInfo info;
	info.fontMetrics.dpix = dpix;
	info.fontMetrics.dpiy = dpiy;
	info.fontMetrics.size = size;
	info.width  = info.height = size * 10;
	info.bitmap.resize(info.width * info.height);

	int success;
	stbtt_pack_context spc;

	success = stbtt_PackBegin(
		&spc,
		info.bitmap.data(), info.width, info.height, info.width,
		4,
		nullptr
	);
	if(!success) throw std::runtime_error("Failed rendering bitmap: Couldn't begin packing.");

	if(size < 8) {
		stbtt_PackSetOversampling(&spc, 8, 8);
	}

	std::vector<stbtt_packedchar> packedChars(126 - 33);
	success = stbtt_PackFontRange(
		&spc,
		mImpl->fontData.data(), 0, size,
		33, packedChars.size(), packedChars.data()
	);
	if(!success) throw std::runtime_error("Failed packing bitmap font");
	stbtt_PackEnd(&spc);

	info.glyphData.resize(packedChars.size());
	for(size_t i = 0; i < packedChars.size(); i++) {
		auto& a = packedChars[i];
		auto& b = info.glyphData[i];
	}

	stbi_write_bmp("test.bmp", info.width, info.height, 1, info.bitmap.data());
	// int ascend, descend, lineGap;
	// stbtt_GetFontVMetrics(&mImpl->fontInfo, &ascend, &descend, &lineGap);

	to = BitmapFont(std::move(info));
}

} // namespace widget

#endif // ifdef WIDGET_USE_FREETYPE
