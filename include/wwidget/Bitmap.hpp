#pragma once

#include "thirdparty/stx/shared_ptr.hpp"

#include <string>

namespace wwidget {

using namespace stx;

class Bitmap final : public enable_shared_from_this<Bitmap> {
public:
	enum Format {
		INVALID = 0,
		DEFAULT = INVALID,
		ALPHA   = 1,
		RGB     = 3,
		RGBA    = 4
	};
protected:
	friend class Canvas;

	unsigned mWidth = 0, mHeight = 0;
	Format   mFormat = INVALID;
	shared<unsigned char[]> mData;
public:
	mutable shared<void> mRendererProxy;

	Bitmap();
	~Bitmap();

	Bitmap toRGBA();

	void init(shared<unsigned char[]> data, unsigned w, unsigned h, Format fmt);
	void init(unsigned w, unsigned h, Format fmt);
	void load(std::string const& url, Format preferredFormat = DEFAULT);
	void load(uint8_t const* data, size_t length, Format preferredFormat = DEFAULT);
	void free();

	inline unsigned width()  const noexcept { return mWidth; }
	inline unsigned height() const noexcept { return mHeight; }
	inline uint8_t* data()   const noexcept { return mData.get(); }
	inline Format   format() const noexcept { return mFormat; }
};

} // namespace wwidget
