#include "../../include/widget/widget/Image.hpp"

#include "../../include/widget/Canvas.hpp"
#include "../../include/widget/providers/CanvasProvider.hpp"

namespace widget {

Image::Image() :
	Widget(),
	mTint(rgba(255, 255, 255, 1))
{}

Image::~Image() {}

void Image::image(std::nullptr_t) {
	mSource.clear();
	mImage.reset();
}
void Image::image(std::shared_ptr<Bitmap> image, std::string source) {
	mSource = std::move(source);
	mImage  = std::move(image);
}
void Image::image(std::string const& source) {
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
std::string const& Image::source() const {
	return mSource;
}

void Image::onDrawBackground(Canvas& canvas) {
	canvas.fillRect(0, 0, area().width, area().height, rgb(0, 0, 0));
	canvas.fillRect(0, 0, area().width / 2, area().height / 2, rgb(255, 0, 255));
	canvas.fillRect(area().width / 2, area().height / 2, area().width / 2, area().height / 2, rgb(255, 0, 255));
}
void Image::onDraw(Canvas& canvas) {
	if(!mImage) {
		if(!mSource.empty()) {
			mImage = canvas.loadTextureNow(mSource);
		}
	}
	else {
		canvas.fillRect(0, 0, area().width, area().height, mImage.get(), mTint);
	}
}

bool Image::setAttribute(std::string const& name, std::string const& value) {
	if(name == "src" || name == "source") {
		this->image(value); return true;
	}
	return Widget::setAttribute(name, value);
}

} // namespace widget
