#include "../../include/widget/widget/Slider.hpp"

#include "../../include/widget/Canvas.hpp"

namespace widget {

Slider::Slider() :
	mValue(.5f),
	mScale(1),
	mExponent(1)
{
	align(AlignFill, AlignDefault);
}
Slider::~Slider() {}

void Slider::on(Scroll const& scroll) {
	if(scroll.clicks_y == 0) return;
	value(value() + (scroll.clicks_y * mScale) / 20);
	scroll.handled = true;
}

void Slider::on(Click const& click) {
	if(click.button == 0 && click.down()) {
		value((click.x / width()) * mScale);
		click.handled = true;
	}
}
void Slider::on(Dragged const& drag) {
	if(drag.buttons[0])
		value((drag.x / width()) * mScale);
	else
		value(value() + (drag.moved_x / width()) * (mScale * .25f));
}

Slider* Slider::value   (float f) {
	f = std::max(std::min(f, mScale), 0.f);
	if(f != mValue) {
		mValue = f;
		if(valueCallback)
			valueCallback(this, mValue);
	}
	return this;
}
Slider* Slider::scale(float f) { mScale = f; return this; }
Slider* Slider::exponent(float f) { mExponent = f; return this; }

void Slider::onDrawBackground(Canvas& canvas) {
	canvas.fillRRect(100, 3, 0, 0, width(), height(), rgb(48, 48, 48));
	float f = mValue / mScale;
	float w = width() * .2f;
	canvas.fillRRect(100, 3, f * (width() - w), 0, width() * .2f, height(), rgba(255, 255, 255, 0.19));
}

} // namespace widget
