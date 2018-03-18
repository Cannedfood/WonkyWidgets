#include "../../include/wwidget/widget/Slider.hpp"

#include "../../include/wwidget/Canvas.hpp"

#include <cmath>

namespace wwidget {

Slider::Slider() :
	mValue(.5f),
	mScale(1),
	mOffset(0),
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

bool Slider::onFocus(bool b, float strength) {
	return strength > 0;
}

void Slider::on(Scroll const& scroll) {
	if(scroll.clicks_y == 0) return;
	value(value() + (scroll.clicks_y * mScale) / 20);
	scroll.handled = true;
}
void Slider::on(Click const& click) {
	if((mPressed = click.down())) {
		requestFocus();
		if(click.button == 0) {
			value(positionToValue(click.x));
		}
		click.handled = true;
		return;
	}

	if(focused()) {
		removeFocus();
		click.handled = true;
		return;
	}
}
void Slider::on(Dragged const& drag) {
	if(mPressed) {
		if(drag.buttons[0])
			value(positionToValue(drag.x));
		else
			value(value() + (drag.moved_x / width()) * (mScale * .25f));
		drag.handled = true;
	}
}

float Slider::handleSize() const noexcept { return width() * .2f; }
float Slider::positionToValue(float x) const noexcept {
	float hs = handleSize();
	float w = width() - hs;
	float position = std::clamp((x - hs * .5f) / w, 0.f, 1.f);
	return offset() + scale() * powf(position, mExponent);
}
float Slider::valueToPosition(float value) const noexcept {
	float f  = std::clamp((value - offset()) / scale(), 0.f, 1.f);
	f = powf(f, 1 / mExponent);
	float hs = handleSize();
	float w  = width() - hs;
	return f * w;
}

float Slider::fractionToValue(float x) const noexcept {
	x = std::clamp(x, 0.f, 1.f);
	return powf(x, mExponent) * scale() + offset();
}
float Slider::valueToFraction(float x) const noexcept {
	return std::clamp(powf(x - offset(), 1 / mExponent) / scale(), 0.f, 1.f);
}

Slider* Slider::value(float f) {
	float min, max;
	if(mScale > 0) {
		min = mOffset;
		max = mOffset + mScale;
	}
	else {
		min = mOffset;
		max = mOffset + mScale;
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
Slider* Slider::fraction(float f) {
	mValue = fractionToValue(f);
	return this;
}
Slider* Slider::scale(float f)  { mScale = f; return this; }
Slider* Slider::exponent(float f) {
	mExponent = f;
	return this;
}
Slider* Slider::offset(float f) { mOffset = f; return this; }
Slider* Slider::range(float min, float max) {
	offset(min);
	scale(max - min);
	return this;
}
Slider* Slider::range(float min, float max, float exp) {
	range(min, max);
	exponent(exp);
	return this;
}
Slider* Slider::valueCallback(std::function<void()> fn) {
	mValueCallback = fn;
	return this;
}

void Slider::onDrawBackground(Canvas& canvas) {
	canvas.rect(100, {0, 0, width(), height()}, rgb(48, 48, 48));
	canvas.rect(100, {valueToPosition(mValue), 0, handleSize(), height()}, rgb(87, 87, 87));
}

void Slider::onDraw(Canvas& canvas) {
	if(focused())
		canvas.box(100, {0, 0, width(), height()}, rgb(215, 150, 0));
}

} // namespace wwidget
