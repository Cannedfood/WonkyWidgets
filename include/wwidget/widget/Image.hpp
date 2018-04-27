#pragma once

#include "../Widget.hpp"

namespace wwidget {

class Bitmap;

class Image : public Widget {
	std::string mSource;

	uint32_t                mTint;
	std::shared_ptr<Bitmap> mImage;

protected:
	void onCalcPreferredSize(PreferredSize& to) override;
	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;
	void onAppletChanged() override;
public:
	Image();
	Image(std::string source);
	Image(Widget* addTo);
	Image(Widget* addTo, std::string source);
	~Image();

	void image(std::nullptr_t);
	void image(std::string const& source);
	void image(std::shared_ptr<Bitmap> image, std::string source = std::string());
	std::shared_ptr<Bitmap> const& image() const noexcept { return mImage; }
	std::string const& source() const noexcept;

	bool setAttribute(std::string const& name, std::string const& value) override;
	void getAttributes(AttributeCollectorInterface& collector) override;
};

} // namespace wwidget
