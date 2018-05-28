#pragma once

#include "../Widget.hpp"

namespace wwidget {

class Bitmap;

class Image : public Widget {
	std::string mSource;

	bool                    mStretch;
	uint32_t                mTint;
	std::shared_ptr<Bitmap> mImage;

protected:
	void reload();
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

	void image(std::nullptr_t);
	void image(std::string const& source);
	void image(std::shared_ptr<Bitmap> image, std::string source = std::string());
	std::shared_ptr<Bitmap> const& image() const noexcept { return mImage; }
	std::string const& source() const noexcept;

	bool stretch() const noexcept { return mStretch; }
	Image* stretch(bool b) noexcept { mStretch = b; return this; }

	bool setAttribute(std::string const& name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;
};

} // namespace wwidget
