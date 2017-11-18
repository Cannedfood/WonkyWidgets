#include "../../include/widget/widget/Button.hpp"

#include "../../include/widget/Canvas.hpp"

namespace widget {

Button::Button() :
	Label(),
	mPressed(false)
{}

Button::~Button() {}

void Button::on(Click const& click) { WIDGET_M_FN_MARKER
	mPressed = click.down();
	if(click.up() && onClickCallback) onClickCallback(this);
}

void Button::onDrawBackground(Canvas& canvas) {
	if(mPressed) {
		canvas.fillRRect(20, 3, 1, 1, area().width - 2, area().height - 2, rgb(81, 40, 228));
	}
	else {
		Label::onDrawBackground(canvas);
	}
}

void Button::onDraw(Canvas& canvas) {
	canvas.outlineRRect(20, 3, 1, 1, area().width - 2, area().height - 2, rgb(0, 0, 0));
}

} // namespace widget
