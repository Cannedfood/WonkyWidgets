#include "../../include/wwidget/widget/Image.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/Bitmap.hpp"

#include "../../include/wwidget/AttributeCollector.hpp"

namespace wwidget {

Image::Image() :
	Widget(),
	mStretch(false),
	mTint(Color::white()),
	mMaxSize(Size::infinite())
{}

Image::Image(std::string source) :
	Image()
{
	image(source);
}
Image::Image(Widget* addTo) : Image() {
	addTo->add(this);
}
Image::Image(Widget* addTo, std::string source) : Image(source) {
	addTo->add(this);
}

Image::~Image() {}

Image::Image(Image&& other) noexcept :
	Widget(std::move(other)),
	mSource(std::move(other.mSource)),
	mStretch(other.mStretch),
	mTint(std::move(other.mTint)),
	mImage(std::move(other.mImage))
{
	other.mTint = Color::white();
	other.mStretch = false;
}
Image& Image::operator=(Image&& other) noexcept {
	Widget::operator=(std::move(other));
	mSource = std::move(other.mSource);
	mTint = other.mTint;
	other.mTint = Color::white();
	mImage = std::move(other.mImage);
	return *this;
}

void Image::load(std::string path, bool force_synchronous) {
	mSource = path;
	if(force_synchronous) {
		image(loadImage(path), std::move(path));
	}
	else {
		loadImage(&mLoadingTasks, [this, path = std::move(path)](std::shared_ptr<Bitmap> img) {
			if(img) image(std::move(img), std::move(path));
		}, mSource);
	}
}

void Image::reload(bool force_synchronous) {
	load(mSource, force_synchronous);
}

void Image::onContextChanged() {
	mImage.reset();
	if(!source().empty() && context())
		reload(false);
}
Image* Image::image(std::nullptr_t) {
	mSource.clear();
	mImage.reset();
	return this;
}
Image* Image::image(std::shared_ptr<Bitmap> image, std::string source) {
	mSource = std::move(source);
	mImage  = std::move(image);
	if(mImage) {
		if(mImage->width() != width() || mImage->height() != height()) {
			preferredSizeChanged();
		}
	}
	return this;
}
Image* Image::image(std::string const& source, bool force_synchronous) {
	mLoadingTasks.clearOwnerships();

	if(mSource != source) {
		mSource = source;
		reload(force_synchronous);
	}
	return this;
}
Image* Image::source(std::string const& source, bool force_synchronous) {
	return image(source, force_synchronous);
}
std::string const& Image::source() const noexcept {
	return mSource;
}
Image* Image::maxSize(Size size) {
	// TODO: optimize
	if(mMaxSize != size) {
		mMaxSize = size;
		preferredSizeChanged();
	}
	return this;
}
PreferredSize Image::onCalcPreferredSize() {
	PreferredSize result = Widget::onCalcPreferredSize();
	if(mImage) {
		result.pref.x = std::min(mMaxSize.x, (float)mImage->width());
		result.pref.y = std::min(mMaxSize.y, (float)mImage->height());
	}
	result.sanitize();
	return result;
}
void Image::onDrawBackground(Canvas& canvas) {
	if(!mImage) {
		canvas.rect({0, 0, width(), height()}, rgb(0, 0, 0));
		canvas.rect({0, 0, width() / 2, height() / 2}, rgb(255, 0, 255));
		canvas.rect({width() / 2, height() / 2, width() / 2, height() / 2}, rgb(255, 0, 255));
	}
}
void Image::onDraw(Canvas& canvas) {
	if(mImage) {
		if(mStretch) {
			// canvas.rect({0, 0, width(), height()}, mImage, mTint);
			canvas.rect(size());
			return;
		}

		float ratio = mImage->width() / (float) mImage->height();
		float w = height() * ratio;
		if(w > width()) {
			float h  = width() / ratio;
			float hd = height() - h;
			canvas
				.fillColor(mImage, mTint)
				.rect({0, hd * .5f, this->width(), h})
				.fill();
		}
		else {
			float wd = width() - w;
			canvas
				.fillColor(mImage, mTint)
				.rect({wd * .5f, 0, w, height()})
				.fill();
		}
	}
}
bool Image::setAttribute(std::string_view name, std::string const& value) {
	if(name == "src" || name == "source") {
		this->image(value); return true;
	}

	if(name == "stretch") {
		this->stretch(value == "true"); return true;
	}

	if(name == "max-size") {
		// TODO: parse max size
		return true;
	}

	return Widget::setAttribute(name, value);
}
void Image::getAttributes(AttributeCollectorInterface& collector) {
	if(collector.startSection("wwidget::Image")) {
		collector("src", mSource, mSource == "");
		collector("stretch", mStretch, mStretch == false);
		collector("max-size", mMaxSize.x, mMaxSize.y, mMaxSize == Size::infinite());
		collector.endSection();
	}
	Widget::getAttributes(collector);
}

} // namespace wwidget
