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
	if(click.up() && onClickCallback) {
		onClickCallback(this);
	}
	click.handled = true;
}

void Button::onCalculateLayout(LayoutInfo& info) {
	Widget::onCalculateLayout(info);
	LayoutInfo info2;
	Label::onCalculateLayout(info2);
	info.include(info2, 0, 0);
}

void Button::onDrawBackground(Canvas& canvas) {
	if(mPressed) {
		canvas.fillRRect(20, 3, 1, 1, width() - 2, height() - 2, rgb(81, 40, 228));
	}
	else {
		Label::onDrawBackground(canvas);
	}
}

void Button::onDraw(Canvas& canvas) {
	canvas.outlineRRect(20, 3, 1, 1, width() - 2, height() - 2, rgb(0, 0, 0));
	Label::onDraw(canvas);
}

} // namespace widget
