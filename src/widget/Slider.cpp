#include "../../include/wwidget/widget/Slider.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/AttributeCollector.hpp"

#include <cmath>

namespace wwidget {

Slider::Slider() :
	mValue(.5f),
	mStart(0),
	mScale(1),
	mExponent(1)
{
	align(AlignFill);
}
Slider::Slider(Widget* addTo) :
	Slider()
{
	addTo->add(this);
}
Slider::~Slider() {}

bool Slider::onFocus(bool b, FocusType type) {
	return true;
}

void Slider::on(Scroll const& scroll) {
	if(scroll.clicks_y == 0) return;
	fraction(fraction() + scroll.clicks_y / 20);
	scroll.handled = true;
}
void Slider::on(Click const& click) {
	if((mPressed = click.down())) {
		requestFocus();
		if(click.button == 0) {
			value(positionToValue(click.position.x));
		}
		click.handled = true;
	}
	else if(focused()) {
		removeFocus();
		click.handled = true;
	}
}
void Slider::on(Dragged const& drag) {
	if(mPressed) {
		if(drag.buttons[0])
			value(positionToValue(drag.position.x));
		else
			value(value() + (drag.moved_x / width()) * (mScale * .25f));
		drag.handled = true;
	}
}

float Slider::handleSize() const noexcept { return width() * .2f; }
double Slider::positionToValue(float x) const noexcept {
	float hs = handleSize();
	float w = width() - hs;
	float position = std::clamp((x - hs * .5f) / w, 0.f, 1.f);
	return fractionToValue(position);
}
float Slider::valueToPosition(double value) const noexcept {
	float f  = valueToFraction(value);
	f = std::pow(f, 1 / mExponent);
	float hs = handleSize();
	float w  = width() - hs;
	return f * w;
}

double Slider::fractionToValue(double x) const noexcept {
	x  = std::clamp(x, 0.0, 1.0);
	x  = std::pow(x, mExponent);
	x *= scale();
	x += start();
	return x;
}
double Slider::valueToFraction(double x) const noexcept {
	x -= start();
	x /= scale();
	x  = std::pow(x, 1.0 / mExponent);
	return std::clamp(x, 0.0, 1.0);
}

bool Slider::setAttribute(std::string_view name, std::string const& value) {
	if(name == "start")    { start(std::stof(value));    return true; }
	if(name == "scale")    { scale(std::stof(value));    return true; }
	if(name == "exponent") { exponent(std::stof(value)); return true; }
	return Widget::setAttribute(name, value);
}
void Slider::getAttributes(AttributeCollectorInterface& collector) {
	if(collector.startSection("wwidget::Slider")) {
		collector("start",    (float)start(), 0);
		collector("scale",    (float)scale(), 1);
		collector("exponent", (float)exponent(), 1);
		collector.endSection();
	}
	Widget::getAttributes(collector);
}

Slider* Slider::value(double f) {
	double min, max;
	if(mScale > 0) {
		min = mStart;
		max = mStart + mScale;
	}
	else {
		min = mStart;
		max = mStart + mScale;
	}

	f = std::clamp(f, min, max);
	if(f != mValue) {
		mValue = f;
		if(mValueCallback) {
			defer(mValueCallback);
		}
	}
	return this;
}
Slider* Slider::fraction(double f) {
	float v = fractionToValue(f);
	if(v != mValue) {
		mValue = v;
		if(mValueCallback) {
			defer(mValueCallback);
		}
	}
	return this;
}
Slider* Slider::scale(double f)  { mScale = f; return this; }
Slider* Slider::exponent(double f) {
	mExponent = f;
	return this;
}
Slider* Slider::start(double f) { mStart = f; return this; }
Slider* Slider::range(double min, double max) {
	start(min);
	scale(max - min);
	return this;
}
Slider* Slider::range(double min, double max, double exp) {
	range(min, max);
	exponent(exp);
	return this;
}
Slider* Slider::valueCallback(std::function<void()> fn) {
	mValueCallback = fn;
	return this;
}

void Slider::onDrawBackground(Canvas& c) {
	c.fillColor(rgb(48, 48, 48))
	 .rect(size())
	 .fill();

	Rect handle = {valueToPosition(mValue), 0, handleSize(), height()};
	if(focused()) {
		handle.min.x += 2;
		handle.min.y += 2;
		handle.max.x -= 2;
		handle.max.y -= 2;
	}
	c.fillColor(rgb(87, 87, 87))
	 .rect(handle, 5)
	 .fill();
}

void Slider::onDraw(Canvas& c) {
	// if(focused()) {
	// 	c.strokeColor(rgb(215, 150, 0))
	// 	 .rect(size())
	// 	 .stroke();
	// }
}

} // namespace wwidget
