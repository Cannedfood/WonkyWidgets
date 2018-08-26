#pragma once

#include "../Widget.hpp"

namespace wwidget {

class Bitmap;

class Image : public Widget {
	std::string mSource;

	bool                    mStretch;
	uint32_t                mTint;
	shared<Bitmap> mImage;
	Owner                   mLoadingTasks;
	Size                    mMaxSize;

protected:
	void load(std::string path, bool force_synchronous);
	void reload(bool force_synchronous);
	PreferredSize onCalcPreferredSize() override;
	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;
	void onContextChanged() override;
public:
	Image();
	Image(std::string source);
	Image(Widget* addTo);
	Image(Widget* addTo, std::string source);
	~Image();

	Image(Image&& other) noexcept;
	Image& operator=(Image&& other) noexcept;

	Image* image(std::nullptr_t);
	Image* image(std::string const& source, bool force_synchronous = false);
	Image* image(shared<Bitmap> image);
	Image* image(shared<Bitmap> image, std::string source);
	shared<Bitmap> const& image() const noexcept { return mImage; }

	Image* source(std::string const& source, bool force_synchronous = false);
	std::string const& source() const noexcept;

	Size maxSize() const noexcept { return mMaxSize; }
	Image* maxSize(Size size);

	bool stretch() const noexcept { return mStretch; }
	Image* stretch(bool b) noexcept { mStretch = b; return this; }

	bool setAttribute(std::string_view name, Attribute const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;
};

} // namespace wwidget
