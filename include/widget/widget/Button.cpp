#include "Button.hpp"

#include "../Canvas.hpp"

namespace widget {

Button::Button() :
	Label()
{}

Button::~Button() {}

void Button::on(Click const& click) {
	mPressed = !mPressed;
}

void Button::onDrawBackground(Canvas& canvas) {
	Label::onDrawBackground(canvas);
	if(mPressed) {
		canvas.fillRect(area().x, area().y + 1, area().width - 2, area().height - 2, rgb(81, 40, 228));
	}
}

void Button::onDraw(Canvas& canvas) {
	Label::onDraw(canvas);
}

} // namespace widget
