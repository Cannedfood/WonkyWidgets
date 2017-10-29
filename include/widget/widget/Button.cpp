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
	if(mPressed) {
		canvas.fillRRect(20, 3, area().x + 1, area().y + 1, area().width - 2, area().height - 2, rgb(81, 40, 228));
	}
	else {
		Label::onDrawBackground(canvas);
	}
}

void Button::onDraw(Canvas& canvas) {
	canvas.outlineRRect(20, 3, area().x + 1, area().y + 1, area().width - 2, area().height - 2, rgb(0, 0, 0));
}

} // namespace widget
