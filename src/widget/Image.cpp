#include "../../include/wwidget/widget/Image.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/Bitmap.hpp"

#include "../../include/wwidget/Context.hpp"

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
	addTo->add(*this);
}
Image::Image(Widget* addTo, std::string source) : Image(source) {
	addTo->add(*this);
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

	if(!context()) return;

	if(force_synchronous) {
		mImage = context()->loadImage(mSource);
	}
	else {
		context()->loadImage([wself = weak_from_this(), src = mSource](shared<Bitmap> bm) {
			if(shared<Image> self = wself.lock().cast_static<Image>()) {
				if(self->mSource == src) {
					self->image(bm, std::move(src));
				}
			}
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
Image& Image::image(std::nullptr_t) {
	mSource.clear();
	mImage.reset();
	return *this;
}
Image& Image::image(shared<Bitmap> image, std::string source) {
	mSource = std::move(source);
	mImage  = std::move(image);
	if(mImage) {
		if(mImage->width() != width() || mImage->height() != height()) {
			preferredSizeChanged();
		}
	}
	return *this;
}
Image& Image::image(std::string const& source, bool force_synchronous) {
	// TODO: cancel loading tasks?
	if(mSource != source) {
		mSource = source;
		reload(force_synchronous);
	}
	return *this;
}
Image& Image::source(std::string const& source, bool force_synchronous) {
	return image(source, force_synchronous);
}
std::string const& Image::source() const noexcept {
	return mSource;
}
Image& Image::maxSize(Size size) {
	// TODO: optimize
	if(mMaxSize != size) {
		mMaxSize = size;
		preferredSizeChanged();
	}
	return *this;
}
PreferredSize Image::onCalcPreferredSize(PreferredSize const& constraint) {
	PreferredSize result = Widget::onCalcPreferredSize(constraint);
	if(mImage) {
		result.pref.x = std::min(mMaxSize.x, (float)mImage->width());
		result.pref.y = std::min(mMaxSize.y, (float)mImage->height());
	}
	result.sanitize();
	return result;
}
void Image::onDrawBackground(Canvas& c) {
	if(!mImage) {
		c.fillColor(Color::black())
		 .rect({0, 0, width(), height()})
		 .fill();
		c.fillColor(Color::eyecancer1())
		 .rect({0, 0, width() / 2, height() / 2})
		 .rect({width() / 2, height() / 2, width() / 2, height() / 2})
		 .fill();
	}
	else {
		if(mStretch) {
			// canvas.rect({0, 0, width(), height()}, mImage, mTint);
			Rect target {size()};
			c.fillTexture(target, mImage, mTint)
			 .rect(target)
			 .fill();
			return;
		}

		float ratio = mImage->width() / (float) mImage->height();
		float w = height() * ratio;
		if(w > width()) {
			float h  = width() / ratio;
			float hd = height() - h;
			Rect target {0, hd * .5f, this->width(), h};
			c.fillTexture(target, mImage, mTint)
			 .rect(target)
			 .fill();
		}
		else {
			float wd = width() - w;
			Rect target{wd * .5f, 0, w, height()};
			c.fillTexture(target, mImage, mTint)
			 .rect(target)
			 .fill();
		}
	}
}
void Image::onDraw(Canvas& c) {}
bool Image::setAttribute(std::string_view name, Attribute const& value) {
	if(name == "src" || name == "source") {
		this->image(value.toString()); return true;
	}

	if(name == "stretch") {
		this->stretch(value.toBool()); return true;
	}

	if(name == "max-size") {
		this->maxSize(value.toSize()); return true;
	}

	if(name == "tint") {
		this->tint(value.toColor()); return true;
	}

	return Widget::setAttribute(name, value);
}
void Image::getAttributes(AttributeCollectorInterface& collector) {
	if(collector.startSection("wwidget::Image")) {
		collector("src", mSource, "");
		collector("stretch", mStretch, false);
		collector("max-size", mMaxSize, Size::infinite());
		collector.endSection();
	}
	Widget::getAttributes(collector);
}

} // namespace wwidget
