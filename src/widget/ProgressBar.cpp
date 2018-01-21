#include "../../include/wwidget/widget/ProgressBar.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/Attribute.hpp"

#include <algorithm>

namespace wwidget {

ProgressBar::ProgressBar() :
	Widget(),
	mScale(1),
	mProgress(0),
	mProgressInterpolated(0)
{
	align(AlignFill);
}

ProgressBar::~ProgressBar() {}

ProgressBar* ProgressBar::progress(float f) {
	mProgress = f;
	return this;
}

ProgressBar* ProgressBar::scale(float f) {
	mScale = f;
	return this;
}

void ProgressBar::onDrawBackground(Canvas& canvas) {
	canvas.rect(100, {0, 0, width(), height()}, rgb(46, 42, 33));

	mProgressInterpolated = (mProgressInterpolated * 1023 + mProgress) / 1024.f;
	float f = std::min(std::max(mProgressInterpolated / scale(), 0.f), 1.f);
	canvas.rect(100, {0, 0, f * width(), height()}, rgb(217, 150, 1));
}
void ProgressBar::onDraw(Canvas& canvas) {
	// canvas.outlineRRect(100, 3, 0, 0, width(), height(), rgb(70, 70, 70));
}

bool ProgressBar::setAttribute(std::string const& name, std::string const& value) {
	if(name == "progress") {
		try { progress(std::stof(value)); return true; }
		catch(...) { return false; }
	}
	if(name == "scale") {
		try { scale(std::stof(value)); return true; }
		catch(...) { return false; }
	}
	return Widget::setAttribute(name, value);
}

void ProgressBar::getAttributes(AttributeCollectorInterface& collector) {
	collector("progress", progress());
	collector("scale", scale());
	Widget::getAttributes(collector);
}

} // namespace wwidget
