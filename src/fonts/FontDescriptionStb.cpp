#ifndef WIDGET_USE_FREETYPE

#include "../../include/widget/fonts/FontDescription.hpp"

#include "../../include/widget/Error.hpp"

#define STB_RECT_PACK_IMPLEMENTATION 1
#define STBRP_STATIC 1
#include "../3rd-party/stb_rect_pack.h"

#define STB_TRUETYPE_IMPLEMENTATION 1
#define STBTT_STATIC 1
#include "../3rd-party/stb_truetype.h"


struct ImplData {
	stbtt_fontinfo fontInfo;
	std::vector<uint8_t> fontData;
};
#define mImplData ((ImplData*&) mImpl)
#define mFontInfo mImplData->fontInfo
#define mFontData mImplData->fontData

#include <iostream>
#include <fstream>

namespace widget {

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

	mImplData = new ImplData;

	std::basic_ifstream<uint8_t> stream(path, std::ios::binary);
	if (!stream)
			throw exceptions::FailedLoadingFile(path);
	stream.unsetf(std::ios::skipws);

	// Determine stream size
	stream.seekg(0, std::ios::end);
	size_t size = stream.tellg();
	stream.seekg(0);

	// Load data and add terminating 0
	mFontData.resize(size);
	stream.read(mFontData.data(), mFontData.size());

	stbtt_InitFont(&mFontInfo, mFontData.data(), 0);
}
void FontDescription::load(std::istream& data) {
	free();

	mImplData = new ImplData;

	data.unsetf(std::ios::skipws);
	mFontData.assign(std::istreambuf_iterator<char>(data), std::istreambuf_iterator<char>());
	if(data.fail() || data.bad())
			throw exceptions::FailedLoadingFile("stream");

	stbtt_InitFont(&mFontInfo, mFontData.data(), 0);
}
void FontDescription::load(void* data, size_t length) {
	free();

	mImplData = new ImplData;

	mFontData.assign(
		(uint8_t*) data,
		((uint8_t*) data) + length
	);

	stbtt_InitFont(&mFontInfo, (unsigned char*) data, 0);
}

void FontDescription::free() {
	if(mImplData) {
		delete mImplData;
		mImplData = nullptr;
	}
}

void FontDescription::render(float dpix, float dpiy, float size, BitmapFont& to, bool cache) {
	// TODO: cache
	// TODO: render
	BitmapFont::ConstructionInfo info;
}

void FontDescription::render(
	std::string const& string,
	float dpix, float dpiy, float size,
	std::vector<uint8_t>& bitmap, unsigned w, unsigned h,
	float* posx, float* posy)
{
	throw exceptions::Unimplemented();
}

} // namespace widget

#endif // ifdef WIDGET_USE_FREETYPE
