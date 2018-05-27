#pragma once

#include <memory>

namespace wwidget {

class Bitmap : public std::enable_shared_from_this<Bitmap> {
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
	std::shared_ptr<uint8_t[]> mData;
public:
	mutable std::shared_ptr<void> mRendererProxy;

	Bitmap();
	~Bitmap();

	void init(std::shared_ptr<uint8_t[]> data, unsigned w, unsigned h, Format fmt);
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
