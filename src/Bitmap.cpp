#include "../include/wwidget/Bitmap.hpp"
#include "../include/wwidget/Error.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "thirdparty/stb_image.h"

namespace wwidget {

Bitmap::Bitmap() :
	mWidth(0), mHeight(0),
	mFormat(INVALID),
	mData(nullptr),
	mRendererProxy()
{}
Bitmap::~Bitmap() {
	free();
}

void Bitmap::init(
	std::shared_ptr<uint8_t[]> data,
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
		default: throw std::runtime_error("Invalid image format");
	}
	unsigned num_values = w * h * components;
	this->init({(uint8_t*)malloc(num_values), &::free}, w, h, fmt);
	memset(mData.get(), 0, num_values);
}
void Bitmap::load(std::string const& url, Format preferredFormat) {
	int w = 0, h = 0, c = 0;

	// printf("Load Bitmap %s (%p)\n", url.c_str(), this);

	switch(preferredFormat) {
		case INVALID: c = 0; break;
		case ALPHA: c = STBI_grey; break;
		case RGB: c = STBI_rgb; break;
		case RGBA: c = STBI_rgb_alpha; break;
	}

	stbi_convert_iphone_png_to_rgb(true);
	auto data = std::shared_ptr<uint8_t[]>(
		stbi_load(url.c_str(), &w, &h, &c, c),
		&stbi_image_free
	);
	if(!data) {
		throw std::runtime_error("Failed loading '" + url + "': " + stbi_failure_reason());
	}

	free();

	Format fmt;
	switch (c) {
		case 1: fmt = ALPHA; break;
		case 3: fmt = RGB; break;
		case 4: fmt = RGBA; break;
		default: throw std::runtime_error("File has invalid number of components");
	}
	init(data, w, h, fmt);
}
void Bitmap::load(uint8_t const* data, size_t length, Format preferredFormat) {
	throw exceptions::Unimplemented();
}
void Bitmap::free() {
	// if(mData)
	// 	printf("Free Bitmap %p\n", this);
	mRendererProxy.reset();
	mData.reset();
	mWidth = mHeight = 0;
	mFormat = INVALID;
}

} // namespace wwidget
