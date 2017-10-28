#include "Button.hpp"

namespace widget {

Button::Button() :
	Label()
{}

Button::~Button() {}

void Button::on(Click const& click) {
	mPressed = click.state != Click::UP;
}



} // namespace widget
