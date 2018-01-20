#include "../../include/widget/widget/Slider.hpp"

#include "../../include/widget/Canvas.hpp"

#include <cmath>

namespace widget {

Slider::Slider() :
	mValue(.5f),
	mScale(1),
	mOffset(0),
	mExponent(1)
{
	align(AlignFill);
}
Slider::~Slider() {}

void Slider::on(Scroll const& scroll) {
	if(scroll.clicks_y == 0) return;
	value(value() + (scroll.clicks_y * mScale) / 20);
	scroll.handled = true;
}
void Slider::on(Click const& click) {
	if(click.button == 0) {
		if((mPressed = click.down())) {
			value(positionToValue(click.x));
		}
		click.handled = true;
	}
}
void Slider::on(Dragged const& drag) {
	if(mPressed) {
		if(drag.buttons[0]) {
				value(positionToValue(drag.x));
		}
		else {
			value(value() + (drag.moved_x / width()) * (mScale * .25f));
		}
		drag.handled = true;
	}
}

float Slider::handleSize() const noexcept { return width() * .2f; }
float Slider::positionToValue(float x) const noexcept {
	float hs = handleSize();
	float w = width() - hs;
	return offset() + scale() * powf((x - hs * .5f) / w, mExponent);
}
float Slider::valueToPosition(float value) const noexcept {
	float f  = std::clamp((value - offset()) / scale(), 0.f, 1.f);
	f = powf(f, 1 / mExponent);
	float hs = handleSize();
	float w  = width() - hs;
	return f * w;
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
		if(valueCallback)
			valueCallback(this, mValue);
	}
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

void Slider::onDrawBackground(Canvas& canvas) {
	canvas.rect(100, {0, 0, width(), height()}, rgb(48, 48, 48));
	canvas.rect(100, {valueToPosition(mValue), 0, handleSize(), height()}, rgb(87, 87, 87));
}

} // namespace widget
