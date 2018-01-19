#include "../../include/widget/widget/Slider.hpp"

#include "../../include/widget/Canvas.hpp"

namespace widget {

Slider::Slider() :
	mValue(.5f),
	mScale(1),
	mOffset(0)
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
	if(click.button == 0 && click.down()) {
		value(positionToValue(click.x));
		click.handled = true;
	}
}
void Slider::on(Dragged const& drag) {
	if(drag.buttons[0]) {
		value(positionToValue(drag.x));
	}
	else
		value(value() + (drag.moved_x / width()) * (mScale * .25f));
}

float Slider::handleSize() const noexcept { return width() * .2f; }
float Slider::positionToValue(float x) const noexcept {
	float hs = handleSize();
	float w = width() - hs;
	return offset() + scale() * ((x - hs * .5f) / w);
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
Slider* Slider::offset(float f) { mOffset = f; return this; }
Slider* Slider::range(float min, float max) {
	offset(min);
	scale(max - min);
	return this;
}

void Slider::onDrawBackground(Canvas& canvas) {
	canvas.rect(100, {0, 0, width(), height()}, rgb(48, 48, 48));
	float f = mValue / mScale;
	float w = handleSize();
	canvas.rect(100, {f * (width() - w), 0, width() * .2f, height()}, rgb(87, 87, 87));
}

} // namespace widget
