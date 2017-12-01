#include "../../include/widget/widget/Image.hpp"

#include "../../include/widget/Canvas.hpp"
#include "../../include/widget/providers/CanvasProvider.hpp"

namespace widget {

Image::Image() :
	Widget(),
	mTint(rgba(255, 255, 255, 1))
{}

Image::~Image() {}

void Image::image(std::nullptr_t) { WIDGET_M_FN_MARKER
	mSource.clear();
	mImage.reset();
}
void Image::image(std::shared_ptr<Bitmap> image, std::string source) { WIDGET_M_FN_MARKER
	mSource = std::move(source);
	mImage  = std::move(image);
	if(mImage) {
		if(mImage->width() != width() || mImage->height() != height()) {
			preferredSizeChanged();
		}
	}
}
void Image::image(std::string const& source) { WIDGET_M_FN_MARKER
	if(mSource == source) return;

	if(auto* provider = searchParent<CanvasProvider>()) {
		provider->canvas()->loadTexture(this, mSource, [this](auto&& ptr) {
			this->image(std::move(ptr));
		});
	}
	else {
		mSource = source;
	}
}
std::string const& Image::source() const noexcept {
	return mSource;
}
void Image::onCalculateLayout(LayoutInfo& to) { WIDGET_M_FN_MARKER
	Widget::onCalculateLayout(to);
	if(mImage) {
		to.prefx = mImage->width();
		to.prefy = mImage->height();
	}
	to.sanitize();
}
void Image::onDrawBackground(Canvas& canvas) {
	if(!mImage) {
		canvas.fillRect(0, 0, width(), height(), rgb(0, 0, 0));
		canvas.fillRect(0, 0, width() / 2, height() / 2, rgb(255, 0, 255));
		canvas.fillRect(width() / 2, height() / 2, width() / 2, height() / 2, rgb(255, 0, 255));
	}
}
void Image::onDraw(Canvas& canvas) {
	if(!mImage) {
		if(!mSource.empty()) {
			auto ptr = canvas.loadTextureNow(mSource);
			image(std::move(ptr), std::move(mSource));
		}
	}
	else {
		canvas.fillRect(0, 0, width(), height(), mImage, mTint);
	}
}
bool Image::setAttribute(std::string const& name, std::string const& value) { WIDGET_M_FN_MARKER
	if(name == "src" || name == "source") {
		this->image(value); return true;
	}
	return Widget::setAttribute(name, value);
}

} // namespace widget
