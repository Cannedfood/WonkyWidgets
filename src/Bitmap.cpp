#include "../include/widget/Bitmap.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "3rd-party/stb_image.h"

namespace widget {

Bitmap::Bitmap() :
	mRendererProxy(),
	mWidth(0), mHeight(0),
	mFormat(INVALID),
	mData(nullptr, nullptr)
{}
Bitmap::~Bitmap() {}

void Bitmap::init(
	std::unique_ptr<uint8_t[], void(*)(void*)> data,
	unsigned w, unsigned h, Format fmt)
{
	mRendererProxy.reset();
	mData = std::move(data);
	mWidth = w;
	mHeight = h;
	mFormat = fmt;
}
void Bitmap::init(unsigned w, unsigned h, Format fmt) {
	unsigned components;
	switch (fmt) {
		case ALPHA: components = 1; break;
		case RGB: components = 3; break;
		case INVALID: fmt = RGBA; // FALLTHROUGH
		case RGBA: components = 4; break;
	}
	unsigned num_values = w * h * components;
	this->init({(uint8_t*)malloc(num_values), &::free}, w, h, fmt);
	memset(mData.get(), 0, num_values);
}
void Bitmap::load(std::string const& url, Format preferredFormat) {
	int w, h, c;

	switch (preferredFormat) {
		case INVALID: c = 0; break;
		case ALPHA: c = STBI_grey; break;
		case RGB: c = STBI_rgb; break;
		case RGBA: c = STBI_rgb_alpha; break;
	}

	stbi_convert_iphone_png_to_rgb(true);
	auto data = std::unique_ptr<uint8_t[], void(*)(void*)>(
		stbi_load(url.c_str(), &w, &h, &c, c),
		&stbi_image_free
	);
	if(!data) {
		throw std::runtime_error("Failed loading '" + url + "': " + stbi_failure_reason());
	}

	free();

	switch (c) {
		case 1: mFormat = ALPHA; break;
		case 3: mFormat = RGB; break;
		case 4: mFormat = RGBA; break;
		default: throw std::runtime_error("File has invalid number of components");
	}
	mWidth  = w;
	mHeight = h;
	mData   = std::move(data);
}
void Bitmap::load(uint8_t const* data, size_t length, Format preferredFormat) {

}
void Bitmap::free() {
	mRendererProxy.reset();
	mData.reset();
	mWidth = mHeight = 0;
	mFormat = INVALID;
}

} // namespace widget
