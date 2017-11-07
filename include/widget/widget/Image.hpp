#pragma once

#include "../Widget.hpp"

namespace widget {

class Bitmap;

class Image : public Widget {
	std::string mSource;

	uint32_t                mTint;
	std::shared_ptr<Bitmap> mImage;

	void onDrawBackground(Canvas& canvas) override;
	void onDraw(Canvas& canvas) override;
public:
	Image();
	~Image();

	void image(std::nullptr_t);
	void image(std::string const& source);
	void image(std::shared_ptr<Bitmap> image, std::string source = std::string());
	std::string const& source() const;

	bool setAttribute(std::string const& name, std::string const& value) override;
};

} // namespace widget
