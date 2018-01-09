#pragma once

#include <memory>

namespace widget {

class Bitmap : public std::enable_shared_from_this<Bitmap> {
private:
	friend class Canvas;
	std::shared_ptr<void> mRendererProxy;

	unsigned mWidth, mHeight;
	unsigned mFormat;
	std::unique_ptr<uint8_t[]> mData;
public:
	Bitmap();
	~Bitmap();

	inline unsigned width() const noexcept { return mWidth; }
	inline unsigned height() const noexcept { return mHeight; }
};

} // namespace widget
