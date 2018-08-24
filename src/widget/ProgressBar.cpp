#include "../../include/wwidget/widget/ProgressBar.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/AttributeCollector.hpp"

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

void ProgressBar::onDrawBackground(Canvas& c) {
	c.fillColor(rgb(46, 42, 33))
	 .rect({0, 0, width(), height()}, 5)
	 .fill();

	mProgressInterpolated = (mProgressInterpolated * 1023 + mProgress) / 1024.f;
	float f = std::min(std::max(mProgressInterpolated / scale(), 0.f), 1.f);
	c.fillColor(rgb(217, 150, 1))
	 .rect({0, 0, f * width(), height()}, 5)
	 .fill();
}
void ProgressBar::onDraw(Canvas& canvas) {
	// canvas.outlineRRect(100, 3, 0, 0, width(), height(), rgb(70, 70, 70));
}

bool ProgressBar::setAttribute(std::string_view name, Attribute const& value) {
	if(name == "progress") {
		try { progress(value.toFloat()); return true; }
		catch(...) { return false; }
	}
	if(name == "scale") {
		try { scale(value.toFloat()); return true; }
		catch(...) { return false; }
	}
	return Widget::setAttribute(name, value);
}

void ProgressBar::getAttributes(AttributeCollectorInterface& collector) {
	if(collector.startSection("wwidget::ProgressBar")) {
		collector("progress", progress(), 0);
		collector("scale", scale(), 1);
		collector.endSection();
	}
	Widget::getAttributes(collector);
}

} // namespace wwidget
