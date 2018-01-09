#pragma once

#include <memory>

namespace widget {

class Bitmap : public std::enable_shared_from_this<Bitmap> {
public:
	enum Format {
		INVALID,
		DEFAULT = INVALID,
		ALPHA,
		RGB,
		RGBA
	};
protected:
	friend class Canvas;
	mutable std::shared_ptr<void> mRendererProxy;

	unsigned mWidth = 0, mHeight = 0;
	Format   mFormat = INVALID;
	std::unique_ptr<uint8_t[], void(*)(void*)> mData;
public:
	Bitmap();
	~Bitmap();

	void init(std::unique_ptr<uint8_t[], void(*)(void*)> data, unsigned w, unsigned h, Format fmt);
	void load(std::string const& url, Format preferredFormat = DEFAULT);
	void load(uint8_t const* data, size_t length, Format preferredFormat = DEFAULT);
	void free();

	inline unsigned width()  const noexcept { return mWidth; }
	inline unsigned height() const noexcept { return mHeight; }
	inline uint8_t* data()   const noexcept { return mData.get(); }
	inline Format   format() const noexcept { return mFormat; }
};

} // namespace widget
