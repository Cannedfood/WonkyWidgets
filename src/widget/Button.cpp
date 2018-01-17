#include "../../include/widget/widget/Button.hpp"

#include "../../include/widget/Canvas.hpp"
#include "../../include/widget/Attribute.hpp"

namespace widget {

Button::Button() :
	mPressed(false)
{
	align(AlignFill);
}

Button::~Button() {}

Button::Button(std::string txt) :
	Button()
{
	text(txt);
}
Button::Button(Widget* addTo) :
	Button()
{
	addTo->add(this);
}
Button::Button(Widget* addTo, std::string txt) :
	Button(txt)
{
	addTo->add(this);
}

Button* Button::text(std::string const& s) {
	if(auto* l = search<Label>()) {
		if(s.empty())
			l->remove();
		else
			l->content(s);
	}
	else if(!s.empty()) {
		add<Label>()->content(s)->align(AlignCenter);
	}
	return this;
}
std::string Button::text() {
	if(auto* l = search<Label>())
		return l->content();
	else
		return "";
}
Button* Button::onClick(std::function<void()> c) {
	onClickCallback = std::move(c);
	return this;
}
void Button::on(Click const& click) { WIDGET_M_FN_MARKER
	if(mPressed && click.up() && onClickCallback) {
		defer(onClickCallback);
	}
	mPressed = click.down();
	click.handled = true;
}

void Button::onCalculateLayout(LayoutInfo& info) {
	info = calcOverlappingLayout(5, 5);
}

void Button::onDrawBackground(Canvas& canvas) {
	if(mPressed) {
		canvas.rect(100, {0, 0, width(), height()}, rgba(0, 0, 0, 0.3));
	}
}

void Button::onDraw(Canvas& canvas) {
	canvas.box(100, {0, 0, width(), height()}, rgba(0, 0, 0, 0.3));
}

bool Button::setAttribute(std::string const& name, std::string const& value) {
	if(name == "content" || name == "text") {
		text(value); return true;
	}
	return Widget::setAttribute(name, value);
}
void Button::getAttributes(AttributeCollectorInterface& collector) {
	if(Label* l = search<Label>())
		collector("text", l->content());
	Widget::getAttributes(collector);
}

} // namespace widget
