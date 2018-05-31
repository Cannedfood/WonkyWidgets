#include "../../include/wwidget/widget/Button.hpp"

#include "../../include/wwidget/widget/Text.hpp"

#include "../../include/wwidget/Canvas.hpp"
#include "../../include/wwidget/AttributeCollector.hpp"

#include "../../include/wwidget/Context.hpp"

namespace wwidget {

struct StringExecutor {
	Widget*     widget;
	std::string command;

	void operator()() {
		widget->context()->execute(widget, command);
	}
};

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

Button* Button::onClick(std::function<void()> c) {
	mOnClick = std::move(c);
	return this;
}
Button* Button::onClick(std::string_view const& command) {
	return onClick(StringExecutor { this, std::string(command) });
}

void Button::on(Click const& click) {
	if(mPressed && click.up() && mOnClick) {
		defer(mOnClick);
	}
	mPressed = click.down();
	click.handled = true;
}

PreferredSize Button::onCalcPreferredSize() {
	return calcBoxAroundChildren(5, 5);
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
	if(name == "onclick") {
		onClick(value); return true;
	}
	return Widget::setAttribute(name, value);
}
void Button::getAttributes(AttributeCollectorInterface& collector) {
	if(collector.startSection("wwidget::Button")) {
		if(Text* l = search<Text>())
			collector("text", l->content());
		if(auto* exec = mOnClick.target<StringExecutor>()) {
			collector("onclick", exec->command);
		}
		collector.endSection();
	}

	Widget::getAttributes(collector);
}

} // namespace wwidget
